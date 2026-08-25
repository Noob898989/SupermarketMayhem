// Copyright Epic Games, Inc. All Rights Reserved.

#include "SupermarketMayhemDisguiseComponent.h"

#include "Camera/CameraComponent.h"
#include "CollisionQueryParams.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "SupermarketMayhemCharacter.h"
#include "SupermarketMayhemGameState.h"
#include "SupermarketMayhemPlayerState.h"
#include "SupermarketMayhemProp.h"
#include "SupermarketMayhemTypes.h"

USupermarketMayhemDisguiseComponent::USupermarketMayhemDisguiseComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicated(true);
}

void USupermarketMayhemDisguiseComponent::TryInteract()
{
    ServerTryInteract();
}

void USupermarketMayhemDisguiseComponent::ApplyReplicatedDisguiseState(bool bNewIsDisguised, FName NewPropId)
{
    if (bNewIsDisguised)
    {
        if (ASupermarketMayhemProp* TargetProp = FindPropById(NewPropId))
        {
            ApplyDisguiseVisuals(TargetProp);
        }
    }
    else
    {
        RemoveDisguiseVisuals(CurrentProp);
    }
}

void USupermarketMayhemDisguiseComponent::ServerTryInteract_Implementation()
{
    const UWorld* World = GetWorld();
    const ASupermarketMayhemGameState* MayhemGameState =
        World ? World->GetGameState<ASupermarketMayhemGameState>() : nullptr;

    if (!MayhemGameState ||
        MayhemGameState->GetCurrentRoundState() != ESupermarketMayhemRoundState::Preparation)
    {
        return;
    }

    ASupermarketMayhemCharacter* OwnerCharacter =
        Cast<ASupermarketMayhemCharacter>(GetOwner());

    ASupermarketMayhemPlayerState* MayhemPlayerState =
        OwnerCharacter ? OwnerCharacter->GetPlayerState<ASupermarketMayhemPlayerState>() : nullptr;

    if (MayhemPlayerState && MayhemPlayerState->IsDisguised())
    {
        ASupermarketMayhemProp* DisguisedProp =
            FindPropById(MayhemPlayerState->GetCurrentPropId());

        RemoveDisguise();
        RemoveDisguiseVisuals(DisguisedProp);
        return;
    }

    ASupermarketMayhemProp* TargetProp = FindInteractableProp();

    if (TargetProp)
    {
        if (TryDisguise(TargetProp))
        {
            ApplyDisguiseVisuals(TargetProp);
        }
    }
}

bool USupermarketMayhemDisguiseComponent::TryDisguise(ASupermarketMayhemProp* TargetProp)
{
    if (!TargetProp)
    {
        return false;
    }

    ASupermarketMayhemCharacter* OwnerCharacter =
        Cast<ASupermarketMayhemCharacter>(GetOwner());

    if (!OwnerCharacter)
    {
        return false;
    }

    ASupermarketMayhemPlayerState* MayhemPlayerState =
        OwnerCharacter->GetPlayerState<ASupermarketMayhemPlayerState>();

    if (!MayhemPlayerState ||
        MayhemPlayerState->GetCurrentRole() != ESupermarketMayhemPlayerRole::Hider)
    {
        return false;
    }

    const UWorld* World = GetWorld();
    const ASupermarketMayhemGameState* MayhemGameState =
        World ? World->GetGameState<ASupermarketMayhemGameState>() : nullptr;

    if (!MayhemGameState ||
        MayhemGameState->GetCurrentRoundState() != ESupermarketMayhemRoundState::Preparation)
    {
        return false;
    }

    if (!TargetProp->GetPropMeshComponent())
    {
        return false;
    }

    MayhemPlayerState->SetDisguiseState(true, TargetProp->GetPropId());

    return true;
}

void USupermarketMayhemDisguiseComponent::ForceRemoveDisguise()
{
    ASupermarketMayhemCharacter* OwnerCharacter =
        Cast<ASupermarketMayhemCharacter>(GetOwner());

    ASupermarketMayhemPlayerState* MayhemPlayerState =
        OwnerCharacter ? OwnerCharacter->GetPlayerState<ASupermarketMayhemPlayerState>() : nullptr;

    if (!MayhemPlayerState || !MayhemPlayerState->IsDisguised())
    {
        return;
    }

    ASupermarketMayhemProp* DisguisedProp =
        FindPropById(MayhemPlayerState->GetCurrentPropId());

    RemoveDisguise();
    RemoveDisguiseVisuals(DisguisedProp);
}

void USupermarketMayhemDisguiseComponent::RemoveDisguise()
{
    ASupermarketMayhemCharacter* OwnerCharacter =
        Cast<ASupermarketMayhemCharacter>(GetOwner());

    ASupermarketMayhemPlayerState* MayhemPlayerState =
        OwnerCharacter ? OwnerCharacter->GetPlayerState<ASupermarketMayhemPlayerState>() : nullptr;

    if (!MayhemPlayerState || !MayhemPlayerState->IsDisguised())
    {
        return;
    }

    MayhemPlayerState->SetDisguiseState(false, NAME_None);
}

void USupermarketMayhemDisguiseComponent::ApplyDisguiseVisuals(ASupermarketMayhemProp* TargetProp)
{
    if (!TargetProp)
    {
        return;
    }

    ASupermarketMayhemCharacter* OwnerCharacter =
        Cast<ASupermarketMayhemCharacter>(GetOwner());

    UStaticMeshComponent* PropMeshComponent = TargetProp->GetPropMeshComponent();

    if (!OwnerCharacter || !PropMeshComponent)
    {
        return;
    }

    USkeletalMeshComponent* FirstPersonMesh = OwnerCharacter->GetFirstPersonMesh();

    if (FirstPersonMesh)
    {
        FirstPersonMesh->SetVisibility(false, true);
    }

    USkeletalMeshComponent* ThirdPersonMesh = OwnerCharacter->GetMesh();

    if (ThirdPersonMesh)
    {
        ThirdPersonMesh->SetVisibility(false, true);
    }

    if (!DisguiseMeshComponent)
    {
        DisguiseMeshComponent =
            NewObject<UStaticMeshComponent>(OwnerCharacter, TEXT("DisguiseMesh"));
        DisguiseMeshComponent->SetupAttachment(OwnerCharacter->GetRootComponent());
        DisguiseMeshComponent->RegisterComponent();
        DisguiseMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    DisguiseMeshComponent->SetStaticMesh(PropMeshComponent->GetStaticMesh());
    DisguiseMeshComponent->SetRelativeTransform(FTransform::Identity);
    DisguiseMeshComponent->SetVisibility(true, true);

    TargetProp->SetWornByHider(true);

    CurrentProp = TargetProp;
}

void USupermarketMayhemDisguiseComponent::RemoveDisguiseVisuals(ASupermarketMayhemProp* TargetProp)
{
    ASupermarketMayhemCharacter* OwnerCharacter =
        Cast<ASupermarketMayhemCharacter>(GetOwner());

    if (DisguiseMeshComponent)
    {
        DisguiseMeshComponent->SetVisibility(false, true);
    }

    if (OwnerCharacter)
    {
        USkeletalMeshComponent* FirstPersonMesh = OwnerCharacter->GetFirstPersonMesh();

        if (FirstPersonMesh)
        {
            FirstPersonMesh->SetVisibility(true, true);
        }

        USkeletalMeshComponent* ThirdPersonMesh = OwnerCharacter->GetMesh();

        if (ThirdPersonMesh)
        {
            ThirdPersonMesh->SetVisibility(true, true);
        }
    }

    if (TargetProp)
    {
        TargetProp->SetWornByHider(false);
    }

    CurrentProp = nullptr;
}

ASupermarketMayhemProp* USupermarketMayhemDisguiseComponent::FindInteractableProp() const
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
        TraceStart + Camera->GetForwardVector() * InteractTraceDistance;

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(OwnerCharacter);

    FHitResult HitResult;

    if (World->LineTraceSingleByChannel(
            HitResult,
            TraceStart,
            TraceEnd,
            ECC_Visibility,
            QueryParams))
    {
        ASupermarketMayhemProp* HitProp = Cast<ASupermarketMayhemProp>(HitResult.GetActor());
        return HitProp;
    }

    return nullptr;
}

ASupermarketMayhemProp* USupermarketMayhemDisguiseComponent::FindPropById(FName PropId) const
{
    if (PropId.IsNone())
    {
        return nullptr;
    }

    UWorld* World = GetWorld();

    if (!World)
    {
        return nullptr;
    }

    for (TActorIterator<ASupermarketMayhemProp> It(World); It; ++It)
    {
        if (It->GetPropId() == PropId)
        {
            return *It;
        }
    }

    return nullptr;
}
