// Copyright Epic Games, Inc. All Rights Reserved.

#include "SupermarketMayhemDisguiseComponent.h"

#include "Camera/CameraComponent.h"
#include "CollisionQueryParams.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "SupermarketMayhemCharacter.h"
#include "SupermarketMayhemGameState.h"
#include "SupermarketMayhemPlayerState.h"
#include "SupermarketMayhemProp.h"
#include "SupermarketMayhemTypes.h"

USupermarketMayhemDisguiseComponent::USupermarketMayhemDisguiseComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USupermarketMayhemDisguiseComponent::TryInteract()
{
    const UWorld* World = GetWorld();
    const ASupermarketMayhemGameState* MayhemGameState =
        World ? World->GetGameState<ASupermarketMayhemGameState>() : nullptr;

    if (!MayhemGameState ||
        MayhemGameState->GetCurrentRoundState() != ESupermarketMayhemRoundState::Preparation)
    {
        return;
    }

    if (CurrentProp)
    {
        RemoveDisguise();
        return;
    }

    if (ASupermarketMayhemProp* TargetProp = FindInteractableProp())
    {
        TryDisguise(TargetProp);
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

    UStaticMeshComponent* PropMeshComponent = TargetProp->GetPropMeshComponent();

    if (!PropMeshComponent)
    {
        return false;
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

    MayhemPlayerState->SetDisguiseState(true, TargetProp->GetPropId());

    return true;
}

void USupermarketMayhemDisguiseComponent::RemoveDisguise()
{
    if (!CurrentProp)
    {
        return;
    }

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

    CurrentProp->SetWornByHider(false);

    if (OwnerCharacter)
    {
        ASupermarketMayhemPlayerState* MayhemPlayerState =
            OwnerCharacter->GetPlayerState<ASupermarketMayhemPlayerState>();

        if (MayhemPlayerState)
        {
            MayhemPlayerState->SetDisguiseState(false, NAME_None);
        }
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
        return Cast<ASupermarketMayhemProp>(HitResult.GetActor());
    }

    return nullptr;
}
