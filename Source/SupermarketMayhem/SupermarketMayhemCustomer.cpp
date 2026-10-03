// Copyright Epic Games, Inc. All Rights Reserved.

#include "SupermarketMayhemCustomer.h"

#include "AIController.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "SupermarketMayhemCustomerAIController.h"
#include "SupermarketMayhemCustomerData.h"

ASupermarketMayhemCustomer::ASupermarketMayhemCustomer()
{
	bReplicates = true;
	SetReplicateMovement(true);
	SetNetUpdateFrequency(10.0f);
	SetMinNetUpdateFrequency(2.0f);
	SetNetCullDistanceSquared(FMath::Square(6000.0f));

	AIControllerClass = ASupermarketMayhemCustomerAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CustomerMesh(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple"));
	if (CustomerMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(CustomerMesh.Object);
	}
	static ConstructorHelpers::FClassFinder<UAnimInstance> CustomerAnimation(TEXT("/Game/Characters/Mannequins/Anims/Unarmed/ABP_Unarmed"));
	if (CustomerAnimation.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(CustomerAnimation.Class);
	}
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
}

namespace
{
	constexpr float DefaultCustomerMovementSpeed = 180.0f;
}

void ASupermarketMayhemCustomer::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority() && GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = FMath::Max(0.0f,
			CustomerData ? CustomerData->MovementSpeed : DefaultCustomerMovementSpeed);
	}
}

void ASupermarketMayhemCustomer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASupermarketMayhemCustomer, CustomerState);
	DOREPLIFETIME(ASupermarketMayhemCustomer, CustomerType);
}

void ASupermarketMayhemCustomer::SetCustomerData(USupermarketMayhemCustomerData* NewCustomerData)
{
	if (!HasAuthority())
	{
		return;
	}

	CustomerData = NewCustomerData;
	CustomerType = CustomerData ? CustomerData->CustomerType : FName(TEXT("DefaultCustomer"));
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = FMath::Max(0.0f,
			CustomerData ? CustomerData->MovementSpeed : DefaultCustomerMovementSpeed);
	}
}

void ASupermarketMayhemCustomer::SetDestinationActorTag(FName NewDestinationActorTag)
{
	if (HasAuthority())
	{
		DestinationActorTag = NewDestinationActorTag;
	}
}

void ASupermarketMayhemCustomer::SetCustomerState(ESupermarketMayhemCustomerState NewState)
{
	if (!HasAuthority() || CustomerState == NewState)
	{
		return;
	}

	const ESupermarketMayhemCustomerState PreviousState = CustomerState;
	CustomerState = NewState;
	OnRep_CustomerState(PreviousState);
}

void ASupermarketMayhemCustomer::OnRep_CustomerState(ESupermarketMayhemCustomerState PreviousState)
{
	OnCustomerStateChanged(PreviousState, CustomerState);
}

void ASupermarketMayhemCustomer::SetCustomerBehaviorEnabled(bool bEnabled, bool bResetState)
{
	if (!HasAuthority())
	{
		return;
	}

	if (ASupermarketMayhemCustomerAIController* CustomerController = Cast<ASupermarketMayhemCustomerAIController>(GetController()))
	{
		CustomerController->SetBehaviorEnabled(bEnabled, bResetState);
	}
}

void ASupermarketMayhemCustomer::ReportNoiseToCustomer(FVector NoiseLocation, float Intensity, float HearingRange)
{
	if (!HasAuthority() || !FMath::IsFinite(Intensity) || !FMath::IsFinite(HearingRange) ||
		CustomerState == ESupermarketMayhemCustomerState::Paused || HearingRange <= 0.0f)
	{
		return;
	}

	const float Sensitivity = FMath::Clamp(CustomerData ? CustomerData->ReactionSensitivity : 0.5f, 0.0f, 1.0f);
	const float Distance = FVector::Distance(GetActorLocation(), NoiseLocation);
	const float EffectiveRange = HearingRange * (0.5f + Sensitivity);
	if (Distance > EffectiveRange)
	{
		return;
	}

	const float Strength = FMath::Clamp(Intensity, 0.0f, 1.0f) * (1.0f - Distance / EffectiveRange);
	const float MinimumStrength = 0.15f + (1.0f - Sensitivity) * 0.45f;
	if (Strength < MinimumStrength)
	{
		return;
	}

	OnCustomerNoiseReported(NoiseLocation, Strength);
	if (ASupermarketMayhemCustomerAIController* CustomerController = Cast<ASupermarketMayhemCustomerAIController>(GetController()))
	{
		CustomerController->ReactToNoise(Strength);
	}
}
