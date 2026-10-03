// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SupermarketMayhemTypes.h"
#include "TimerManager.h"
#include "SupermarketMayhemGameMode.generated.h"

class APlayerController;
class ACharacter;
class ASupermarketMayhemGameState;
class ASupermarketMayhemPlayerState;

/**
 *  Simple GameMode for a first person game.
 *
 *  Owns the server-authoritative round flow and resolves participants from
 *  GameState::PlayerArray. RoundRoleAssignment defines the role slots for a
 *  match; its two-player default preserves the original Hider/Hunter setup.
 */
UCLASS(abstract)
class ASupermarketMayhemGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASupermarketMayhemGameMode();

	/** Called when a player logs in. Attempts centralized assignment and starts when the configured roster is ready. */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/**
	 * Server-authoritative: eliminates TargetPlayerState (only valid during
	 * the Hunt phase, only for the Hider role; no-op if already eliminated).
	 * Clears any active disguise first (reusing the existing disguise
	 * replication), then marks the PlayerState eliminated, then checks the
	 * round's win condition.
	 */
	void EliminateHider(ASupermarketMayhemPlayerState* TargetPlayerState);

protected:
	/** Duration of the Preparation phase, in seconds. */
	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Round")
	float PreparationDuration = 15.0f;

	/** Duration of the Hunt phase, in seconds. */
	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Round")
	float HuntDuration = 60.0f;

	/** Duration of the Result phase, in seconds, before the round automatically restarts at Preparation. */
	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Round")
	float ResultDuration = 8.0f;

	/** How often RoundTimeRemaining and the on-screen timer message are refreshed, in seconds. */
	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Round")
	float RoundTimeUpdateInterval = 0.1f;

	/** Returns the game's GameState cast to ASupermarketMayhemGameState, or nullptr if not yet valid. */
	ASupermarketMayhemGameState* GetSupermarketMayhemGameState() const;

	/** Assigns a role to a player's PlayerState and reports it via log and on-screen debug message. */
	void AssignRole(ASupermarketMayhemPlayerState* PlayerState, ESupermarketMayhemPlayerRole NewRole);

	/** Sets the round state on the GameState and reports the transition via log and on-screen debug message. */
	void SetRoundState(ESupermarketMayhemRoundState NewRoundState);

	/** Starts the Preparation phase: resets all Hiders, locks all Hunters and starts timers. */
	void StartPreparationPhase();

	/** Starts the Hunt phase: unlocks all Hunters and starts the phase timer. */
	void StartHuntPhase();

	/** Starts the Result phase: clears all round timers and starts the timer that restarts the round at Preparation after ResultDuration. */
	void StartResultPhase();

	/** Resets every Hider's per-round state (elimination, disguise). Does not change roles. */
	void ResetHiderRoundState();

	/** Resolves connected SupermarketMayhem player states in stable GameState order. */
	TArray<ASupermarketMayhemPlayerState*> GetConnectedPlayers() const;

	/** Assigns configured role slots to the connected players once the complete roster is present. */
	bool AssignRolesToConnectedPlayers();

	/** Returns all connected players currently assigned the requested role. */
	TArray<ASupermarketMayhemPlayerState*> GetPlayersWithRole(ESupermarketMayhemPlayerRole Role) const;

	/** Refreshes RoundTimeRemaining on the GameState and shows an on-screen debug message with the remaining time. */
	void UpdateRoundTimeRemaining();

	/** Sets all Hunter pawns' MaxWalkSpeed to 0, caching their previous values. */
	void LockHunterMovement();

	/** Restores all Hunter pawns' MaxWalkSpeed values cached by LockHunterMovement. */
	void UnlockHunterMovement();

	/** Role slots assigned in GameState player order. Configure one slot per expected player to select a 2-8 player role distribution. */
	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Round|Roles")
	TArray<ESupermarketMayhemPlayerRole> RoundRoleAssignment = { ESupermarketMayhemPlayerRole::Hider, ESupermarketMayhemPlayerRole::Hunter };

	/** MaxWalkSpeed cached from each Hunter character before locking movement. */
	TMap<TWeakObjectPtr<ACharacter>, float> CachedHunterMaxWalkSpeeds;

	/** True while the Hunter's movement is currently locked (i.e. during the Preparation phase). */
	bool bHunterMovementLocked = false;

	/** World time (seconds) at which the current phase will end; used to compute RoundTimeRemaining. */
	float CurrentPhaseEndTime = 0.0f;

	/** Timer handle for the timer that ends the current phase (Preparation -> Hunt, Hunt -> Result). */
	FTimerHandle PhaseTimerHandle;

	/** Timer handle for the recurring timer that refreshes RoundTimeRemaining and its on-screen debug message. */
	FTimerHandle RoundTimeUpdateTimerHandle;
};



