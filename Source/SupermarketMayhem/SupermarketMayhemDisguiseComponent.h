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

	/** Whether the owner is currently disguised as a prop. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Supermarket Mayhem|Disguise")
	bool IsDisguised() const { return CurrentProp != nullptr; }

protected:
	/** Trace range for finding an interactable prop, in cm. */
	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Disguise")
	float InteractTraceDistance = 200.0f;

	/** Attempts to disguise the owner as TargetProp. Fails silently if role/round-state checks do not pass. */
	bool TryDisguise(ASupermarketMayhemProp* TargetProp);

	/** Reverts an active disguise, restoring the owner's normal appearance and the prop's visibility. */
	void RemoveDisguise();

	/** Line-traces from the owner's first person camera to find an interactable prop. Returns nullptr if none found. */
	ASupermarketMayhemProp* FindInteractableProp() const;

	/** The prop currently worn as a disguise, or nullptr if not disguised. */
	UPROPERTY(Transient)
	TObjectPtr<ASupermarketMayhemProp> CurrentProp;

	/** Mesh component spawned on the owner to visually represent the current disguise. */
	UPROPERTY(Transient)
	TObjectPtr<UStaticMeshComponent> DisguiseMeshComponent;
};
