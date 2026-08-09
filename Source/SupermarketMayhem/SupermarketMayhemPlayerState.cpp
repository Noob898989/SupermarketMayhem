// Copyright Epic Games, Inc. All Rights Reserved.

#include "SupermarketMayhemPlayerState.h"

ASupermarketMayhemPlayerState::ASupermarketMayhemPlayerState()
{
	// stub
}

void ASupermarketMayhemPlayerState::SetDisguiseState(bool bNewIsDisguised, FName NewPropId)
{
	bIsDisguised = bNewIsDisguised;
	CurrentPropId = NewPropId;
}
