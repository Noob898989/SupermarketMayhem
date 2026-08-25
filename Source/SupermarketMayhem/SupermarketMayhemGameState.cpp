// Copyright Epic Games, Inc. All Rights Reserved.

#include "SupermarketMayhemGameState.h"
#include "Net/UnrealNetwork.h"

ASupermarketMayhemGameState::ASupermarketMayhemGameState()
{
	// stub
}

void ASupermarketMayhemGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASupermarketMayhemGameState, CurrentRoundState);
	DOREPLIFETIME(ASupermarketMayhemGameState, RoundTimeRemaining);
}
