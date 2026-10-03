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

void ASupermarketMayhemWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASupermarketMayhemWeapon, bIsEquipped);
}

void ASupermarketMayhemWeapon::RequestFire()
{
	if (GetOwner())
	{
		ServerRequestFire();
	}
}

void ASupermarketMayhemWeapon::ServerRequestFire_Implementation()
{
	ResolveFireRequest();
}

void ASupermarketMayhemWeapon::ResolveFireRequest()
{
	ASupermarketMayhemCharacter* OwningCharacter = Cast<ASupermarketMayhemCharacter>(GetOwner());
	ASupermarketMayhemPlayerState* AttackerPlayerState = OwningCharacter
		? OwningCharacter->GetPlayerState<ASupermarketMayhemPlayerState>()
		: nullptr;
	UWorld* World = GetWorld();
	const ASupermarketMayhemGameState* MayhemGameState = World
		? World->GetGameState<ASupermarketMayhemGameState>()
		: nullptr;

	if (!HasAuthority() || !bIsEquipped || !OwningCharacter || !AttackerPlayerState ||
		AttackerPlayerState->GetCurrentRole() != ESupermarketMayhemPlayerRole::Hunter ||
		AttackerPlayerState->IsEliminated() || !MayhemGameState ||
		MayhemGameState->GetCurrentRoundState() != ESupermarketMayhemRoundState::Hunt)
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
