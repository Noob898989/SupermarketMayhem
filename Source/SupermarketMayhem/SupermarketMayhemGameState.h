// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SupermarketMayhemTypes.h"
#include "SupermarketMayhemGameState.generated.h"

/**
 *  Game state for Supermarket Mayhem.
 */
UCLASS()
class SUPERMARKETMAYHEM_API ASupermarketMayhemGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ASupermarketMayhemGameState();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Supermarket Mayhem|Round")
	ESupermarketMayhemRoundState GetCurrentRoundState() const { return CurrentRoundState; }

	UFUNCTION(BlueprintCallable, Category = "Supermarket Mayhem|Round")
	void SetCurrentRoundState(ESupermarketMayhemRoundState NewRoundState) { CurrentRoundState = NewRoundState; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Supermarket Mayhem|Round")
	float GetRoundTimeRemaining() const { return RoundTimeRemaining; }

	UFUNCTION(BlueprintCallable, Category = "Supermarket Mayhem|Round")
	void SetRoundTimeRemaining(float NewRoundTimeRemaining) { RoundTimeRemaining = NewRoundTimeRemaining; }

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Supermarket Mayhem|Round", meta = (AllowPrivateAccess = "true"))
	ESupermarketMayhemRoundState CurrentRoundState = ESupermarketMayhemRoundState::WaitingToStart;

	UPROPERTY(BlueprintReadOnly, Category = "Supermarket Mayhem|Round", meta = (AllowPrivateAccess = "true"))
	float RoundTimeRemaining = 0.0f;
};
