// Copyright Epic Games, Inc. All Rights Reserved.

#include "SupermarketMayhemHunterComponent.h"

#include "Camera/CameraComponent.h"
#include "CollisionQueryParams.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "SupermarketMayhemCharacter.h"
#include "SupermarketMayhemGameMode.h"
#include "SupermarketMayhemGameState.h"
#include "SupermarketMayhemPlayerState.h"
#include "SupermarketMayhemTypes.h"

USupermarketMayhemHunterComponent::USupermarketMayhemHunterComponent()
{
    SetIsReplicated(true);
}

void USupermarketMayhemHunterComponent::TryEliminate()
{
    ServerTryEliminate();
}

void USupermarketMayhemHunterComponent::ServerTryEliminate_Implementation()
{
    const UWorld* World = GetWorld();
    const ASupermarketMayhemGameState* MayhemGameState =
        World ? World->GetGameState<ASupermarketMayhemGameState>() : nullptr;

    if (!MayhemGameState ||
        MayhemGameState->GetCurrentRoundState() != ESupermarketMayhemRoundState::Hunt)
    {
        return;
    }

    ASupermarketMayhemCharacter* OwnerCharacter =
        Cast<ASupermarketMayhemCharacter>(GetOwner());

    ASupermarketMayhemPlayerState* AttackerPlayerState =
        OwnerCharacter ? OwnerCharacter->GetPlayerState<ASupermarketMayhemPlayerState>() : nullptr;

    if (!AttackerPlayerState ||
        AttackerPlayerState->GetCurrentRole() != ESupermarketMayhemPlayerRole::Hunter)
    {
        return;
    }

    ASupermarketMayhemCharacter* TargetCharacter = FindTargetedCharacter();

    if (!TargetCharacter)
    {
        return;
    }

    ASupermarketMayhemPlayerState* TargetPlayerState =
        TargetCharacter->GetPlayerState<ASupermarketMayhemPlayerState>();

    if (!TargetPlayerState ||
        TargetPlayerState->GetCurrentRole() != ESupermarketMayhemPlayerRole::Hider ||
        TargetPlayerState->IsEliminated())
    {
        return;
    }

    if (ASupermarketMayhemGameMode* MayhemGameMode =
            World ? World->GetAuthGameMode<ASupermarketMayhemGameMode>() : nullptr)
    {
        MayhemGameMode->EliminateHider(TargetPlayerState);
    }
}

ASupermarketMayhemCharacter* USupermarketMayhemHunterComponent::FindTargetedCharacter() const
{
    const ASupermarketMayhemCharacter* OwnerCharacter =
        Cast<ASupermarketMayhemCharacter>(GetOwner());

    if (!OwnerCharacter)
    {
        return nullptr;
    }

    const UCameraComponent* Camera =
        OwnerCharacter->GetFirstPersonCameraComponent();

    if (!Camera)
    {
        return nullptr;
    }

    UWorld* World = GetWorld();

    if (!World)
    {
        return nullptr;
    }

    const FVector TraceStart = Camera->GetComponentLocation();
    const FVector TraceEnd =
        TraceStart + Camera->GetForwardVector() * EliminateTraceDistance;

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(OwnerCharacter);

    FHitResult HitResult;

    if (World->LineTraceSingleByChannel(
            HitResult,
            TraceStart,
            TraceEnd,
            ECC_Camera,
            QueryParams))
    {
        return Cast<ASupermarketMayhemCharacter>(HitResult.GetActor());
    }

    return nullptr;
}
