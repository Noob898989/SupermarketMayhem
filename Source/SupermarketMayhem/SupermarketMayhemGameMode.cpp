// Copyright Epic Games, Inc. All Rights Reserved.

#include "SupermarketMayhemGameMode.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "SupermarketMayhem.h"
#include "SupermarketMayhemCharacter.h"
#include "SupermarketMayhemDisguiseComponent.h"
#include "SupermarketMayhemGameState.h"
#include "SupermarketMayhemPlayerState.h"
#include "SupermarketMayhemCustomerSpawnManager.h"
#include "GameFramework/GameStateBase.h"

ASupermarketMayhemGameMode::ASupermarketMayhemGameMode()
{
	GameStateClass = ASupermarketMayhemGameState::StaticClass();
	PlayerStateClass = ASupermarketMayhemPlayerState::StaticClass();
	CustomerSpawnManagerClass = ASupermarketMayhemCustomerSpawnManager::StaticClass();
}

ASupermarketMayhemCustomerSpawnManager* ASupermarketMayhemGameMode::EnsureCustomerSpawnManager()
{
	if (!HasAuthority() || !GetWorld())
	{
		return nullptr;
	}
	if (!CustomerSpawnManager && CustomerSpawnManagerClass)
	{
		CustomerSpawnManager = GetWorld()->SpawnActor<ASupermarketMayhemCustomerSpawnManager>(CustomerSpawnManagerClass, FTransform::Identity);
		if (CustomerSpawnManager)
		{
			CustomerSpawnManager->Configure(MinimumCustomerCount, MaximumCustomerCount, InitialCustomerCount,
				CustomerClass, DefaultCustomerData, CustomerSpawnTag, CustomerDestinationTag, CustomerSpawnSpread);
		}
	}
	return CustomerSpawnManager;
}

void ASupermarketMayhemGameMode::ReportCustomerNoise(FVector NoiseLocation, float Intensity, float HearingRange)
{
	if (!HasAuthority() || !FMath::IsFinite(Intensity) || !FMath::IsFinite(HearingRange) || HearingRange <= 0.0f)
	{
		return;
	}
	const ASupermarketMayhemGameState* RoundState = GetSupermarketMayhemGameState();
	if (!RoundState || RoundState->GetCurrentRoundState() != ESupermarketMayhemRoundState::Hunt)
	{
		return;
	}
	if (ASupermarketMayhemCustomerSpawnManager* Manager = EnsureCustomerSpawnManager())
	{
		Manager->ReportNoise(NoiseLocation, Intensity, HearingRange);
	}
}

ASupermarketMayhemGameState* ASupermarketMayhemGameMode::GetSupermarketMayhemGameState() const
{
	return GetGameState<ASupermarketMayhemGameState>();
}

void ASupermarketMayhemGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (GetSupermarketMayhemGameState() && GetSupermarketMayhemGameState()->GetCurrentRoundState() == ESupermarketMayhemRoundState::WaitingToStart && AssignRolesToConnectedPlayers())
	{
		StartPreparationPhase();
	}
}

TArray<ASupermarketMayhemPlayerState*> ASupermarketMayhemGameMode::GetConnectedPlayers() const
{
	TArray<ASupermarketMayhemPlayerState*> Players;
	const ASupermarketMayhemGameState* RoundGameState = GetSupermarketMayhemGameState();
	if (!RoundGameState)
	{
		return Players;
	}

	for (APlayerState* PlayerState : RoundGameState->PlayerArray)
	{
		if (ASupermarketMayhemPlayerState* MayhemPlayerState = Cast<ASupermarketMayhemPlayerState>(PlayerState))
		{
			Players.Add(MayhemPlayerState);
		}
	}
	return Players;
}

TArray<ASupermarketMayhemPlayerState*> ASupermarketMayhemGameMode::GetPlayersWithRole(ESupermarketMayhemPlayerRole RequestedRole) const
{
	TArray<ASupermarketMayhemPlayerState*> Players;
	for (ASupermarketMayhemPlayerState* PlayerState : GetConnectedPlayers())
	{
		if (PlayerState->GetCurrentRole() == RequestedRole)
		{
			Players.Add(PlayerState);
		}
	}
	return Players;
}

bool ASupermarketMayhemGameMode::AssignRolesToConnectedPlayers()
{
	const TArray<ASupermarketMayhemPlayerState*> Players = GetConnectedPlayers();
	if (RoundRoleAssignment.Num() < 2 || Players.Num() != RoundRoleAssignment.Num())
	{
		return false;
	}

	int32 HiderCount = 0;
	int32 HunterCount = 0;
	for (int32 Index = 0; Index < Players.Num(); ++Index)
	{
		const ESupermarketMayhemPlayerRole AssignedRole = RoundRoleAssignment[Index];
		if (AssignedRole == ESupermarketMayhemPlayerRole::Hider)
		{
			++HiderCount;
		}
		else if (AssignedRole == ESupermarketMayhemPlayerRole::Hunter)
		{
			++HunterCount;
		}
		else
		{
			return false;
		}
	}
	if (HiderCount == 0 || HunterCount == 0)
	{
		return false;
	}

	for (int32 Index = 0; Index < Players.Num(); ++Index)
	{
		AssignRole(Players[Index], RoundRoleAssignment[Index]);
	}
	return true;
}

void ASupermarketMayhemGameMode::AssignRole(ASupermarketMayhemPlayerState* MayhemPlayerState, ESupermarketMayhemPlayerRole NewRole)
{
	if (!MayhemPlayerState)
	{
		return;
	}

	MayhemPlayerState->SetCurrentRole(NewRole);
	if (ASupermarketMayhemCharacter* Character = Cast<ASupermarketMayhemCharacter>(MayhemPlayerState->GetPawn()))
	{
		Character->SetHunterWeaponEquipped(NewRole == ESupermarketMayhemPlayerRole::Hunter);
	}

	const UEnum* RoleEnum = StaticEnum<ESupermarketMayhemPlayerRole>();
	const FString RoleName = RoleEnum ? RoleEnum->GetNameStringByValue(static_cast<int64>(NewRole)) : TEXT("Unknown");

	UE_LOG(LogSupermarketMayhem, Log, TEXT("[SupermarketMayhem] %s assigned role: %s"), *GetNameSafe(MayhemPlayerState), *RoleName);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("%s -> Role: %s"), *GetNameSafe(MayhemPlayerState), *RoleName));
	}
}

void ASupermarketMayhemGameMode::SetRoundState(ESupermarketMayhemRoundState NewRoundState)
{
	ASupermarketMayhemGameState* MayhemGameState = GetSupermarketMayhemGameState();
	if (!MayhemGameState)
	{
		return;
	}

	MayhemGameState->SetCurrentRoundState(NewRoundState);

	const UEnum* StateEnum = StaticEnum<ESupermarketMayhemRoundState>();
	const FString StateName = StateEnum ? StateEnum->GetNameStringByValue(static_cast<int64>(NewRoundState)) : TEXT("Unknown");

	UE_LOG(LogSupermarketMayhem, Log, TEXT("[SupermarketMayhem] Round state changed to: %s"), *StateName);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(8000, 5.0f, FColor::Yellow, FString::Printf(TEXT("Round State: %s"), *StateName));
	}
}

void ASupermarketMayhemGameMode::StartPreparationPhase()
{
	UE_LOG(LogSupermarketMayhem, Log, TEXT("[SupermarketMayhem] Starting Preparation phase (%.1f s)."), PreparationDuration);

	ResetHiderRoundState();
	for (ASupermarketMayhemPlayerState* HunterPlayerState : GetPlayersWithRole(ESupermarketMayhemPlayerRole::Hunter))
	{
		if (ASupermarketMayhemCharacter* HunterCharacter = Cast<ASupermarketMayhemCharacter>(HunterPlayerState->GetPawn()))
		{
			HunterCharacter->ResetWeaponForNewRound();
		}
	}

	SetRoundState(ESupermarketMayhemRoundState::Preparation);
	if (ASupermarketMayhemCustomerSpawnManager* CustomerManager = EnsureCustomerSpawnManager())
	{
		CustomerManager->BeginPreparationPhase();
	}

	LockHunterMovement();

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	ASupermarketMayhemGameState* MayhemGameState = GetSupermarketMayhemGameState();
	if (MayhemGameState)
	{
		MayhemGameState->SetRoundTimeRemaining(PreparationDuration);
	}

	CurrentPhaseEndTime = World->GetTimeSeconds() + PreparationDuration;

	World->GetTimerManager().ClearTimer(PhaseTimerHandle);
	World->GetTimerManager().SetTimer(PhaseTimerHandle, this, &ASupermarketMayhemGameMode::StartHuntPhase, PreparationDuration, false);

	World->GetTimerManager().ClearTimer(RoundTimeUpdateTimerHandle);
	World->GetTimerManager().SetTimer(RoundTimeUpdateTimerHandle, this, &ASupermarketMayhemGameMode::UpdateRoundTimeRemaining, RoundTimeUpdateInterval, true);
}

void ASupermarketMayhemGameMode::UnlockHunterMovement()
{
	if (!bHunterMovementLocked) return;
	for (const TPair<TWeakObjectPtr<ACharacter>, float>& Entry : CachedHunterMaxWalkSpeeds)
	{
		if (ACharacter* HunterCharacter = Entry.Key.Get())
		{
			if (UCharacterMovementComponent* MovementComponent = HunterCharacter->GetCharacterMovement())
			{
				MovementComponent->MaxWalkSpeed = Entry.Value;
			}
		}
	}
	CachedHunterMaxWalkSpeeds.Reset();
	bHunterMovementLocked = false;

	UE_LOG(LogSupermarketMayhem, Log, TEXT("[SupermarketMayhem] Hunter movement unlocked (Hunt phase)."));
}

void ASupermarketMayhemGameMode::LockHunterMovement()
{
	CachedHunterMaxWalkSpeeds.Reset();
	for (ASupermarketMayhemPlayerState* HunterState : GetPlayersWithRole(ESupermarketMayhemPlayerRole::Hunter))
	{
		if (ACharacter* HunterCharacter = Cast<ACharacter>(HunterState->GetPawn()))
		{
			if (UCharacterMovementComponent* MovementComponent = HunterCharacter->GetCharacterMovement())
			{
				CachedHunterMaxWalkSpeeds.Add(HunterCharacter, MovementComponent->MaxWalkSpeed);
				MovementComponent->MaxWalkSpeed = 0.0f;
			}
		}
	}
	bHunterMovementLocked = CachedHunterMaxWalkSpeeds.Num() > 0;

	UE_LOG(LogSupermarketMayhem, Log, TEXT("[SupermarketMayhem] Hunter movement locked (Preparation phase)."));
}

void ASupermarketMayhemGameMode::StartHuntPhase()
{
	UE_LOG(LogSupermarketMayhem, Log, TEXT("[SupermarketMayhem] Starting Hunt phase (%.1f s)."), HuntDuration);

	SetRoundState(ESupermarketMayhemRoundState::Hunt);
	if (ASupermarketMayhemCustomerSpawnManager* CustomerManager = EnsureCustomerSpawnManager())
	{
		CustomerManager->BeginHuntPhase();
	}

	UnlockHunterMovement();

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	ASupermarketMayhemGameState* MayhemGameState = GetSupermarketMayhemGameState();
	if (MayhemGameState)
	{
		MayhemGameState->SetRoundTimeRemaining(HuntDuration);
	}

	CurrentPhaseEndTime = World->GetTimeSeconds() + HuntDuration;

	World->GetTimerManager().ClearTimer(PhaseTimerHandle);
	World->GetTimerManager().SetTimer(PhaseTimerHandle, this, &ASupermarketMayhemGameMode::StartResultPhase, HuntDuration, false);

	World->GetTimerManager().ClearTimer(RoundTimeUpdateTimerHandle);
	World->GetTimerManager().SetTimer(RoundTimeUpdateTimerHandle, this, &ASupermarketMayhemGameMode::UpdateRoundTimeRemaining, RoundTimeUpdateInterval, true);
}

void ASupermarketMayhemGameMode::StartResultPhase()
{
	UE_LOG(LogSupermarketMayhem, Log, TEXT("[SupermarketMayhem] Starting Result phase (%.1f s)."), ResultDuration);

	SetRoundState(ESupermarketMayhemRoundState::Result);
	if (ASupermarketMayhemCustomerSpawnManager* CustomerManager = EnsureCustomerSpawnManager())
	{
		CustomerManager->BeginResultPhase();
	}

	UnlockHunterMovement();
	for (ASupermarketMayhemPlayerState* HunterPlayerState : GetPlayersWithRole(ESupermarketMayhemPlayerRole::Hunter))
	{
		if (ASupermarketMayhemCharacter* HunterCharacter = Cast<ASupermarketMayhemCharacter>(HunterPlayerState->GetPawn()))
		{
			HunterCharacter->CancelWeaponReload();
		}
	}

	bool bAnyHiderEscaped = false;
	for (ASupermarketMayhemPlayerState* HiderPlayerState : GetPlayersWithRole(ESupermarketMayhemPlayerRole::Hider))
	{
		if (!HiderPlayerState->IsEliminated())
		{
			bAnyHiderEscaped = true;
			UE_LOG(LogSupermarketMayhem, Log, TEXT("[SupermarketMayhem] %s escaped!"), *GetNameSafe(HiderPlayerState));
		}
	}
	if (bAnyHiderEscaped && GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Hider(s) escaped!"));

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	ASupermarketMayhemGameState* MayhemGameState = GetSupermarketMayhemGameState();
	if (MayhemGameState)
	{
		MayhemGameState->SetRoundTimeRemaining(ResultDuration);
	}

	CurrentPhaseEndTime = World->GetTimeSeconds() + ResultDuration;

	World->GetTimerManager().ClearTimer(PhaseTimerHandle);
	World->GetTimerManager().SetTimer(PhaseTimerHandle, this, &ASupermarketMayhemGameMode::StartPreparationPhase, ResultDuration, false);

	World->GetTimerManager().ClearTimer(RoundTimeUpdateTimerHandle);
	World->GetTimerManager().SetTimer(RoundTimeUpdateTimerHandle, this, &ASupermarketMayhemGameMode::UpdateRoundTimeRemaining, RoundTimeUpdateInterval, true);
}

void ASupermarketMayhemGameMode::EliminateHider(ASupermarketMayhemPlayerState* TargetPlayerState)
{
	if (!TargetPlayerState || TargetPlayerState->IsEliminated())
	{
		return;
	}

	ASupermarketMayhemGameState* MayhemGameState = GetSupermarketMayhemGameState();
	if (!MayhemGameState ||
		MayhemGameState->GetCurrentRoundState() != ESupermarketMayhemRoundState::Hunt)
	{
		return;
	}

	if (TargetPlayerState->GetCurrentRole() != ESupermarketMayhemPlayerRole::Hider)
	{
		return;
	}

	if (TargetPlayerState->IsDisguised())
	{
		if (ASupermarketMayhemCharacter* TargetCharacter = Cast<ASupermarketMayhemCharacter>(TargetPlayerState->GetPawn()))
		{
			if (USupermarketMayhemDisguiseComponent* TargetDisguiseComponent = TargetCharacter->GetDisguiseComponent())
			{
				TargetDisguiseComponent->ForceRemoveDisguise();
			}
		}
	}

	TargetPlayerState->SetEliminationState(true);

	if (ASupermarketMayhemCharacter* EliminatedCharacter = Cast<ASupermarketMayhemCharacter>(TargetPlayerState->GetPawn()))
	{
		EliminatedCharacter->ApplyEliminatedState();
	}

	UE_LOG(LogSupermarketMayhem, Log, TEXT("[SupermarketMayhem] %s eliminated."), *GetNameSafe(TargetPlayerState));

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%s eliminated!"), *GetNameSafe(TargetPlayerState)));
	}

	bool bAnyHiderRemains = false;
	for (const ASupermarketMayhemPlayerState* HiderState : GetPlayersWithRole(ESupermarketMayhemPlayerRole::Hider))
	{
		if (!HiderState->IsEliminated())
		{
			bAnyHiderRemains = true;
			break;
		}
	}
	if (!bAnyHiderRemains) StartResultPhase();
}

void ASupermarketMayhemGameMode::ResetHiderRoundState()
{
	for (ASupermarketMayhemPlayerState* HiderPlayerState : GetPlayersWithRole(ESupermarketMayhemPlayerRole::Hider))
	{
		if (ASupermarketMayhemCharacter* HiderCharacter = Cast<ASupermarketMayhemCharacter>(HiderPlayerState->GetPawn()))
		{
			if (USupermarketMayhemDisguiseComponent* DisguiseComponent = HiderCharacter->GetDisguiseComponent())
			{
				DisguiseComponent->ForceRemoveDisguise();
			}

			if (HiderPlayerState->IsEliminated()) HiderCharacter->ClearEliminatedState();
		}
		HiderPlayerState->SetEliminationState(false);
	}
}

void ASupermarketMayhemGameMode::UpdateRoundTimeRemaining()
{
	ASupermarketMayhemGameState* MayhemGameState = GetSupermarketMayhemGameState();
	UWorld* World = GetWorld();
	if (!MayhemGameState || !World)
	{
		return;
	}

	const float TimeRemaining = FMath::Max(0.0f, CurrentPhaseEndTime - World->GetTimeSeconds());
	MayhemGameState->SetRoundTimeRemaining(TimeRemaining);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(8001, RoundTimeUpdateInterval * 2.0f, FColor::Green, FString::Printf(TEXT("Time Remaining: %.1f s"), TimeRemaining));
	}
}
