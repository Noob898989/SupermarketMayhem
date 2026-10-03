// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SupermarketMayhemWeapon.generated.h"

class ASupermarketMayhemCharacter;
class UStaticMesh;
class UStaticMeshComponent;
class UAnimMontage;
class UAnimSequence;
class UAnimSequenceBase;
class USkeletalMeshComponent;
class UPointLightComponent;
class USoundBase;

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
	UPROPERTY(ReplicatedUsing = OnRep_ReloadingState, BlueprintReadOnly, Category = "Supermarket Mayhem|Weapon|Ammo", meta = (AllowPrivateAccess = "true"))
	bool bIsReloading = false;

	/** True while this weapon is equipped; replicated for relevant clients. */
	UPROPERTY(ReplicatedUsing = OnRep_EquippedState, BlueprintReadOnly, Category = "Supermarket Mayhem|Weapon", meta = (AllowPrivateAccess = "true"))
	bool bIsEquipped = false;

	/** Shared placeholder mesh used for local first-person and remote third-person presentation. */
	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Weapon|Presentation")
	TObjectPtr<UStaticMesh> WeaponMeshAsset;

	/** Camera-relative first-person placement, tuned later through weapon defaults. */
	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Weapon|Presentation")
	FTransform FirstPersonMeshTransform = FTransform(FRotator::ZeroRotator, FVector(28.0f, 12.0f, -14.0f));

	/** Hand socket-relative third-person placement. */
	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Weapon|Presentation")
	FTransform ThirdPersonMeshTransform = FTransform(FRotator(0.0f, 90.0f, 0.0f), FVector::ZeroVector);

	/** Owner-only weapon view, attached locally to the first-person camera. */
	UPROPERTY(VisibleAnywhere, Category = "Supermarket Mayhem|Weapon|Presentation")
	TObjectPtr<UStaticMeshComponent> FirstPersonWeaponMesh;

	/** World representation, attached locally to the character's hand socket. */
	UPROPERTY(VisibleAnywhere, Category = "Supermarket Mayhem|Weapon|Presentation")
	TObjectPtr<UStaticMeshComponent> ThirdPersonWeaponMesh;

	/** Existing mannequin pistol animation assets; all playback is cosmetic. */
	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Weapon|Animation")
	TObjectPtr<UAnimMontage> FireMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Weapon|Animation")
	TObjectPtr<UAnimSequence> ReloadAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Weapon|Animation")
	TObjectPtr<UAnimSequence> EquipAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Weapon|Animation")
	TObjectPtr<UAnimSequence> DryFireAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Weapon|Animation")
	FName CosmeticAnimationSlot = FName(TEXT("DefaultSlot"));

	/** Optional sound hooks; only the existing template fire sound is assigned by default. */
	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Weapon|Audio")
	TObjectPtr<USoundBase> FireSound;

	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Weapon|Audio")
	TObjectPtr<USoundBase> ReloadSound;

	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Weapon|Audio")
	TObjectPtr<USoundBase> EquipSound;

	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Weapon|Audio")
	TObjectPtr<USoundBase> DryFireSound;

	/** Short point-light flash placeholders attached at the muzzle on each view representation. */
	UPROPERTY(VisibleAnywhere, Category = "Supermarket Mayhem|Weapon|Presentation")
	TObjectPtr<UPointLightComponent> FirstPersonMuzzleFlash;

	UPROPERTY(VisibleAnywhere, Category = "Supermarket Mayhem|Weapon|Presentation")
	TObjectPtr<UPointLightComponent> ThirdPersonMuzzleFlash;

	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Weapon|Presentation")
	FVector MuzzleOffset = FVector(35.0f, 0.0f, 0.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Weapon|Presentation", meta = (ClampMin = "0.0", ClampMax = "3.0"))
	float RecoilKickDegrees = 0.8f;

	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Weapon|Presentation", meta = (ClampMin = "0.1"))
	float RecoilReturnSpeed = 12.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Weapon|Presentation", meta = (ClampMin = "0.01", ClampMax = "0.2"))
	float MuzzleFlashDuration = 0.05f;

	UFUNCTION(Server, Reliable)
	void ServerRequestFire();

	UFUNCTION(Server, Reliable)
	void ServerRequestReload();

	UFUNCTION()
	void OnRep_EquippedState();

	UFUNCTION()
	void OnRep_ReloadingState();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayFireFeedback();

	UFUNCTION(Client, Unreliable)
	void ClientPlayDryFireFeedback();

	/** Rebuilds local-only mesh attachments from the replicated equip state. */
	void UpdateWeaponPresentation();
	void UpdateReloadPresentation(bool bNowReloading);
	void PlayCosmeticMontage(UAnimMontage* Montage, float PlayRate = 1.0f);
	void PlayCosmeticSequence(UAnimSequenceBase* Animation, float PlayRate = 1.0f);
	UAnimMontage* PlaySequenceOnMesh(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float PlayRate);
	void PlayDryFireFeedback();
	void StartRecoilRecovery();
	void UpdateRecoilRecovery();
	void HideMuzzleFlash();

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
	FTimerHandle MuzzleFlashTimerHandle;
	FTimerHandle RecoilReturnTimerHandle;
	float CurrentRecoilPitch = 0.0f;
	float NextDryFireFeedbackTime = 0.0f;
	TWeakObjectPtr<ASupermarketMayhemCharacter> PresentationCharacter;
	TWeakObjectPtr<UAnimMontage> FirstPersonReloadMontage;
	TWeakObjectPtr<UAnimMontage> ThirdPersonReloadMontage;
};
