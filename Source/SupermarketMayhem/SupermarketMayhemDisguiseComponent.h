// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SupermarketMayhemDisguiseComponent.generated.h"

class ASupermarketMayhemProp;
class UStaticMeshComponent;

/**
 *  Lets a Hider disguise as a nearby ASupermarketMayhemProp.
 *
 *  AP4.1: local-only, non-replicated. Intended for ASupermarketMayhemCharacter.
 *  Disguise is a purely visual mesh swap (no capsule/collision change on the
 *  owner) and is only allowed while the owner's PlayerState role is Hider and
 *  the GameState round state is Preparation. The same interact input both
 *  disguises and reverts. Mirrors its result onto the owner's
 *  ASupermarketMayhemPlayerState via SetDisguiseState so other systems can
 *  read disguise state without depending on this component directly.
 */
UCLASS(ClassGroup = (SupermarketMayhem), meta = (BlueprintSpawnableComponent))
class SUPERMARKETMAYHEM_API USupermarketMayhemDisguiseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USupermarketMayhemDisguiseComponent();

	/** Called on Interact input. Reverts if currently disguised, otherwise tries to disguise as the traced prop. */
	UFUNCTION(BlueprintCallable, Category = "Supermarket Mayhem|Disguise")
	void TryInteract();

	/**
	 * Reacts to a replicated disguise-state change from the owner's PlayerState
	 * (called from PlayerState::OnRep_DisguiseState and Character::OnRep_PlayerState).
	 * Applies or removes cosmetics; sets no PlayerState.
	 */
	void ApplyReplicatedDisguiseState(bool bNewIsDisguised, FName NewPropId);

	/** Whether the owner is currently disguised as a prop. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Supermarket Mayhem|Disguise")
	bool IsDisguised() const { return CurrentProp != nullptr; }

	/**
	 * Forcibly clears any active disguise, e.g. when the owner is eliminated.
	 * Server-only: clears the authoritative PlayerState disguise state and
	 * immediately applies the local cosmetic revert (OnRep_DisguiseState does
	 * not fire on the authority itself, so this mirrors the same explicit
	 * state+visuals pair already used by ServerTryInteract_Implementation's
	 * "already disguised" case). No-op if not currently disguised.
	 */
	UFUNCTION(BlueprintCallable, Category = "Supermarket Mayhem|Disguise")
	void ForceRemoveDisguise();

protected:
	/** Trace range for finding an interactable prop, in cm. */
	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Disguise")
	float InteractTraceDistance = 200.0f;

	/** Server RPC: requests the server to run TryInteract's validated, authoritative logic. */
	UFUNCTION(Server, Reliable)
	void ServerTryInteract();

	/** Validates the disguise request and sets the authoritative state on the owner's PlayerState. Applies no cosmetics. */
	bool TryDisguise(ASupermarketMayhemProp* TargetProp);

	/** Clears the authoritative disguise state on the owner's PlayerState. Applies no cosmetics. */
	void RemoveDisguise();

	/** Applies the local cosmetic disguise appearance (mesh swap, prop visibility) for TargetProp. */
	void ApplyDisguiseVisuals(ASupermarketMayhemProp* TargetProp);

	/** Reverts the local cosmetic disguise appearance for TargetProp, restoring the owner's normal appearance and the prop's visibility. Does not read CurrentProp to determine the target. */
	void RemoveDisguiseVisuals(ASupermarketMayhemProp* TargetProp);

	/** Line-traces from the owner's first person camera to find an interactable prop. Returns nullptr if none found. */
	ASupermarketMayhemProp* FindInteractableProp() const;

	/** Resolves the currently placed prop actor with the given PropId, using the authoritative CurrentPropId as input. Returns nullptr if none found. */
	ASupermarketMayhemProp* FindPropById(FName PropId) const;

	/** The prop currently shown as a disguise, or nullptr if not disguised. Local cache only, never replicated. */
	UPROPERTY(Transient)
	TObjectPtr<ASupermarketMayhemProp> CurrentProp;

	/** Mesh component spawned on the owner to visually represent the current disguise. */
	UPROPERTY(Transient)
	TObjectPtr<UStaticMeshComponent> DisguiseMeshComponent;
};
