// Copyright Epic Games, Inc. All Rights Reserved.

#include "SupermarketMayhemCustomerAIController.h"

#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"
#include "Navigation/PathFollowingComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "SupermarketMayhem.h"
#include "SupermarketMayhemCustomer.h"
#include "SupermarketMayhemCustomerData.h"
#include "SupermarketMayhemInteractiveActor.h"

namespace
{
	constexpr float CustomerDestinationSearchRadius = 1800.0f;
	constexpr float CustomerDestinationAcceptanceRadius = 100.0f;
	constexpr float MinimumDestinationSeparation = 300.0f;
}

ASupermarketMayhemCustomerAIController::ASupermarketMayhemCustomerAIController()
{
	PrimaryActorTick.bCanEverTick = false;
	bAttachToPawn = true;
}

void ASupermarketMayhemCustomerAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	Customer = Cast<ASupermarketMayhemCustomer>(InPawn);
	if (HasAuthority() && Customer.IsValid())
	{
		SetBehaviorEnabled(true, true);
	}
}

void ASupermarketMayhemCustomerAIController::OnUnPossess()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BehaviorTimerHandle);
	}
	Customer.Reset();
	Super::OnUnPossess();
}

void ASupermarketMayhemCustomerAIController::SetBehaviorEnabled(bool bEnabled, bool bResetState)
{
	if (!HasAuthority())
	{
		return;
	}

	UWorld* World = GetWorld();
	ASupermarketMayhemCustomer* CustomerCharacter = Customer.Get();
	if (!World || !CustomerCharacter)
	{
		return;
	}
	if (bEnabled && bBehaviorEnabled && !bResetState)
	{
		return;
	}

	World->GetTimerManager().ClearTimer(BehaviorTimerHandle);
	bBehaviorEnabled = bEnabled;
	if (!bEnabled)
	{
		CurrentShoppingTarget.Reset();
		StopMovement();
		SetCustomerState(ESupermarketMayhemCustomerState::Paused);
		return;
	}

	if (bResetState)
	{
		StopMovement();
		LastDestination = FVector::ZeroVector;
		CurrentShoppingTarget.Reset();
		LastNoiseReactionTime = -1.0f;
	}
	SetCustomerState(ESupermarketMayhemCustomerState::Idle);
	ScheduleNextDestination(GetRandomIdleDuration());
}

void ASupermarketMayhemCustomerAIController::ReactToNoise(float ReactionStrength)
{
	if (!HasAuthority() || !bBehaviorEnabled || !Customer.IsValid() || ReactionStrength <= 0.0f ||
		Customer->GetCustomerState() == ESupermarketMayhemCustomerState::Paused)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World || (LastNoiseReactionTime >= 0.0f && World->GetTimeSeconds() - LastNoiseReactionTime < 0.75f))
	{
		return;
	}
	LastNoiseReactionTime = World->GetTimeSeconds();

	// Small sounds do not interrupt an NPC already walking. A strong sound
	// abandons the current destination and resumes the normal customer route.
	if (Customer->GetCustomerState() == ESupermarketMayhemCustomerState::Walk && ReactionStrength < 0.65f)
	{
		return;
	}

	World->GetTimerManager().ClearTimer(BehaviorTimerHandle);
	StopMovement();
	LastDestination = Customer->GetActorLocation();
	SetCustomerState(ESupermarketMayhemCustomerState::Idle);
	ScheduleNextDestination(FMath::FRandRange(0.15f, 0.45f));
}

void ASupermarketMayhemCustomerAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	if (!HasAuthority() || !bBehaviorEnabled || !Customer.IsValid())
	{
		return;
	}

	SetCustomerState(Result.IsSuccess()
		? ESupermarketMayhemCustomerState::Shop
		: ESupermarketMayhemCustomerState::Idle);
	if (Result.IsSuccess())
	{
		if (ASupermarketMayhemInteractiveActor* ShoppingTarget = CurrentShoppingTarget.Get())
		{
			ShoppingTarget->TryInteract(Customer.Get());
		}
	}
	CurrentShoppingTarget.Reset();
	ScheduleNextDestination(Result.IsSuccess() ? GetRandomShoppingDuration() : GetRandomIdleDuration());
}

void ASupermarketMayhemCustomerAIController::ScheduleNextDestination(float Delay)
{
	if (UWorld* World = GetWorld(); World && HasAuthority() && bBehaviorEnabled)
	{
		World->GetTimerManager().ClearTimer(BehaviorTimerHandle);
		World->GetTimerManager().SetTimer(BehaviorTimerHandle, this,
			&ASupermarketMayhemCustomerAIController::ChooseAndMoveToDestination,
			FMath::Max(0.1f, Delay), false);
	}
}

void ASupermarketMayhemCustomerAIController::ChooseAndMoveToDestination()
{
	ASupermarketMayhemCustomer* CustomerCharacter = Customer.Get();
	UWorld* World = GetWorld();
	UNavigationSystemV1* NavigationSystem = World ? FNavigationSystem::GetCurrent<UNavigationSystemV1>(World) : nullptr;
	if (!HasAuthority() || !bBehaviorEnabled || !CustomerCharacter || !NavigationSystem)
	{
		if (!NavigationSystem && CustomerCharacter && !bHasLoggedNavigationStatus)
		{
			bHasLoggedNavigationStatus = true;
			UE_LOG(LogSupermarketMayhem, Warning, TEXT("Customer %s has no navigation system; provide NavMesh coverage in the active level."), *GetNameSafe(CustomerCharacter));
		}
		if (CustomerCharacter && HasAuthority())
		{
			SetCustomerState(ESupermarketMayhemCustomerState::Idle);
			ScheduleNextDestination(GetRandomIdleDuration());
		}
		return;
	}

	TArray<AActor*> TaggedDestinations;
	const FName DestinationTag = CustomerCharacter->GetDestinationActorTag();
	UGameplayStatics::GetAllActorsWithTag(World, DestinationTag, TaggedDestinations);
	if (CustomerCharacter->GetCustomerData() && !CustomerCharacter->GetCustomerData()->PreferredDestinationTag.IsNone())
	{
		TArray<AActor*> PreferredDestinations;
		for (AActor* Destination : TaggedDestinations)
		{
			if (IsValid(Destination) && Destination->ActorHasTag(CustomerCharacter->GetCustomerData()->PreferredDestinationTag))
			{
				PreferredDestinations.Add(Destination);
			}
		}
		if (!PreferredDestinations.IsEmpty())
		{
			TaggedDestinations = MoveTemp(PreferredDestinations);
		}
	}

	FVector Destination = FVector::ZeroVector;
	bool bFoundDestination = false;
	CurrentShoppingTarget.Reset();
	if (!TaggedDestinations.IsEmpty())
	{
		for (int32 Attempt = 0; Attempt < 5; ++Attempt)
		{
			AActor* Candidate = TaggedDestinations[FMath::RandRange(0, TaggedDestinations.Num() - 1)];
			if (IsValid(Candidate) &&
				(FVector::DistSquared(Candidate->GetActorLocation(), LastDestination) > FMath::Square(MinimumDestinationSeparation) || LastDestination.IsZero()))
			{
				FNavLocation ProjectedLocation;
				if (NavigationSystem->ProjectPointToNavigation(Candidate->GetActorLocation(), ProjectedLocation))
				{
					UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(
						World, CustomerCharacter->GetActorLocation(), ProjectedLocation.Location, CustomerCharacter);
					if (Path && Path->IsValid() && !Path->IsPartial())
					{
						Destination = ProjectedLocation.Location;
						CurrentShoppingTarget = Cast<ASupermarketMayhemInteractiveActor>(Candidate);
						bFoundDestination = true;
						break;
					}
				}
			}
		}
	}

	if (!bFoundDestination)
	{
		for (int32 Attempt = 0; Attempt < 5; ++Attempt)
		{
			FNavLocation RandomLocation;
			if (NavigationSystem->GetRandomReachablePointInRadius(CustomerCharacter->GetActorLocation(),
				CustomerDestinationSearchRadius, RandomLocation) &&
				(FVector::DistSquared(RandomLocation.Location, LastDestination) > FMath::Square(MinimumDestinationSeparation) || LastDestination.IsZero()))
			{
				Destination = RandomLocation.Location;
				bFoundDestination = true;
				break;
			}
		}
	}

	if (!bFoundDestination)
	{
		if (!bHasLoggedNavigationStatus)
		{
			bHasLoggedNavigationStatus = true;
			const ANavigationData* NavData = NavigationSystem->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
			const ARecastNavMesh* RecastNavData = Cast<ARecastNavMesh>(NavData);
			FNavLocation ProjectedCustomerLocation;
			const bool bCustomerLocationProjects = NavigationSystem->ProjectPointToNavigation(CustomerCharacter->GetActorLocation(), ProjectedCustomerLocation);
			UE_LOG(LogSupermarketMayhem, Warning, TEXT("Customer %s found no reachable NavMesh destinations (NavData=%s, tiles=%d, activeTiles=%d, spawnProjects=%s)."),
				*GetNameSafe(CustomerCharacter), *GetNameSafe(NavData), RecastNavData ? RecastNavData->GetNavMeshTilesCount() : -1,
				RecastNavData ? RecastNavData->GetNumActiveTiles() : -1, bCustomerLocationProjects ? TEXT("true") : TEXT("false"));
		}
		SetCustomerState(ESupermarketMayhemCustomerState::Idle);
		ScheduleNextDestination(GetRandomIdleDuration());
		return;
	}

	LastDestination = Destination;
	SetCustomerState(ESupermarketMayhemCustomerState::Walk);
	const EPathFollowingRequestResult::Type MoveRequest = MoveToLocation(Destination,
		CustomerDestinationAcceptanceRadius, true, true, true, false, nullptr, true);
	if (MoveRequest != EPathFollowingRequestResult::RequestSuccessful)
	{
		SetCustomerState(ESupermarketMayhemCustomerState::Idle);
		ScheduleNextDestination(GetRandomIdleDuration());
	}
	else if (!bHasLoggedNavigationStatus)
	{
		bHasLoggedNavigationStatus = true;
		UE_LOG(LogSupermarketMayhem, Log, TEXT("Customer %s accepted its first navigation request."), *GetNameSafe(CustomerCharacter));
	}
}

float ASupermarketMayhemCustomerAIController::GetRandomIdleDuration() const
{
	const USupermarketMayhemCustomerData* Data = Customer.IsValid() ? Customer->GetCustomerData() : nullptr;
	const float Minimum = Data ? Data->MinimumIdleDuration : 1.0f;
	const float Maximum = Data ? Data->MaximumIdleDuration : 3.0f;
	return FMath::FRandRange(FMath::Min(Minimum, Maximum), FMath::Max(Minimum, Maximum));
}

float ASupermarketMayhemCustomerAIController::GetRandomShoppingDuration() const
{
	const USupermarketMayhemCustomerData* Data = Customer.IsValid() ? Customer->GetCustomerData() : nullptr;
	const float Minimum = Data ? Data->MinimumShoppingDuration : 3.0f;
	const float Maximum = Data ? Data->MaximumShoppingDuration : 6.0f;
	return FMath::FRandRange(FMath::Min(Minimum, Maximum), FMath::Max(Minimum, Maximum));
}

void ASupermarketMayhemCustomerAIController::SetCustomerState(ESupermarketMayhemCustomerState NewState)
{
	if (ASupermarketMayhemCustomer* CustomerCharacter = Customer.Get())
	{
		CustomerCharacter->SetCustomerState(NewState);
	}
}
