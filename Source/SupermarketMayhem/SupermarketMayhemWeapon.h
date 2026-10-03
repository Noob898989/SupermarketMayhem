// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SupermarketMayhemWeapon.generated.h"

class ASupermarketMayhemCharacter;
class ASupermarketMayhemPlayerState;

/** Server-authoritative, reusable base for player weapons. */
UCLASS(Blueprintable)
class SUPERMARKETMAYHEM_API ASupermarketMayhemWeapon : public AActor
{
	GENERATED_BODY()

public:
	ASupermarketMayhemWeapon();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Requests a shot. Clients only send intent; the server validates and resolves the hit. */
	UFUNCTION(BlueprintCallable, Category = "Supermarket Mayhem|Weapon")
	void RequestFire();

	/** Equips this weapon to a character. Authority only. */
	void EquipTo(ASupermarketMayhemCharacter* Character);

	/** Unequips this weapon. Authority only. */
	void Unequip();

	UFUNCTION(BlueprintPure, Category = "Supermarket Mayhem|Weapon")
	bool IsEquipped() const { return bIsEquipped; }

protected:
	/** Maximum server trace distance for this weapon, in cm. */
	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Weapon", meta = (ClampMin = "0.0"))
	float FireTraceDistance = 500.0f;

	/** True while this weapon is equipped; replicated for relevant clients. */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Supermarket Mayhem|Weapon", meta = (AllowPrivateAccess = "true"))
	bool bIsEquipped = false;

	UFUNCTION(Server, Reliable)
	void ServerRequestFire();

	/** Performs server-side role, round, equipment and ECC_Camera hit validation. */
	void ResolveFireRequest();

	/** Resolves a target using the owning Hunter's server-side camera view. */
	ASupermarketMayhemCharacter* FindTargetedCharacter() const;
};
