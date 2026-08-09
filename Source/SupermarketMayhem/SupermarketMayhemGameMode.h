// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SupermarketMayhemTypes.h"
#include "TimerManager.h"
#include "SupermarketMayhemGameMode.generated.h"

class APlayerController;
class ASupermarketMayhemGameState;

/**
 *  Simple GameMode for a first person game.
 *
 *  Extended for Milestone 2 with a local, non-replicated round flow for
 *  exactly two players (Hider/Hunter), intended for local PIE testing only.
 *  No networking/Steam code, no replication, no weapons/damage, no NPC AI.
 */
UCLASS(abstract)
class ASupermarketMayhemGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASupermarketMayhemGameMode();

	/** Called when a player logs in. Assigns Hider/Hunter roles to the first two players and starts the round. */
	virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
	/** Duration of the Preparation phase, in seconds. */
	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Round")
	float PreparationDuration = 15.0f;

	/** Duration of the Hunt phase, in seconds. */
	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Round")
	float HuntDuration = 60.0f;

	/** How often RoundTimeRemaining and the on-screen timer message are refreshed, in seconds. */
	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Round")
	float RoundTimeUpdateInterval = 0.1f;

	/** Returns the game's GameState cast to ASupermarketMayhemGameState, or nullptr if not yet valid. */
	ASupermarketMayhemGameState* GetSupermarketMayhemGameState() const;

	/** Assigns a role to a player's PlayerState and reports it via log and on-screen debug message. */
	void AssignRole(APlayerController* PlayerController, ESupermarketMayhemPlayerRole NewRole);

	/** Sets the round state on the GameState and reports the transition via log and on-screen debug message. */
	void SetRoundState(ESupermarketMayhemRoundState NewRoundState);

	/** Starts the Preparation phase: locks the Hunter's movement and starts the phase and update timers. */
	void StartPreparationPhase();

	/** Starts the Hunt phase: unlocks the Hunter's movement and starts the phase timer. */
	void StartHuntPhase();

	/** Starts the Result phase: clears all round timers and zeroes the remaining round time. */
	void StartResultPhase();

	/** Refreshes RoundTimeRemaining on the GameState and shows an on-screen debug message with the remaining time. */
	void UpdateRoundTimeRemaining();

	/** Sets the Hunter's pawn MaxWalkSpeed to 0, caching the previous value so it can be restored later. */
	void LockHunterMovement();

	/** Restores the Hunter's pawn MaxWalkSpeed to the value cached by LockHunterMovement. */
	void UnlockHunterMovement();

	/** PlayerController of the first player to log in, assigned the Hider role. */
	UPROPERTY(Transient)
	TObjectPtr<APlayerController> HiderController;

	/** PlayerController of the second player to log in, assigned the Hunter role. */
	UPROPERTY(Transient)
	TObjectPtr<APlayerController> HunterController;

	/** MaxWalkSpeed cached from the Hunter's pawn before locking movement; restored when unlocking. */
	float CachedHunterMaxWalkSpeed = 0.0f;

	/** True while the Hunter's movement is currently locked (i.e. during the Preparation phase). */
	bool bHunterMovementLocked = false;

	/** World time (seconds) at which the current phase will end; used to compute RoundTimeRemaining. */
	float CurrentPhaseEndTime = 0.0f;

	/** Timer handle for the timer that ends the current phase (Preparation -> Hunt, Hunt -> Result). */
	FTimerHandle PhaseTimerHandle;

	/** Timer handle for the recurring timer that refreshes RoundTimeRemaining and its on-screen debug message. */
	FTimerHandle RoundTimeUpdateTimerHandle;
};



