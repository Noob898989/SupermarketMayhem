// Copyright Epic Games, Inc. All Rights Reserved.

#include "SupermarketMayhemCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "SupermarketMayhem.h"
#include "SupermarketMayhemDisguiseComponent.h"
#include "SupermarketMayhemHunterComponent.h"
#include "SupermarketMayhemPlayerState.h"
#include "SupermarketMayhemWeapon.h"

ASupermarketMayhemCharacter::ASupermarketMayhemCharacter()
{
	DefaultWeaponClass = ASupermarketMayhemWeapon::StaticClass();

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;

	DisguiseComponent = CreateDefaultSubobject<USupermarketMayhemDisguiseComponent>(TEXT("DisguiseComponent"));

	HunterComponent = CreateDefaultSubobject<USupermarketMayhemHunterComponent>(TEXT("HunterComponent"));
}

void ASupermarketMayhemCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASupermarketMayhemCharacter, EquippedWeapon);
}

void ASupermarketMayhemCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (HasAuthority())
	{
		const ASupermarketMayhemPlayerState* MayhemPlayerState = GetPlayerState<ASupermarketMayhemPlayerState>();
		SetHunterWeaponEquipped(MayhemPlayerState && MayhemPlayerState->GetCurrentRole() == ESupermarketMayhemPlayerRole::Hunter);
	}
}

void ASupermarketMayhemCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (ASupermarketMayhemPlayerState* MayhemPlayerState = GetPlayerState<ASupermarketMayhemPlayerState>())
	{
		if (DisguiseComponent)
		{
			DisguiseComponent->ApplyReplicatedDisguiseState(MayhemPlayerState->IsDisguised(), MayhemPlayerState->GetCurrentPropId());
		}
	}
}

void ASupermarketMayhemCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (!RuntimeReloadAction)
		{
			RuntimeReloadAction = NewObject<UInputAction>(this, TEXT("RuntimeReloadAction"));
			RuntimeReloadMappingContext = NewObject<UInputMappingContext>(this, TEXT("RuntimeReloadMappingContext"));
			RuntimeReloadMappingContext->MapKey(RuntimeReloadAction, EKeys::R);
		}

		if (APlayerController* PlayerController = Cast<APlayerController>(GetController());
			PlayerController && PlayerController->IsLocalController())
		{
			if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
			{
				if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
				{
					InputSubsystem->AddMappingContext(RuntimeReloadMappingContext, 50);
					ReloadInputLocalPlayer = LocalPlayer;
				}
			}
		}

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ASupermarketMayhemCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ASupermarketMayhemCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASupermarketMayhemCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASupermarketMayhemCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ASupermarketMayhemCharacter::LookInput);

		// Interacting
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ASupermarketMayhemCharacter::DoInteract);

		// Eliminating
		EnhancedInputComponent->BindAction(EliminateAction, ETriggerEvent::Started, this, &ASupermarketMayhemCharacter::DoEliminate);
		EnhancedInputComponent->BindAction(RuntimeReloadAction, ETriggerEvent::Started, this, &ASupermarketMayhemCharacter::DoReload);
	}
	else
	{
		UE_LOG(LogSupermarketMayhem, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void ASupermarketMayhemCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void ASupermarketMayhemCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void ASupermarketMayhemCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ASupermarketMayhemCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void ASupermarketMayhemCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void ASupermarketMayhemCharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}

void ASupermarketMayhemCharacter::DoInteract()
{
	if (DisguiseComponent)
	{
		DisguiseComponent->TryInteract();
	}
}

void ASupermarketMayhemCharacter::DoEliminate()
{
	if (HunterComponent)
	{
		HunterComponent->TryFire();
	}
}

void ASupermarketMayhemCharacter::DoReload()
{
	if (HunterComponent)
	{
		HunterComponent->TryReload();
	}
}

void ASupermarketMayhemCharacter::SetHunterWeaponEquipped(bool bShouldBeEquipped)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!bShouldBeEquipped)
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->Unequip();
		}
		return;
	}

	if (!EquippedWeapon && DefaultWeaponClass)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		SpawnParameters.Instigator = this;
		EquippedWeapon = GetWorld()->SpawnActor<ASupermarketMayhemWeapon>(DefaultWeaponClass, GetActorTransform(), SpawnParameters);
	}

	if (EquippedWeapon)
	{
		EquippedWeapon->EquipTo(this);
		ForceNetUpdate();
	}
}

void ASupermarketMayhemCharacter::ResetWeaponForNewRound()
{
	if (HasAuthority() && EquippedWeapon)
	{
		EquippedWeapon->ResetForNewRound();
	}
}

void ASupermarketMayhemCharacter::CancelWeaponReload()
{
	if (HasAuthority() && EquippedWeapon)
	{
		EquippedWeapon->CancelReload();
	}
}

void ASupermarketMayhemCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (RuntimeReloadMappingContext)
	{
		if (ULocalPlayer* LocalPlayer = ReloadInputLocalPlayer.Get())
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				InputSubsystem->RemoveMappingContext(RuntimeReloadMappingContext);
			}
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ASupermarketMayhemCharacter::ApplyEliminatedState()
{
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		CachedMaxWalkSpeedBeforeElimination = MovementComponent->MaxWalkSpeed;
		MovementComponent->MaxWalkSpeed = 0.0f;
	}
}

void ASupermarketMayhemCharacter::ClearEliminatedState()
{
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->MaxWalkSpeed = CachedMaxWalkSpeedBeforeElimination;
	}
}
