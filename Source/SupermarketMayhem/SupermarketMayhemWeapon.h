// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SupermarketMayhemWeapon.generated.h"

class ASupermarketMayhemCharacter;

/** Server-authoritative, reusable base for player weapons. */
UCLASS(Blueprintable)
class SUPERMARKETMAYHEM_API ASupermarketMayhemWeapon : public AActor
{
	GENERATED_BODY()

public:
	ASupermarketMayhemWeapon();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:

	/** Requests a shot. Clients only send intent; the server validates and resolves the hit. */
	UFUNCTION(BlueprintCallable, Category = "Supermarket Mayhem|Weapon")
	void RequestFire();

	/** Requests a reload. The server decides whether it may begin and when it completes. */
	UFUNCTION(BlueprintCallable, Category = "Supermarket Mayhem|Weapon")
	void RequestReload();

	/** Equips this weapon to a character. Authority only. */
	void EquipTo(ASupermarketMayhemCharacter* Character);

	/** Unequips this weapon. Authority only. */
	void Unequip();

	/** Cancels any pending reload and fills the magazine for a new round. Authority only. */
	void ResetForNewRound();

	/** Cancels a pending reload without changing ammunition. Authority only. */
	void CancelReload();

	UFUNCTION(BlueprintPure, Category = "Supermarket Mayhem|Weapon")
	bool IsEquipped() const { return bIsEquipped; }

	UFUNCTION(BlueprintPure, Category = "Supermarket Mayhem|Weapon")
	int32 GetCurrentAmmo() const { return CurrentAmmo; }

	UFUNCTION(BlueprintPure, Category = "Supermarket Mayhem|Weapon")
	int32 GetMagazineCapacity() const { return MagazineCapacity; }

	UFUNCTION(BlueprintPure, Category = "Supermarket Mayhem|Weapon")
	bool IsReloading() const { return bIsReloading; }

protected:
	/** Maximum server trace distance for this weapon, in cm. */
	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Weapon", meta = (ClampMin = "0.0"))
	float FireTraceDistance = 500.0f;

	/** Maximum rounds in this weapon's magazine. */
	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Weapon|Ammo", meta = (ClampMin = "1"))
	int32 MagazineCapacity = 6;

	/** Duration, in seconds, of a server-authoritative reload. */
	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Weapon|Ammo", meta = (ClampMin = "0.05"))
	float ReloadDuration = 1.5f;

	/** Current authoritative magazine count. Replicated to relevant clients. */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Supermarket Mayhem|Weapon|Ammo", meta = (AllowPrivateAccess = "true"))
	int32 CurrentAmmo = 0;

	/** Whether the server is currently reloading. Replicated to relevant clients. */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Supermarket Mayhem|Weapon|Ammo", meta = (AllowPrivateAccess = "true"))
	bool bIsReloading = false;

	/** True while this weapon is equipped; replicated for relevant clients. */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Supermarket Mayhem|Weapon", meta = (AllowPrivateAccess = "true"))
	bool bIsEquipped = false;

	UFUNCTION(Server, Reliable)
	void ServerRequestFire();

	UFUNCTION(Server, Reliable)
	void ServerRequestReload();

	/** Performs server-side role, round, equipment and ECC_Camera hit validation. */
	void ResolveFireRequest();

	/** Resolves a target using the owning Hunter's server-side camera view. */
	ASupermarketMayhemCharacter* FindTargetedCharacter() const;

	/** Validates server authority, equipped Hunter ownership, elimination state and Hunt phase. */
	bool ValidateHunterAction() const;

	/** Begins the authoritative reload timer. */
	void StartReload();

	/** Completes the reload on the server after ReloadDuration. */
	void CompleteReload();

	FTimerHandle ReloadTimerHandle;
};
