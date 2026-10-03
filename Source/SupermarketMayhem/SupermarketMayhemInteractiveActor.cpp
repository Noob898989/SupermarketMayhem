// Copyright Epic Games, Inc. All Rights Reserved.

#include "SupermarketMayhemInteractiveActor.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "SupermarketMayhemCustomer.h"
#include "SupermarketMayhem.h"
#include "SupermarketMayhemGameMode.h"
#include "SupermarketMayhemGameState.h"
#include "SupermarketMayhemTypes.h"

ASupermarketMayhemInteractiveActor::ASupermarketMayhemInteractiveActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(false);
	SetNetUpdateFrequency(5.0f);

	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	SetRootComponent(VisualMesh);
	VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VisualMesh->SetCanEverAffectNavigation(false);

	InteractionCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionCollision"));
	InteractionCollision->SetupAttachment(VisualMesh);
	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionCollision->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	InteractionCollision->SetCanEverAffectNavigation(false);
	InteractionCollision->SetGenerateOverlapEvents(false);
	InteractionCollision->SetBoxExtent(InteractionBoundsExtent);
}

void ASupermarketMayhemInteractiveActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (InteractionCollision)
	{
		InteractionCollision->SetBoxExtent(InteractionBoundsExtent);
	}
}

void ASupermarketMayhemInteractiveActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASupermarketMayhemInteractiveActor, bInteractionEnabled);
	DOREPLIFETIME(ASupermarketMayhemInteractiveActor, InteractionType);
	DOREPLIFETIME(ASupermarketMayhemInteractiveActor, DisplayName);
	DOREPLIFETIME(ASupermarketMayhemInteractiveActor, InteractionTarget);
	DOREPLIFETIME(ASupermarketMayhemInteractiveActor, InteractionCount);
}

FText ASupermarketMayhemInteractiveActor::GetInteractionPrompt() const
{
	return FText::Format(NSLOCTEXT("SupermarketMayhem", "InteractPrompt", "Interact with {0}"), DisplayName);
}

bool ASupermarketMayhemInteractiveActor::IsCustomerActor(const AActor* Actor) const
{
	return Cast<ASupermarketMayhemCustomer>(Actor) != nullptr;
}

bool ASupermarketMayhemInteractiveActor::CanInteractFrom(AActor* InteractingActor) const
{
	return bInteractionEnabled && IsRoundInteractionOpen() && IsValid(InteractingActor) &&
		(!IsCustomerActor(InteractingActor) || bAllowCustomerInteraction);
}

bool ASupermarketMayhemInteractiveActor::IsRoundInteractionOpen() const
{
	const ASupermarketMayhemGameState* GameState = GetWorld()
		? GetWorld()->GetGameState<ASupermarketMayhemGameState>()
		: nullptr;
	if (!GameState)
	{
		return false;
	}

	const ESupermarketMayhemRoundState RoundState = GameState->GetCurrentRoundState();
	return RoundState == ESupermarketMayhemRoundState::Preparation ||
		RoundState == ESupermarketMayhemRoundState::Hunt;
}

bool ASupermarketMayhemInteractiveActor::TryInteract(AActor* InteractingActor)
{
	if (!HasAuthority() || !CanInteractFrom(InteractingActor) ||
		FVector::DistSquared(GetActorLocation(), InteractingActor->GetActorLocation()) > FMath::Square(InteractionRadius))
	{
		return false;
	}

	HandleInteraction(InteractingActor);
	return true;
}

void ASupermarketMayhemInteractiveActor::HandleInteraction(AActor* InteractingActor)
{
	if (!HasAuthority())
	{
		return;
	}

	++InteractionCount;
	if (InteractionCount == 1)
	{
		UE_LOG(LogSupermarketMayhem, Log, TEXT("Interactive actor %s was first used by %s."),
			*GetNameSafe(this), *GetNameSafe(InteractingActor));
	}
	OnRep_InteractionCount();
	OnInteractionPerformed(InteractingActor);
	ForceNetUpdate();

	if (bEmitNoiseForPlayerInteraction && !IsCustomerActor(InteractingActor))
	{
		if (ASupermarketMayhemGameMode* GameMode = GetWorld()->GetAuthGameMode<ASupermarketMayhemGameMode>())
		{
			GameMode->ReportCustomerNoise(GetActorLocation(), InteractionNoiseIntensity, InteractionNoiseRange);
		}
	}
}

void ASupermarketMayhemInteractiveActor::ResetForNewRound()
{
	if (HasAuthority() && InteractionCount != 0)
	{
		InteractionCount = 0;
		OnRep_InteractionCount();
		ForceNetUpdate();
	}
}

void ASupermarketMayhemInteractiveActor::SetPhysicsSimulationEnabled(bool bEnabled)
{
	if (!HasAuthority() || !VisualMesh || !VisualMesh->GetStaticMesh())
	{
		return;
	}
	VisualMesh->SetMobility(EComponentMobility::Movable);
	InteractionCollision->SetCollisionEnabled(bEnabled ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryOnly);
	VisualMesh->SetCollisionEnabled(bEnabled ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	VisualMesh->SetSimulatePhysics(bEnabled);
	SetReplicateMovement(bEnabled);
}

void ASupermarketMayhemInteractiveActor::OnRep_InteractionCount()
{
	OnInteractionCountChanged(InteractionCount);
}
