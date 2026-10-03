// Copyright Epic Games, Inc. All Rights Reserved.

#include "SupermarketMayhemWeapon.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "Camera/CameraComponent.h"
#include "CollisionQueryParams.h"
#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/HitResult.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Net/UnrealNetwork.h"
#include "SupermarketMayhemCharacter.h"
#include "SupermarketMayhemGameMode.h"
#include "SupermarketMayhemGameState.h"
#include "SupermarketMayhemPlayerState.h"
#include "SupermarketMayhemTypes.h"

ASupermarketMayhemWeapon::ASupermarketMayhemWeapon()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PistolMesh(TEXT("/Game/Weapons/Pistol/Meshes/SM_Pistol.SM_Pistol"));
	if (PistolMesh.Succeeded())
	{
		WeaponMeshAsset = PistolMesh.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> PistolFireMontage(TEXT("/Game/Characters/Mannequins/Anims/Pistol/MM_Pistol_Fire_Montage.MM_Pistol_Fire_Montage"));
	if (PistolFireMontage.Succeeded()) FireMontage = PistolFireMontage.Object;
	static ConstructorHelpers::FObjectFinder<UAnimSequence> PistolReloadAnimation(TEXT("/Game/Characters/Mannequins/Anims/Pistol/MM_Pistol_Reload.MM_Pistol_Reload"));
	if (PistolReloadAnimation.Succeeded()) ReloadAnimation = PistolReloadAnimation.Object;
	static ConstructorHelpers::FObjectFinder<UAnimSequence> PistolEquipAnimation(TEXT("/Game/Characters/Mannequins/Anims/Pistol/MM_Pistol_Equip.MM_Pistol_Equip"));
	if (PistolEquipAnimation.Succeeded()) EquipAnimation = PistolEquipAnimation.Object;
	static ConstructorHelpers::FObjectFinder<UAnimSequence> PistolDryFireAnimation(TEXT("/Game/Characters/Mannequins/Anims/Pistol/MM_Pistol_DryFire.MM_Pistol_DryFire"));
	if (PistolDryFireAnimation.Succeeded()) DryFireAnimation = PistolDryFireAnimation.Object;
	static ConstructorHelpers::FObjectFinder<USoundBase> TemplateFireSound(TEXT("/Game/Weapons/GrenadeLauncher/Audio/FirstPersonTemplateWeaponFire02.FirstPersonTemplateWeaponFire02"));
	if (TemplateFireSound.Succeeded()) FireSound = TemplateFireSound.Object;

	FirstPersonWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FirstPersonWeaponMesh"));
	FirstPersonWeaponMesh->SetupAttachment(RootComponent);
	FirstPersonWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FirstPersonWeaponMesh->SetGenerateOverlapEvents(false);
	FirstPersonWeaponMesh->SetOnlyOwnerSee(true);
	FirstPersonWeaponMesh->SetCastShadow(false);
	FirstPersonWeaponMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;

	ThirdPersonWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ThirdPersonWeaponMesh"));
	ThirdPersonWeaponMesh->SetupAttachment(RootComponent);
	ThirdPersonWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ThirdPersonWeaponMesh->SetGenerateOverlapEvents(false);
	ThirdPersonWeaponMesh->SetOwnerNoSee(true);
	ThirdPersonWeaponMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;
	ThirdPersonWeaponMesh->SetCastShadow(true);

	FirstPersonWeaponMesh->SetStaticMesh(WeaponMeshAsset);
	ThirdPersonWeaponMesh->SetStaticMesh(WeaponMeshAsset);
	FirstPersonWeaponMesh->SetVisibility(false);
	ThirdPersonWeaponMesh->SetVisibility(false);
	FirstPersonMuzzleFlash = CreateDefaultSubobject<UPointLightComponent>(TEXT("FirstPersonMuzzleFlash"));
	FirstPersonMuzzleFlash->SetupAttachment(FirstPersonWeaponMesh);
	FirstPersonMuzzleFlash->SetRelativeLocation(MuzzleOffset);
	FirstPersonMuzzleFlash->SetIntensity(6000.0f);
	FirstPersonMuzzleFlash->SetAttenuationRadius(180.0f);
	FirstPersonMuzzleFlash->SetLightColor(FLinearColor(1.0f, 0.58f, 0.22f));
	FirstPersonMuzzleFlash->SetCastShadows(false);
	FirstPersonMuzzleFlash->SetVisibility(false);

	ThirdPersonMuzzleFlash = CreateDefaultSubobject<UPointLightComponent>(TEXT("ThirdPersonMuzzleFlash"));
	ThirdPersonMuzzleFlash->SetupAttachment(ThirdPersonWeaponMesh);
	ThirdPersonMuzzleFlash->SetRelativeLocation(MuzzleOffset);
	ThirdPersonMuzzleFlash->SetIntensity(6000.0f);
	ThirdPersonMuzzleFlash->SetAttenuationRadius(180.0f);
	ThirdPersonMuzzleFlash->SetLightColor(FLinearColor(1.0f, 0.58f, 0.22f));
	ThirdPersonMuzzleFlash->SetCastShadows(false);
	ThirdPersonMuzzleFlash->SetVisibility(false);

	bReplicates = true;
	SetReplicateMovement(false);
}

void ASupermarketMayhemWeapon::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		MagazineCapacity = FMath::Max(MagazineCapacity, 1);
		CurrentAmmo = MagazineCapacity;
	}
	UpdateWeaponPresentation();
}

void ASupermarketMayhemWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASupermarketMayhemWeapon, bIsEquipped);
	DOREPLIFETIME(ASupermarketMayhemWeapon, CurrentAmmo);
	DOREPLIFETIME(ASupermarketMayhemWeapon, bIsReloading);
}

void ASupermarketMayhemWeapon::RequestFire()
{
	if (GetOwner())
	{
		ServerRequestFire();
	}
}

void ASupermarketMayhemWeapon::RequestReload()
{
	if (GetOwner())
	{
		ServerRequestReload();
	}
}

void ASupermarketMayhemWeapon::ServerRequestFire_Implementation()
{
	if (!ValidateHunterAction() || bIsReloading)
	{
		return;
	}
	if (CurrentAmmo <= 0)
	{
		if (UWorld* World = GetWorld(); World && World->GetTimeSeconds() >= NextDryFireFeedbackTime)
		{
			NextDryFireFeedbackTime = World->GetTimeSeconds() + 0.15f;
			ClientPlayDryFireFeedback();
		}
		return;
	}

	--CurrentAmmo;
	ForceNetUpdate();
	MulticastPlayFireFeedback();
	if (ResolveFireRequest())
	{
		if (UWorld* World = GetWorld())
		{
			if (ASupermarketMayhemGameMode* MayhemGameMode = World->GetAuthGameMode<ASupermarketMayhemGameMode>())
			{
				MayhemGameMode->ReportCustomerNoise(GetActorLocation(), 1.0f, 2200.0f);
			}
		}
	}
}

void ASupermarketMayhemWeapon::ServerRequestReload_Implementation()
{
	if (!ValidateHunterAction() || bIsReloading || CurrentAmmo >= MagazineCapacity)
	{
		return;
	}

	StartReload();
}

bool ASupermarketMayhemWeapon::ResolveFireRequest()
{
	UWorld* World = GetWorld();
	const ASupermarketMayhemGameState* MayhemGameState = World
		? World->GetGameState<ASupermarketMayhemGameState>()
		: nullptr;

	if (!MayhemGameState || MayhemGameState->GetCurrentRoundState() != ESupermarketMayhemRoundState::Hunt)
	{
		return false;
	}

	// A valid server trace is a real shot even when it misses. Its noise is
	// emitted only after the authoritative view and round checks have run.
	const ASupermarketMayhemCharacter* OwningCharacter = Cast<ASupermarketMayhemCharacter>(GetOwner());
	if (!OwningCharacter || !OwningCharacter->GetFirstPersonCameraComponent())
	{
		return false;
	}

	ASupermarketMayhemCharacter* TargetCharacter = FindTargetedCharacter();
	ASupermarketMayhemPlayerState* TargetPlayerState = TargetCharacter
		? TargetCharacter->GetPlayerState<ASupermarketMayhemPlayerState>()
		: nullptr;

	if (!TargetPlayerState || TargetPlayerState->GetCurrentRole() != ESupermarketMayhemPlayerRole::Hider ||
		TargetPlayerState->IsEliminated())
	{
		return true;
	}

	if (ASupermarketMayhemGameMode* MayhemGameMode = World->GetAuthGameMode<ASupermarketMayhemGameMode>())
	{
		MayhemGameMode->EliminateHider(TargetPlayerState);
	}
	return true;
}

bool ASupermarketMayhemWeapon::ValidateHunterAction() const
{
	const ASupermarketMayhemCharacter* OwningCharacter = Cast<ASupermarketMayhemCharacter>(GetOwner());
	const ASupermarketMayhemPlayerState* PlayerState = OwningCharacter
		? OwningCharacter->GetPlayerState<ASupermarketMayhemPlayerState>()
		: nullptr;
	const UWorld* World = GetWorld();
	const ASupermarketMayhemGameState* MayhemGameState = World
		? World->GetGameState<ASupermarketMayhemGameState>()
		: nullptr;

	return HasAuthority() && bIsEquipped && PlayerState && OwningCharacter->GetEquippedWeapon() == this &&
		PlayerState->GetCurrentRole() == ESupermarketMayhemPlayerRole::Hunter &&
		!PlayerState->IsEliminated() && MayhemGameState &&
		MayhemGameState->GetCurrentRoundState() == ESupermarketMayhemRoundState::Hunt;
}

void ASupermarketMayhemWeapon::StartReload()
{
	UWorld* World = GetWorld();
	if (!HasAuthority() || !World)
	{
		return;
	}

	bIsReloading = true;
	UpdateReloadPresentation(true);
	ForceNetUpdate();
	World->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ASupermarketMayhemWeapon::CompleteReload, FMath::Max(ReloadDuration, 0.05f), false);
}

void ASupermarketMayhemWeapon::CompleteReload()
{
	if (!HasAuthority())
	{
		return;
	}

	if (ValidateHunterAction() && bIsReloading)
	{
		CurrentAmmo = MagazineCapacity;
	}

	bIsReloading = false;
	UpdateReloadPresentation(false);
	ForceNetUpdate();
}

void ASupermarketMayhemWeapon::CancelReload()
{
	if (!HasAuthority())
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ReloadTimerHandle);
	}
	bIsReloading = false;
	UpdateReloadPresentation(false);
	ForceNetUpdate();
}

void ASupermarketMayhemWeapon::ResetForNewRound()
{
	if (!HasAuthority())
	{
		return;
	}

	CancelReload();
	CurrentAmmo = MagazineCapacity;
	ForceNetUpdate();
}

void ASupermarketMayhemWeapon::EquipTo(ASupermarketMayhemCharacter* Character)
{
	if (!HasAuthority() || !IsValid(Character))
	{
		return;
	}

	SetOwner(Character);
	SetInstigator(Character);
	SetActorTransform(Character->GetActorTransform());
	AttachToActor(Character, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	bIsEquipped = true;
	UpdateWeaponPresentation();
	PlayCosmeticSequence(EquipAnimation);
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
	}
	ForceNetUpdate();
}

void ASupermarketMayhemWeapon::Unequip()
{
	if (!HasAuthority())
	{
		return;
	}

	CancelReload();
	bIsEquipped = false;
	UpdateWeaponPresentation();
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetOwner(nullptr);
	SetInstigator(nullptr);
	ForceNetUpdate();
}

void ASupermarketMayhemWeapon::OnRep_EquippedState()
{
	UpdateWeaponPresentation();
	if (bIsEquipped)
	{
		PlayCosmeticSequence(EquipAnimation);
		if (EquipSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
		}
	}
}

void ASupermarketMayhemWeapon::OnRep_ReloadingState()
{
	UpdateReloadPresentation(bIsReloading);
}

void ASupermarketMayhemWeapon::ClientPlayDryFireFeedback_Implementation()
{
	PlayDryFireFeedback();
}

void ASupermarketMayhemWeapon::UpdateWeaponPresentation()
{
	ASupermarketMayhemCharacter* OwningCharacter = Cast<ASupermarketMayhemCharacter>(GetOwner());
	if (OwningCharacter)
	{
		PresentationCharacter = OwningCharacter;
	}
	if (!OwningCharacter || !bIsEquipped)
	{
		UpdateReloadPresentation(false);
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(MuzzleFlashTimerHandle);
			World->GetTimerManager().ClearTimer(RecoilReturnTimerHandle);
		}
		CurrentRecoilPitch = 0.0f;
		HideMuzzleFlash();
		FirstPersonWeaponMesh->SetVisibility(false);
		ThirdPersonWeaponMesh->SetVisibility(false);
		FirstPersonWeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		ThirdPersonWeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		PresentationCharacter.Reset();
		return;
	}

	// These component attachments are local presentation only. The replicated actor
	// remains attached to the character by the existing authoritative equip path.
	if (UCameraComponent* Camera = OwningCharacter->GetFirstPersonCameraComponent())
	{
		FirstPersonWeaponMesh->AttachToComponent(Camera, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		FirstPersonWeaponMesh->SetRelativeTransform(FirstPersonMeshTransform);
		FirstPersonWeaponMesh->SetStaticMesh(WeaponMeshAsset);
		FirstPersonWeaponMesh->SetVisibility(true);
		FirstPersonMuzzleFlash->SetRelativeLocation(MuzzleOffset);
	}

	if (USkeletalMeshComponent* CharacterMesh = OwningCharacter->GetMesh())
	{
		ThirdPersonWeaponMesh->AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("hand_r")));
		ThirdPersonWeaponMesh->SetRelativeTransform(ThirdPersonMeshTransform);
		ThirdPersonWeaponMesh->SetStaticMesh(WeaponMeshAsset);
		ThirdPersonWeaponMesh->SetVisibility(true);
		ThirdPersonMuzzleFlash->SetRelativeLocation(MuzzleOffset);
	}
}

void ASupermarketMayhemWeapon::MulticastPlayFireFeedback_Implementation()
{
	const ASupermarketMayhemCharacter* OwningCharacter = Cast<ASupermarketMayhemCharacter>(GetOwner());
	const bool bIsLocalOwner = OwningCharacter && OwningCharacter->IsLocallyControlled();
	if (FirstPersonMuzzleFlash) FirstPersonMuzzleFlash->SetVisibility(bIsLocalOwner);
	if (ThirdPersonMuzzleFlash) ThirdPersonMuzzleFlash->SetVisibility(!bIsLocalOwner);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(MuzzleFlashTimerHandle);
		World->GetTimerManager().SetTimer(MuzzleFlashTimerHandle, this, &ASupermarketMayhemWeapon::HideMuzzleFlash,
			FMath::Max(MuzzleFlashDuration, 0.01f), false);
	}

	PlayCosmeticMontage(FireMontage);
	if (FireSound)
	{
		const USceneComponent* FlashOrigin = bIsLocalOwner
			? Cast<USceneComponent>(FirstPersonMuzzleFlash)
			: Cast<USceneComponent>(ThirdPersonMuzzleFlash);
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, FlashOrigin ? FlashOrigin->GetComponentLocation() : GetActorLocation());
	}
	if (bIsLocalOwner)
	{
		StartRecoilRecovery();
	}
}

void ASupermarketMayhemWeapon::PlayCosmeticMontage(UAnimMontage* Montage, float PlayRate)
{
	if (!Montage)
	{
		return;
	}
	const ASupermarketMayhemCharacter* OwningCharacter = Cast<ASupermarketMayhemCharacter>(GetOwner());
	if (!OwningCharacter)
	{
		return;
	}
	if (USkeletalMeshComponent* FirstPersonMesh = OwningCharacter->GetFirstPersonMesh())
	{
		if (UAnimInstance* AnimInstance = FirstPersonMesh->GetAnimInstance())
		{
			AnimInstance->Montage_Play(Montage, PlayRate);
		}
	}
	if (USkeletalMeshComponent* CharacterMesh = OwningCharacter->GetMesh())
	{
		if (UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance())
		{
			AnimInstance->Montage_Play(Montage, PlayRate);
		}
	}
}

void ASupermarketMayhemWeapon::PlayCosmeticSequence(UAnimSequenceBase* Animation, float PlayRate)
{
	const ASupermarketMayhemCharacter* OwningCharacter = Cast<ASupermarketMayhemCharacter>(GetOwner());
	if (!OwningCharacter || !Animation)
	{
		return;
	}
	PlaySequenceOnMesh(OwningCharacter->GetFirstPersonMesh(), Animation, PlayRate);
	PlaySequenceOnMesh(OwningCharacter->GetMesh(), Animation, PlayRate);
}

UAnimMontage* ASupermarketMayhemWeapon::PlaySequenceOnMesh(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float PlayRate)
{
	if (!Mesh || !Animation)
	{
		return nullptr;
	}
	if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
	{
		return AnimInstance->PlaySlotAnimationAsDynamicMontage(Animation, CosmeticAnimationSlot,
			0.1f, 0.1f, FMath::Max(PlayRate, 0.01f), 1);
	}
	return nullptr;
}

void ASupermarketMayhemWeapon::UpdateReloadPresentation(bool bNowReloading)
{
	const ASupermarketMayhemCharacter* OwningCharacter = Cast<ASupermarketMayhemCharacter>(GetOwner());
	if (!OwningCharacter)
	{
		OwningCharacter = PresentationCharacter.Get();
	}
	if (!OwningCharacter)
	{
		return;
	}
	if (bNowReloading)
	{
		const float PlayRate = ReloadAnimation && ReloadDuration > 0.0f
			? ReloadAnimation->GetPlayLength() / ReloadDuration
			: 1.0f;
		const ASupermarketMayhemCharacter* Character = OwningCharacter;
		FirstPersonReloadMontage = PlaySequenceOnMesh(Character->GetFirstPersonMesh(), ReloadAnimation, PlayRate);
		ThirdPersonReloadMontage = PlaySequenceOnMesh(Character->GetMesh(), ReloadAnimation, PlayRate);
		if (ReloadSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, GetActorLocation());
		}
		return;
	}
	if (USkeletalMeshComponent* FirstPersonMesh = OwningCharacter->GetFirstPersonMesh())
	{
		if (UAnimInstance* AnimInstance = FirstPersonMesh->GetAnimInstance(); AnimInstance && FirstPersonReloadMontage.IsValid())
		{
			AnimInstance->Montage_Stop(0.15f, FirstPersonReloadMontage.Get());
		}
	}
	if (USkeletalMeshComponent* CharacterMesh = OwningCharacter->GetMesh())
	{
		if (UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance(); AnimInstance && ThirdPersonReloadMontage.IsValid())
		{
			AnimInstance->Montage_Stop(0.15f, ThirdPersonReloadMontage.Get());
		}
	}
	FirstPersonReloadMontage.Reset();
	ThirdPersonReloadMontage.Reset();
}

void ASupermarketMayhemWeapon::PlayDryFireFeedback()
{
	const ASupermarketMayhemCharacter* OwningCharacter = Cast<ASupermarketMayhemCharacter>(GetOwner());
	if (!OwningCharacter || !OwningCharacter->IsLocallyControlled() || !bIsEquipped || OwningCharacter->GetEquippedWeapon() != this)
	{
		return;
	}
	if (USkeletalMeshComponent* FirstPersonMesh = OwningCharacter->GetFirstPersonMesh())
	{
		PlaySequenceOnMesh(FirstPersonMesh, DryFireAnimation, 1.0f);
	}
	if (DryFireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DryFireSound, GetActorLocation());
	}
}

void ASupermarketMayhemWeapon::StartRecoilRecovery()
{
	if (!FirstPersonWeaponMesh)
	{
		return;
	}
	CurrentRecoilPitch = FMath::Min(CurrentRecoilPitch + FMath::Clamp(RecoilKickDegrees, 0.0f, 3.0f), 3.0f);
	FTransform RecoilTransform = FirstPersonMeshTransform;
	RecoilTransform.SetRotation(FirstPersonMeshTransform.GetRotation() * FQuat(FRotator(CurrentRecoilPitch, 0.0f, 0.0f)));
	FirstPersonWeaponMesh->SetRelativeTransform(RecoilTransform);
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(RecoilReturnTimerHandle, this, &ASupermarketMayhemWeapon::UpdateRecoilRecovery, 0.016f, true);
	}
}

void ASupermarketMayhemWeapon::UpdateRecoilRecovery()
{
	UWorld* World = GetWorld();
	if (!World || !FirstPersonWeaponMesh)
	{
		return;
	}
	CurrentRecoilPitch = FMath::FInterpTo(CurrentRecoilPitch, 0.0f,
		FMath::Clamp(World->GetDeltaSeconds(), 0.001f, 0.05f), FMath::Max(RecoilReturnSpeed, 0.1f));
	if (CurrentRecoilPitch <= 0.01f)
	{
		CurrentRecoilPitch = 0.0f;
		FirstPersonWeaponMesh->SetRelativeTransform(FirstPersonMeshTransform);
		World->GetTimerManager().ClearTimer(RecoilReturnTimerHandle);
		return;
	}
	FTransform RecoilTransform = FirstPersonMeshTransform;
	RecoilTransform.SetRotation(FirstPersonMeshTransform.GetRotation() * FQuat(FRotator(CurrentRecoilPitch, 0.0f, 0.0f)));
	FirstPersonWeaponMesh->SetRelativeTransform(RecoilTransform);
}

void ASupermarketMayhemWeapon::HideMuzzleFlash()
{
	if (FirstPersonMuzzleFlash) FirstPersonMuzzleFlash->SetVisibility(false);
	if (ThirdPersonMuzzleFlash) ThirdPersonMuzzleFlash->SetVisibility(false);
}

ASupermarketMayhemCharacter* ASupermarketMayhemWeapon::FindTargetedCharacter() const
{
	const ASupermarketMayhemCharacter* OwningCharacter = Cast<ASupermarketMayhemCharacter>(GetOwner());
	const UCameraComponent* Camera = OwningCharacter
		? OwningCharacter->GetFirstPersonCameraComponent()
		: nullptr;
	UWorld* World = GetWorld();
	if (!Camera || !World)
	{
		return nullptr;
	}

	const FVector TraceStart = Camera->GetComponentLocation();
	const FVector TraceEnd = TraceStart + Camera->GetForwardVector() * FireTraceDistance;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwningCharacter);

	FHitResult HitResult;
	if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Camera, QueryParams))
	{
		return Cast<ASupermarketMayhemCharacter>(HitResult.GetActor());
	}

	return nullptr;
}
