// Copyright Epic Games, Inc. All Rights Reserved.

#include "SupermarketMayhemWeapon.h"

#include "Camera/CameraComponent.h"
#include "CollisionQueryParams.h"
#include "Components/SceneComponent.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "SupermarketMayhemCharacter.h"
#include "SupermarketMayhemGameMode.h"
#include "SupermarketMayhemGameState.h"
#include "SupermarketMayhemPlayerState.h"
#include "SupermarketMayhemTypes.h"

ASupermarketMayhemWeapon::ASupermarketMayhemWeapon()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));
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
	if (!ValidateHunterAction() || bIsReloading || CurrentAmmo <= 0)
	{
		return;
	}

	--CurrentAmmo;
	ForceNetUpdate();
	ResolveFireRequest();
}

void ASupermarketMayhemWeapon::ServerRequestReload_Implementation()
{
	if (!ValidateHunterAction() || bIsReloading || CurrentAmmo >= MagazineCapacity)
	{
		return;
	}

	StartReload();
}

void ASupermarketMayhemWeapon::ResolveFireRequest()
{
	UWorld* World = GetWorld();
	const ASupermarketMayhemGameState* MayhemGameState = World
		? World->GetGameState<ASupermarketMayhemGameState>()
		: nullptr;

	if (!MayhemGameState || MayhemGameState->GetCurrentRoundState() != ESupermarketMayhemRoundState::Hunt)
	{
		return;
	}

	ASupermarketMayhemCharacter* TargetCharacter = FindTargetedCharacter();
	ASupermarketMayhemPlayerState* TargetPlayerState = TargetCharacter
		? TargetCharacter->GetPlayerState<ASupermarketMayhemPlayerState>()
		: nullptr;

	if (!TargetPlayerState || TargetPlayerState->GetCurrentRole() != ESupermarketMayhemPlayerRole::Hider ||
		TargetPlayerState->IsEliminated())
	{
		return;
	}

	if (ASupermarketMayhemGameMode* MayhemGameMode = World->GetAuthGameMode<ASupermarketMayhemGameMode>())
	{
		MayhemGameMode->EliminateHider(TargetPlayerState);
	}
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

	return HasAuthority() && bIsEquipped && PlayerState &&
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
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetOwner(nullptr);
	SetInstigator(nullptr);
	ForceNetUpdate();
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
