// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SupermarketMayhemInteractionComponent.generated.h"

class ASupermarketMayhemInteractiveActor;

/** Owner-local target query and server-validated request path for player interaction. */
UCLASS(ClassGroup = (SupermarketMayhem), meta = (BlueprintSpawnableComponent))
class SUPERMARKETMAYHEM_API USupermarketMayhemInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USupermarketMayhemInteractionComponent();

	/** Sends a request for the current local target; the server independently retraces and validates it. */
	UFUNCTION(BlueprintCallable, Category = "Supermarket Mayhem|Interaction")
	bool TryInteract();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Supermarket Mayhem|Interaction")
	ASupermarketMayhemInteractiveActor* GetFocusedInteractiveActor() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Supermarket Mayhem|Interaction")
	FText GetFocusedInteractionPrompt() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Interaction", meta = (ClampMin = "25.0"))
	float InteractionTraceDistance = 250.0f;

	UFUNCTION(Server, Reliable)
	void ServerTryInteract(ASupermarketMayhemInteractiveActor* ClientTargetHint);

private:
	ASupermarketMayhemInteractiveActor* TraceInteractiveActor() const;
	bool IsPlayerAllowedToInteract() const;
	float NextServerInteractionTime = 0.0f;
	float NextLocalRequestTime = 0.0f;
};
