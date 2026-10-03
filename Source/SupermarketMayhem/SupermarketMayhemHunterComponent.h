// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SupermarketMayhemHunterComponent.generated.h"

/**
 * Routes the existing Hunter input to the currently equipped weapon. It does
 * not resolve hits or decide gameplay outcomes.
 */
UCLASS(ClassGroup = (SupermarketMayhem), meta = (BlueprintSpawnableComponent))
class SUPERMARKETMAYHEM_API USupermarketMayhemHunterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USupermarketMayhemHunterComponent();

	/** Called by the existing eliminate input. Requests a shot from the equipped weapon. */
	UFUNCTION(BlueprintCallable, Category = "Supermarket Mayhem|Hunter")
	void TryFire();

	/** Requests reload from the equipped weapon. The weapon validates the request on the server. */
	UFUNCTION(BlueprintCallable, Category = "Supermarket Mayhem|Hunter")
	void TryReload();
};
