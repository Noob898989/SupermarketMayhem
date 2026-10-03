// Copyright Epic Games, Inc. All Rights Reserved.

#include "SupermarketMayhemCustomerSpawnManager.h"

#include "AI/NavigationSystemBase.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "SupermarketMayhem.h"
#include "SupermarketMayhemCustomer.h"
#include "SupermarketMayhemCustomerData.h"

ASupermarketMayhemCustomerSpawnManager::ASupermarketMayhemCustomerSpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;
	SetActorHiddenInGame(true);
}

void ASupermarketMayhemCustomerSpawnManager::Configure(int32 InMinimumCount, int32 InMaximumCount, int32 InInitialCount,
	TSubclassOf<ASupermarketMayhemCustomer> InCustomerClass, USupermarketMayhemCustomerData* InCustomerData,
	FName InSpawnTag, FName InDestinationTag, float InSpawnSpread)
{
	if (!HasAuthority() || bPopulationSpawned)
	{
		return;
	}

	MinimumCustomerCount = FMath::Clamp(InMinimumCount, 0, 32);
	MaximumCustomerCount = FMath::Clamp(InMaximumCount, MinimumCustomerCount, 32);
	InitialCustomerCount = FMath::Clamp(InInitialCount, MinimumCustomerCount, MaximumCustomerCount);
	CustomerClass = InCustomerClass;
	CustomerData = InCustomerData;
	CustomerSpawnTag = InSpawnTag;
	CustomerDestinationTag = InDestinationTag;
	SpawnSpread = FMath::Max(0.0f, InSpawnSpread);
}

void ASupermarketMayhemCustomerSpawnManager::BeginPreparationPhase()
{
	if (!HasAuthority())
	{
		return;
	}
	SpawnInitialCustomers();
	SetAllCustomerBehaviors(true, true);
}

void ASupermarketMayhemCustomerSpawnManager::BeginHuntPhase()
{
	if (HasAuthority())
	{
		SetAllCustomerBehaviors(true, false);
	}
}

void ASupermarketMayhemCustomerSpawnManager::BeginResultPhase()
{
	if (HasAuthority())
	{
		SetAllCustomerBehaviors(false, false);
	}
}

void ASupermarketMayhemCustomerSpawnManager::ReportNoise(FVector NoiseLocation, float Intensity, float HearingRange)
{
	if (!HasAuthority() || !FMath::IsFinite(Intensity) || !FMath::IsFinite(HearingRange) || HearingRange <= 0.0f)
	{
		return;
	}
	for (ASupermarketMayhemCustomer* Customer : Customers)
	{
		if (IsValid(Customer))
		{
			Customer->ReportNoiseToCustomer(NoiseLocation, Intensity, HearingRange);
		}
	}
}

void ASupermarketMayhemCustomerSpawnManager::SpawnInitialCustomers()
{
	if (bPopulationSpawned || !HasAuthority() || !GetWorld())
	{
		return;
	}
	bPopulationSpawned = true;
	if (InitialCustomerCount <= 0)
	{
		return;
	}

	TArray<AActor*> SpawnPoints;
	if (!CustomerSpawnTag.IsNone())
	{
		UGameplayStatics::GetAllActorsWithTag(this, CustomerSpawnTag, SpawnPoints);
	}
	if (SpawnPoints.IsEmpty())
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		SpawnPoints = MoveTemp(PlayerStarts);
	}
	if (SpawnPoints.IsEmpty())
	{
		UE_LOG(LogSupermarketMayhem, Warning, TEXT("Customer population was not spawned: add actors tagged '%s' or place PlayerStarts in the level."), *CustomerSpawnTag.ToString());
		return;
	}

	UClass* ClassToSpawn = CustomerClass.Get();
	if (!ClassToSpawn)
	{
		ClassToSpawn = ASupermarketMayhemCustomer::StaticClass();
	}
	for (int32 Index = 0; Index < InitialCustomerCount; ++Index)
	{
		const FTransform SpawnTransform = ChooseSpawnTransform(SpawnPoints, Index);
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		ASupermarketMayhemCustomer* Customer = GetWorld()->SpawnActor<ASupermarketMayhemCustomer>(ClassToSpawn, SpawnTransform, SpawnParameters);
		if (Customer)
		{
			Customer->SetCustomerData(CustomerData);
			Customer->SetDestinationActorTag(CustomerDestinationTag);
			Customers.Add(Customer);
		}
	}
	UE_LOG(LogSupermarketMayhem, Log, TEXT("Server spawned %d/%d configured customers."), Customers.Num(), InitialCustomerCount);
}

FTransform ASupermarketMayhemCustomerSpawnManager::ChooseSpawnTransform(const TArray<AActor*>& SpawnPoints, int32 Index) const
{
	const AActor* SpawnPoint = SpawnPoints[Index % SpawnPoints.Num()];
	FVector Location = SpawnPoint->GetActorLocation();
	if (SpawnSpread > 0.0f)
	{
		Location += FVector(FMath::FRandRange(-SpawnSpread, SpawnSpread), FMath::FRandRange(-SpawnSpread, SpawnSpread), 0.0f);
	}

	if (UNavigationSystemV1* NavigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		FNavLocation ProjectedLocation;
		if (NavigationSystem->ProjectPointToNavigation(Location, ProjectedLocation))
		{
			Location = ProjectedLocation.Location;
		}
	}
	return FTransform(SpawnPoint->GetActorRotation(), Location, FVector::OneVector);
}

void ASupermarketMayhemCustomerSpawnManager::SetAllCustomerBehaviors(bool bEnabled, bool bResetState)
{
	for (int32 Index = Customers.Num() - 1; Index >= 0; --Index)
	{
		if (!IsValid(Customers[Index]))
		{
			Customers.RemoveAtSwap(Index, 1, EAllowShrinking::No);
			continue;
		}
		Customers[Index]->SetCustomerBehaviorEnabled(bEnabled, bResetState);
	}
}
