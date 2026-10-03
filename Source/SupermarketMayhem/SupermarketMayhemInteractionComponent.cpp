// Copyright Epic Games, Inc. All Rights Reserved.

#include "SupermarketMayhemInteractionComponent.h"

#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "SupermarketMayhemCharacter.h"
#include "SupermarketMayhemGameState.h"
#include "SupermarketMayhemInteractiveActor.h"
#include "SupermarketMayhemPlayerState.h"
#include "SupermarketMayhemTypes.h"

USupermarketMayhemInteractionComponent::USupermarketMayhemInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
}

ASupermarketMayhemInteractiveActor* USupermarketMayhemInteractionComponent::TraceInteractiveActor() const
{
	const ASupermarketMayhemCharacter* Character = Cast<ASupermarketMayhemCharacter>(GetOwner());
	const UCameraComponent* Camera = Character ? Character->GetFirstPersonCameraComponent() : nullptr;
	UWorld* World = GetWorld();
	if (!Camera || !World)
	{
		return nullptr;
	}

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SupermarketInteractionTrace), true, Character);
	FHitResult Hit;
	const FVector Start = Camera->GetComponentLocation();
	const FVector End = Start + Camera->GetForwardVector() * InteractionTraceDistance;
	if (!World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams))
	{
		return nullptr;
	}

	return Cast<ASupermarketMayhemInteractiveActor>(Hit.GetActor());
}

bool USupermarketMayhemInteractionComponent::IsPlayerAllowedToInteract() const
{
	const ASupermarketMayhemCharacter* Character = Cast<ASupermarketMayhemCharacter>(GetOwner());
	const ASupermarketMayhemPlayerState* PlayerState = Character
		? Character->GetPlayerState<ASupermarketMayhemPlayerState>()
		: nullptr;
	const ASupermarketMayhemGameState* GameState = GetWorld()
		? GetWorld()->GetGameState<ASupermarketMayhemGameState>()
		: nullptr;

	if (!Character || !PlayerState || PlayerState->IsEliminated() || !GameState ||
		(PlayerState->GetCurrentRole() != ESupermarketMayhemPlayerRole::Hider &&
		 PlayerState->GetCurrentRole() != ESupermarketMayhemPlayerRole::Hunter))
	{
		return false;
	}

	const ESupermarketMayhemRoundState RoundState = GameState->GetCurrentRoundState();
	return RoundState == ESupermarketMayhemRoundState::Preparation || RoundState == ESupermarketMayhemRoundState::Hunt;
}

ASupermarketMayhemInteractiveActor* USupermarketMayhemInteractionComponent::GetFocusedInteractiveActor() const
{
	if (!IsPlayerAllowedToInteract())
	{
		return nullptr;
	}
	ASupermarketMayhemInteractiveActor* Target = TraceInteractiveActor();
	return IsValid(Target) && Target->CanInteractFrom(GetOwner()) ? Target : nullptr;
}

FText USupermarketMayhemInteractionComponent::GetFocusedInteractionPrompt() const
{
	const ASupermarketMayhemInteractiveActor* Target = GetFocusedInteractiveActor();
	return Target ? Target->GetInteractionPrompt() : FText::GetEmpty();
}

bool USupermarketMayhemInteractionComponent::TryInteract()
{
	UWorld* World = GetWorld();
	if (!World || !IsPlayerAllowedToInteract() || World->GetTimeSeconds() < NextLocalRequestTime)
	{
		return false;
	}

	ASupermarketMayhemInteractiveActor* Target = TraceInteractiveActor();
	if (!IsValid(Target) || !Target->CanInteractFrom(GetOwner()))
	{
		return false;
	}

	NextLocalRequestTime = World->GetTimeSeconds() + 0.1f;
	ServerTryInteract(Target);
	return true;
}

void USupermarketMayhemInteractionComponent::ServerTryInteract_Implementation(ASupermarketMayhemInteractiveActor* ClientTargetHint)
{
	UWorld* World = GetWorld();
	if (!GetOwner() || !GetOwner()->HasAuthority() || !World || World->GetTimeSeconds() < NextServerInteractionTime)
	{
		return;
	}
	NextServerInteractionTime = World->GetTimeSeconds() + 0.25f;

	ASupermarketMayhemInteractiveActor* ServerTarget = TraceInteractiveActor();
	ASupermarketMayhemCharacter* Character = Cast<ASupermarketMayhemCharacter>(GetOwner());
	if (!IsPlayerAllowedToInteract() || !IsValid(ClientTargetHint) || ServerTarget != ClientTargetHint ||
		!IsValid(ServerTarget) || !ServerTarget->CanInteractFrom(Character))
	{
		return;
	}

	const float MaximumRange = InteractionTraceDistance + 100.0f;
	if (FVector::DistSquared(ServerTarget->GetActorLocation(), Character->GetActorLocation()) > FMath::Square(MaximumRange))
	{
		return;
	}
	ServerTarget->TryInteract(Character);
}
