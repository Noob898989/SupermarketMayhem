// Copyright Epic Games, Inc. All Rights Reserved.

#include "SupermarketMayhemHunterComponent.h"

#include "SupermarketMayhemCharacter.h"
#include "SupermarketMayhemWeapon.h"

USupermarketMayhemHunterComponent::USupermarketMayhemHunterComponent()
{
}

void USupermarketMayhemHunterComponent::TryFire()
{
	if (const ASupermarketMayhemCharacter* OwnerCharacter = Cast<ASupermarketMayhemCharacter>(GetOwner()))
	{
		if (ASupermarketMayhemWeapon* Weapon = OwnerCharacter->GetEquippedWeapon())
		{
			Weapon->RequestFire();
		}
	}
}

void USupermarketMayhemHunterComponent::TryReload()
{
	if (const ASupermarketMayhemCharacter* OwnerCharacter = Cast<ASupermarketMayhemCharacter>(GetOwner()))
	{
		if (ASupermarketMayhemWeapon* Weapon = OwnerCharacter->GetEquippedWeapon())
		{
			Weapon->RequestReload();
		}
	}
}
