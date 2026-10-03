// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "SupermarketMayhemCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class USupermarketMayhemDisguiseComponent;
class USupermarketMayhemHunterComponent;
class ASupermarketMayhemWeapon;
class ULocalPlayer;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class ASupermarketMayhemCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Handles disguising as / reverting from a nearby Supermarket Mayhem prop */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USupermarketMayhemDisguiseComponent* DisguiseComponent;

	/** Handles a Hunter's attempt to eliminate a nearby Hider */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USupermarketMayhemHunterComponent* HunterComponent;

protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* MouseLookAction;

	/** Interact Input Action (disguise as / revert from a nearby prop) */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* InteractAction;

	/** Eliminate Input Action (Hunter attempts to eliminate a nearby Hider) */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* EliminateAction;

public:
	ASupermarketMayhemCharacter();

protected:

	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	/** Handles interact inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoInteract();

	/** Handles eliminate inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoEliminate();
	virtual void DoReload();

protected:

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Reacts to the replicated PlayerState reference becoming valid/changing. Re-applies the current disguise state to cover replication ordering races with PlayerState::OnRep_DisguiseState. */
	virtual void OnRep_PlayerState() override;
	

public:

	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** Returns the disguise component **/
	USupermarketMayhemDisguiseComponent* GetDisguiseComponent() const { return DisguiseComponent; }

	/** Returns the replicated equipped weapon, if one is available. */
	ASupermarketMayhemWeapon* GetEquippedWeapon() const { return EquippedWeapon; }

	/** Makes the default weapon available to a Hunter. Server-authoritative. */
	void SetHunterWeaponEquipped(bool bShouldBeEquipped);
	/** Refills the Hunter weapon and clears reload state at the start of each round. Server-authoritative. */
	void ResetWeaponForNewRound();
	/** Cancels any active server-side weapon reload, for example when Result begins. */
	void CancelWeaponReload();

	/** Locks this character's movement in reaction to its PlayerState becoming eliminated. Called from PlayerState::OnRep_Eliminated (remote clients) and GameMode::EliminateHider (server/host). */
	UFUNCTION(BlueprintCallable, Category = "Supermarket Mayhem|Elimination")
	void ApplyEliminatedState();

	/** Restores the movement speed cached by ApplyEliminatedState(). Called by GameMode when a new round starts. */
	UFUNCTION(BlueprintCallable, Category = "Supermarket Mayhem|Elimination")
	void ClearEliminatedState();

protected:
	/** Weapon class used when this character is assigned the Hunter role. */
	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Weapon")
	TSubclassOf<ASupermarketMayhemWeapon> DefaultWeaponClass;

	/** Runtime Enhanced Input action/context for the default Reload key (R). */
	UPROPERTY(Transient)
	TObjectPtr<UInputAction> RuntimeReloadAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> RuntimeReloadMappingContext;

	TWeakObjectPtr<ULocalPlayer> ReloadInputLocalPlayer;

	/** Server-spawned weapon actor available to relevant clients. */
	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = "Supermarket Mayhem|Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ASupermarketMayhemWeapon> EquippedWeapon;

	/** MaxWalkSpeed cached by ApplyEliminatedState() before locking movement; restored by ClearEliminatedState(). */
	float CachedMaxWalkSpeedBeforeElimination = 0.0f;

};

