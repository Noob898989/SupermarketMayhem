// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SupermarketMayhemCustomerData.h"
#include "SupermarketMayhemCustomer.generated.h"

UCLASS(Blueprintable)
class SUPERMARKETMAYHEM_API ASupermarketMayhemCustomer : public ACharacter
{
	GENERATED_BODY()

public:
	ASupermarketMayhemCustomer();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Server-only behavior switch used by the round-owned spawn manager. */
	void SetCustomerBehaviorEnabled(bool bEnabled, bool bResetState = false);

	/** Server-side extension hook for future sound/chaos reactions. This does not implement a reaction policy. */
	UFUNCTION(BlueprintCallable, Category = "Supermarket Mayhem|Customer|Reactions")
	void ReportNoiseToCustomer(FVector NoiseLocation, float Intensity);

	UFUNCTION(BlueprintImplementableEvent, Category = "Supermarket Mayhem|Customer|Reactions")
	void OnCustomerNoiseReported(FVector NoiseLocation, float Intensity);

	USupermarketMayhemCustomerData* GetCustomerData() const { return CustomerData; }
	FName GetDestinationActorTag() const { return DestinationActorTag; }
	ESupermarketMayhemCustomerState GetCustomerState() const { return CustomerState; }

	/** Server-side state setter. */
	void SetCustomerState(ESupermarketMayhemCustomerState NewState);

protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnRep_CustomerState(ESupermarketMayhemCustomerState PreviousState);
	UFUNCTION(BlueprintImplementableEvent, Category = "Supermarket Mayhem|Customer")
	void OnCustomerStateChanged(ESupermarketMayhemCustomerState PreviousState, ESupermarketMayhemCustomerState NewState);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Supermarket Mayhem|Customer")
	TObjectPtr<USupermarketMayhemCustomerData> CustomerData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Supermarket Mayhem|Customer|Navigation")
	FName DestinationActorTag = FName(TEXT("CustomerDestination"));

	UPROPERTY(ReplicatedUsing = OnRep_CustomerState, BlueprintReadOnly, Category = "Supermarket Mayhem|Customer", meta = (AllowPrivateAccess = "true"))
	ESupermarketMayhemCustomerState CustomerState = ESupermarketMayhemCustomerState::Idle;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Supermarket Mayhem|Customer", meta = (AllowPrivateAccess = "true"))
	FName CustomerType = FName(TEXT("DefaultCustomer"));

private:
	friend class ASupermarketMayhemCustomerSpawnManager;
	void SetCustomerData(USupermarketMayhemCustomerData* NewCustomerData);
	void SetDestinationActorTag(FName NewDestinationActorTag);
};
