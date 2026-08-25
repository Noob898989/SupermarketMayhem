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

ASupermarketMayhemGameMode::ASupermarketMayhemGameMode()
{
	GameStateClass = ASupermarketMayhemGameState::StaticClass();
	PlayerStateClass = ASupermarketMayhemPlayerState::StaticClass();
}

ASupermarketMayhemGameState* ASupermarketMayhemGameMode::GetSupermarketMayhemGameState() const
{
	return GetGameState<ASupermarketMayhemGameState>();
}

void ASupermarketMayhemGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (!HiderController)
	{
		HiderController = NewPlayer;
		AssignRole(HiderController, ESupermarketMayhemPlayerRole::Hider);
	}
	else if (!HunterController)
	{
		HunterController = NewPlayer;
		AssignRole(HunterController, ESupermarketMayhemPlayerRole::Hunter);
		StartPreparationPhase();
	}
}

void ASupermarketMayhemGameMode::AssignRole(APlayerController* PlayerController, ESupermarketMayhemPlayerRole NewRole)
{
	if (!PlayerController)
	{
		return;
	}

	ASupermarketMayhemPlayerState* MayhemPlayerState = PlayerController->GetPlayerState<ASupermarketMayhemPlayerState>();
	if (!MayhemPlayerState)
	{
		return;
	}

	MayhemPlayerState->SetCurrentRole(NewRole);

	const UEnum* RoleEnum = StaticEnum<ESupermarketMayhemPlayerRole>();
	const FString RoleName = RoleEnum ? RoleEnum->GetNameStringByValue(static_cast<int64>(NewRole)) : TEXT("Unknown");

	UE_LOG(LogSupermarketMayhem, Log, TEXT("[SupermarketMayhem] %s assigned role: %s"), *GetNameSafe(PlayerController), *RoleName);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("%s -> Role: %s"), *GetNameSafe(PlayerController), *RoleName));
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

	SetRoundState(ESupermarketMayhemRoundState::Preparation);

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
	if (!bHunterMovementLocked)
	{
		return;
	}

	if (!HunterController)
	{
		return;
	}

	APawn* HunterPawn = HunterController->GetPawn();
	if (!HunterPawn)
	{
		return;
	}

	ACharacter* HunterCharacter = Cast<ACharacter>(HunterPawn);
	if (!HunterCharacter)
	{
		return;
	}

	UCharacterMovementComponent* MovementComponent = HunterCharacter->GetCharacterMovement();
	if (!MovementComponent)
	{
		return;
	}

	MovementComponent->MaxWalkSpeed = CachedHunterMaxWalkSpeed;
	bHunterMovementLocked = false;

	UE_LOG(LogSupermarketMayhem, Log, TEXT("[SupermarketMayhem] Hunter movement unlocked (Hunt phase)."));
}

void ASupermarketMayhemGameMode::LockHunterMovement()
{
	if (!HunterController)
	{
		return;
	}

	APawn* HunterPawn = HunterController->GetPawn();
	if (!HunterPawn)
	{
		return;
	}

	ACharacter* HunterCharacter = Cast<ACharacter>(HunterPawn);
	if (!HunterCharacter)
	{
		return;
	}

	UCharacterMovementComponent* MovementComponent = HunterCharacter->GetCharacterMovement();
	if (!MovementComponent)
	{
		return;
	}

	CachedHunterMaxWalkSpeed = MovementComponent->MaxWalkSpeed;
	MovementComponent->MaxWalkSpeed = 0.0f;
	bHunterMovementLocked = true;

	UE_LOG(LogSupermarketMayhem, Log, TEXT("[SupermarketMayhem] Hunter movement locked (Preparation phase)."));
}

void ASupermarketMayhemGameMode::StartHuntPhase()
{
	UE_LOG(LogSupermarketMayhem, Log, TEXT("[SupermarketMayhem] Starting Hunt phase (%.1f s)."), HuntDuration);

	SetRoundState(ESupermarketMayhemRoundState::Hunt);

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

	UnlockHunterMovement();

	if (HiderController)
	{
		if (ASupermarketMayhemPlayerState* HiderPlayerState = HiderController->GetPlayerState<ASupermarketMayhemPlayerState>())
		{
			if (!HiderPlayerState->IsEliminated())
			{
				UE_LOG(LogSupermarketMayhem, Log, TEXT("[SupermarketMayhem] %s escaped!"), *GetNameSafe(HiderPlayerState));

				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Hider escaped! (%s)"), *GetNameSafe(HiderPlayerState)));
				}
			}
		}
	}

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

	// The round system is currently hardcoded to a single Hider (HiderController,
	// see Docs/DECISIONS.md O003), so eliminating the one valid Hider target
	// above always means "all Hiders are eliminated" - end the Hunt immediately.
	StartResultPhase();
}

void ASupermarketMayhemGameMode::ResetHiderRoundState()
{
	if (!HiderController)
	{
		return;
	}

	ASupermarketMayhemPlayerState* HiderPlayerState = HiderController->GetPlayerState<ASupermarketMayhemPlayerState>();
	if (!HiderPlayerState)
	{
		return;
	}

	if (ASupermarketMayhemCharacter* HiderCharacter = Cast<ASupermarketMayhemCharacter>(HiderPlayerState->GetPawn()))
	{
		if (USupermarketMayhemDisguiseComponent* DisguiseComponent = HiderCharacter->GetDisguiseComponent())
		{
			DisguiseComponent->ForceRemoveDisguise();
		}

		if (HiderPlayerState->IsEliminated())
		{
			HiderCharacter->ClearEliminatedState();
		}
	}

	HiderPlayerState->SetEliminationState(false);
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
