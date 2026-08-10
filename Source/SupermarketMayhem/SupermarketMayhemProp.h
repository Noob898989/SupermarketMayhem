// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SupermarketMayhemProp.generated.h"

class UStaticMeshComponent;

/**
 *  A supermarket object that a Hider can disguise as.
 *
 *  AP4.1: a single, manually placed prop used for local testing. The actor
 *  is never destroyed while worn by a Hider - SetWornByHider only toggles
 *  its visibility/collision, so exactly one instance always represents this
 *  prop in the world and can be restored on revert.
 */
UCLASS()
class SUPERMARKETMAYHEM_API ASupermarketMayhemProp : public AActor
{
	GENERATED_BODY()

public:
	ASupermarketMayhemProp();

	/** Identifier for this prop (e.g. a DataTable row name or simple tag). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Supermarket Mayhem|Prop")
	FName GetPropId() const { return PropId; }

	/** Returns the static mesh component representing this prop's appearance. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Supermarket Mayhem|Prop")
	UStaticMeshComponent* GetPropMeshComponent() const { return PropMesh; }

	/**
	 * Toggles this prop between its normal world state and "worn by a Hider".
	 * Only hides the mesh and disables its collision - never destroys the actor -
	 * so it can always be restored via SetWornByHider(false).
	 */
	void SetWornByHider(bool bWorn);

protected:
	/** Visual/collision representation of this prop in the world. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Supermarket Mayhem|Prop", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> PropMesh;

	/** Identifier for this prop, assigned per placed instance/Blueprint. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Supermarket Mayhem|Prop")
	FName PropId;

	/** Collision state cached from PropMesh when SetWornByHider(true) is called, restored on SetWornByHider(false). */
	TEnumAsByte<ECollisionEnabled::Type> CachedCollisionEnabled;
};
