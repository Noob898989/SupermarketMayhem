// Copyright Epic Games, Inc. All Rights Reserved.

#include "SupermarketMayhemPlayerState.h"
#include "Net/UnrealNetwork.h"

#include "EngineUtils.h"
#include "SupermarketMayhemCharacter.h"
#include "SupermarketMayhemDisguiseComponent.h"

ASupermarketMayhemPlayerState::ASupermarketMayhemPlayerState()
{
	// stub
}

void ASupermarketMayhemPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASupermarketMayhemPlayerState, CurrentRole);
	DOREPLIFETIME(ASupermarketMayhemPlayerState, bIsDisguised);
	DOREPLIFETIME(ASupermarketMayhemPlayerState, CurrentPropId);
	DOREPLIFETIME(ASupermarketMayhemPlayerState, bIsEliminated);
}

void ASupermarketMayhemPlayerState::SetDisguiseState(bool bNewIsDisguised, FName NewPropId)
{
	bIsDisguised = bNewIsDisguised;
	CurrentPropId = NewPropId;
}

void ASupermarketMayhemPlayerState::OnRep_DisguiseState()
{
	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	for (TActorIterator<ASupermarketMayhemCharacter> It(World); It; ++It)
	{
		ASupermarketMayhemCharacter* Character = *It;

		if (Character && Character->GetPlayerState<ASupermarketMayhemPlayerState>() == this)
		{
			if (USupermarketMayhemDisguiseComponent* DisguiseComponent = Character->GetDisguiseComponent())
			{
				DisguiseComponent->ApplyReplicatedDisguiseState(bIsDisguised, CurrentPropId);
			}

			return;
		}
	}
}

void ASupermarketMayhemPlayerState::OnRep_Eliminated()
{
	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	for (TActorIterator<ASupermarketMayhemCharacter> It(World); It; ++It)
	{
		ASupermarketMayhemCharacter* Character = *It;

		if (Character && Character->GetPlayerState<ASupermarketMayhemPlayerState>() == this)
		{
			Character->ApplyEliminatedState();

			return;
		}
	}
}
