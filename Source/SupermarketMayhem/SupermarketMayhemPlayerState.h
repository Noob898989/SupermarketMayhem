// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SupermarketMayhemTypes.h"
#include "SupermarketMayhemPlayerState.generated.h"

/**
 *  Player state for Supermarket Mayhem.
 *
 *  Holds the gameplay-relevant state that describes what a player currently
 *  is doing in a match: their role (Hider/Hunter, see Docs/GAME_DESIGN.md)
 *  and, for Hiders, a mirrored view of whether they are currently disguised
 *  as a prop and which prop that is.
 *
 *  This class intentionally contains no networking code (no Replicated
 *  UPROPERTYs, no RPCs, no GetLifetimeReplicatedProps). State is kept
 *  behind getters/setters so replication can be added later without
 *  changing how the rest of the gameplay code reads/writes this state
 *  (see Docs/MULTIPLAYER.md, "Server authority": role and disguise state
 *  are expected to become server-authoritative and replicated).
 */
UCLASS()
class SUPERMARKETMAYHEM_API ASupermarketMayhemPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ASupermarketMayhemPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Returns the role currently assigned to this player. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Supermarket Mayhem|Role")
	ESupermarketMayhemPlayerRole GetCurrentRole() const { return CurrentRole; }

	/** Sets the role currently assigned to this player. */
	UFUNCTION(BlueprintCallable, Category = "Supermarket Mayhem|Role")
	void SetCurrentRole(ESupermarketMayhemPlayerRole NewRole) { CurrentRole = NewRole; }

	/** Returns whether this player is currently disguised as a prop. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Supermarket Mayhem|Disguise")
	bool IsDisguised() const { return bIsDisguised; }

	/**
	 * Returns the identifier of the prop this player is currently disguised as.
	 * NAME_None when the player is not disguised.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Supermarket Mayhem|Disguise")
	FName GetCurrentPropId() const { return CurrentPropId; }

	/**
	 * Sets the mirrored disguise state for this player.
	 * @param bNewIsDisguised	Whether the player is currently disguised.
	 * @param NewPropId			Identifier of the prop the player is disguised as. Should be NAME_None when not disguised.
	 */
	UFUNCTION(BlueprintCallable, Category = "Supermarket Mayhem|Disguise")
	void SetDisguiseState(bool bNewIsDisguised, FName NewPropId);

	/** Returns whether this player has been eliminated (caught by the Hunter) this round. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Supermarket Mayhem|Elimination")
	bool IsEliminated() const { return bIsEliminated; }

	/** Sets the authoritative elimination state for this player. */
	UFUNCTION(BlueprintCallable, Category = "Supermarket Mayhem|Elimination")
	void SetEliminationState(bool bNewIsEliminated) { bIsEliminated = bNewIsEliminated; }

protected:
	/** Current gameplay role of this player (Hider/Hunter/None). */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Supermarket Mayhem|Role", meta = (AllowPrivateAccess = "true"))
	ESupermarketMayhemPlayerRole CurrentRole = ESupermarketMayhemPlayerRole::None;

	/**
	 * Whether this player is currently disguised as a prop.
	 *
	 * This is a mirrored/read-facing flag on the player state. The
	 * authoritative disguise logic (applying the disguise, validating it,
	 * etc.) belongs to the prop/disguise system, which is a separate,
	 * later work package.
	 */
	UPROPERTY(ReplicatedUsing = OnRep_DisguiseState, BlueprintReadOnly, Category = "Supermarket Mayhem|Disguise", meta = (AllowPrivateAccess = "true"))
	bool bIsDisguised = false;

	/**
	 * Identifier of the prop this player is currently disguised as.
	 *
	 * This is a deliberately simple placeholder: a dedicated prop
	 * definition type (e.g. a USupermarketMayhemPropDefinition data asset)
	 * does not exist yet and is planned for a later work package. Using an
	 * FName here (e.g. a DataTable row name or prop tag) avoids coupling
	 * this class to a type that does not exist yet, while still allowing
	 * other systems to identify which prop is currently worn. Once the
	 * prop definition type exists, this can be extended or replaced (e.g.
	 * with a lookup helper or an additional reference), which is expected
	 * to be a small, additive change.
	 * NAME_None means "not disguised".
	 */
	UPROPERTY(ReplicatedUsing = OnRep_DisguiseState, BlueprintReadOnly, Category = "Supermarket Mayhem|Disguise", meta = (AllowPrivateAccess = "true"))
	FName CurrentPropId = NAME_None;

	/**
	 * Whether this player has been eliminated (caught by the Hunter) this round.
	 *
	 * Set server-authoritatively by ASupermarketMayhemGameMode::EliminateHider,
	 * which also calls ASupermarketMayhemCharacter::ApplyEliminatedState()
	 * directly (RepNotify does not fire on the server for its own change).
	 */
	UPROPERTY(ReplicatedUsing = OnRep_Eliminated, BlueprintReadOnly, Category = "Supermarket Mayhem|Elimination", meta = (AllowPrivateAccess = "true"))
	bool bIsEliminated = false;

	/** Reacts to a replicated disguise-state change. Finds the associated Character via its already-replicated PlayerState reference and applies/removes cosmetics accordingly. */
	UFUNCTION()
	void OnRep_DisguiseState();

	/** Reacts to a replicated elimination-state change. Finds the associated Character via its already-replicated PlayerState reference and locks its movement. */
	UFUNCTION()
	void OnRep_Eliminated();
};
