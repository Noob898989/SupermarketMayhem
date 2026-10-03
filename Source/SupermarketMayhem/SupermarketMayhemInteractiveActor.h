// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SupermarketMayhemInteractiveActor.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

/** Reusable server-authoritative base for player- and customer-facing world objects. */
UCLASS(Blueprintable)
class SUPERMARKETMAYHEM_API ASupermarketMayhemInteractiveActor : public AActor
{
	GENERATED_BODY()

public:
	ASupermarketMayhemInteractiveActor();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "Supermarket Mayhem|Interaction")
	bool IsInteractionEnabled() const { return bInteractionEnabled; }

	UFUNCTION(BlueprintPure, Category = "Supermarket Mayhem|Interaction")
	FText GetInteractionPrompt() const;

	UFUNCTION(BlueprintPure, Category = "Supermarket Mayhem|Interaction")
	FName GetInteractionType() const { return InteractionType; }

	UFUNCTION(BlueprintPure, Category = "Supermarket Mayhem|Interaction")
	AActor* GetInteractionTarget() const { return InteractionTarget; }

	/** Checks whether a player or NPC may use this actor; outcome is still applied on authority only. */
	UFUNCTION(BlueprintCallable, Category = "Supermarket Mayhem|Interaction")
	bool CanInteractFrom(AActor* InteractingActor) const;

	/** Authority-only interaction entry point used by validated player requests and server AI. */
	bool TryInteract(AActor* InteractingActor);

	/** Restores this actor's small per-round interaction state. */
	void ResetForNewRound();

	/** Enables physics simulation on authority as a future movable/chaos hook. */
	UFUNCTION(BlueprintCallable, Category = "Supermarket Mayhem|Interaction|Physics")
	void SetPhysicsSimulationEnabled(bool bEnabled);

	UStaticMeshComponent* GetVisualMesh() const { return VisualMesh; }

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Supermarket Mayhem|Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> VisualMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Supermarket Mayhem|Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> InteractionCollision;

	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Interaction", meta = (ClampMin = "1.0"))
	FVector InteractionBoundsExtent = FVector(50.0f);

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Supermarket Mayhem|Interaction", meta = (AllowPrivateAccess = "true"))
	bool bInteractionEnabled = true;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Supermarket Mayhem|Interaction")
	FName InteractionType = FName(TEXT("Use"));

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Supermarket Mayhem|Interaction")
	FText DisplayName = NSLOCTEXT("SupermarketMayhem", "InteractiveObject", "Object");

	/** Optional related actor, for example a shelf display or product anchor. */
	UPROPERTY(EditInstanceOnly, Replicated, BlueprintReadOnly, Category = "Supermarket Mayhem|Interaction")
	TObjectPtr<AActor> InteractionTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Supermarket Mayhem|Interaction", meta = (ClampMin = "25.0"))
	float InteractionRadius = 250.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Supermarket Mayhem|Interaction")
	bool bAllowCustomerInteraction = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Supermarket Mayhem|Interaction|Events")
	bool bEmitNoiseForPlayerInteraction = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Supermarket Mayhem|Interaction|Events", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float InteractionNoiseIntensity = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Supermarket Mayhem|Interaction|Events", meta = (ClampMin = "0.0"))
	float InteractionNoiseRange = 700.0f;

	UPROPERTY(ReplicatedUsing = OnRep_InteractionCount, BlueprintReadOnly, Category = "Supermarket Mayhem|Interaction", meta = (AllowPrivateAccess = "true"))
	int32 InteractionCount = 0;

	/** Called only after a validated server interaction. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Supermarket Mayhem|Interaction")
	void OnInteractionPerformed(AActor* InteractingActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Supermarket Mayhem|Interaction")
	void OnInteractionCountChanged(int32 NewInteractionCount);

	virtual void HandleInteraction(AActor* InteractingActor);

private:
	UFUNCTION()
	void OnRep_InteractionCount();
	bool IsCustomerActor(const AActor* Actor) const;
	bool IsRoundInteractionOpen() const;
};
