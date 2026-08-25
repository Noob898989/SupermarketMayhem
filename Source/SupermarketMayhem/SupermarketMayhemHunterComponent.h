// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SupermarketMayhemHunterComponent.generated.h"

class ASupermarketMayhemCharacter;

/**
 *  Lets a Hunter eliminate a nearby Hider via a short first-person line trace.
 *
 *  Mirrors the trace/RPC shape already established by
 *  USupermarketMayhemDisguiseComponent (camera line trace, Server RPC to a
 *  validated, authoritative implementation), but targets
 *  ASupermarketMayhemCharacter instead of ASupermarketMayhemProp. Applies no
 *  elimination state itself - all authoritative elimination logic lives in
 *  ASupermarketMayhemGameMode::EliminateHider, which this component only calls.
 */
UCLASS(ClassGroup = (SupermarketMayhem), meta = (BlueprintSpawnableComponent))
class SUPERMARKETMAYHEM_API USupermarketMayhemHunterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USupermarketMayhemHunterComponent();

	/** Called on Eliminate input. Traces for a Hider and, if found, requests server-authoritative elimination. */
	UFUNCTION(BlueprintCallable, Category = "Supermarket Mayhem|Hunter")
	void TryEliminate();

protected:
	/** Trace range for the elimination attempt, in cm. */
	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Hunter")
	float EliminateTraceDistance = 500.0f;

	/** Server RPC: requests the server to run TryEliminate's validated, authoritative logic. */
	UFUNCTION(Server, Reliable)
	void ServerTryEliminate();

	/** Line-traces from the owner's first person camera to find a targetable Character. Returns nullptr if none found. */
	ASupermarketMayhemCharacter* FindTargetedCharacter() const;
};
