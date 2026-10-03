// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SupermarketMayhemCustomerSpawnManager.generated.h"

class ASupermarketMayhemCustomer;
class USupermarketMayhemCustomerData;

/** Server-only owner of customer population and round lifecycle. */
UCLASS(NotBlueprintable)
class SUPERMARKETMAYHEM_API ASupermarketMayhemCustomerSpawnManager : public AActor
{
	GENERATED_BODY()

public:
	ASupermarketMayhemCustomerSpawnManager();
	void Configure(int32 InMinimumCount, int32 InMaximumCount, int32 InInitialCount,
		TSubclassOf<ASupermarketMayhemCustomer> InCustomerClass,
		USupermarketMayhemCustomerData* InCustomerData, FName InSpawnTag,
		FName InDestinationTag, float InSpawnSpread);
	void BeginPreparationPhase();
	void BeginHuntPhase();
	void BeginResultPhase();
	void ReportNoise(FVector NoiseLocation, float Intensity, float HearingRange);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Customers", meta = (ClampMin = "0", ClampMax = "32"))
	int32 MinimumCustomerCount = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Customers", meta = (ClampMin = "0", ClampMax = "32"))
	int32 MaximumCustomerCount = 8;

	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Customers", meta = (ClampMin = "0", ClampMax = "32"))
	int32 InitialCustomerCount = 4;

	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Customers")
	TSubclassOf<ASupermarketMayhemCustomer> CustomerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Customers")
	TObjectPtr<USupermarketMayhemCustomerData> CustomerData;

	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Customers|Level Tags")
	FName CustomerSpawnTag = FName(TEXT("CustomerSpawn"));

	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Customers|Level Tags")
	FName CustomerDestinationTag = FName(TEXT("CustomerDestination"));

	UPROPERTY(EditDefaultsOnly, Category = "Supermarket Mayhem|Customers|Spawning", meta = (ClampMin = "0.0"))
	float SpawnSpread = 250.0f;

private:
	void SpawnInitialCustomers();
	FTransform ChooseSpawnTransform(const TArray<AActor*>& SpawnPoints, int32 Index) const;
	void SetAllCustomerBehaviors(bool bEnabled, bool bResetState);

	UPROPERTY(Transient)
	TArray<TObjectPtr<ASupermarketMayhemCustomer>> Customers;
	bool bPopulationSpawned = false;
};
