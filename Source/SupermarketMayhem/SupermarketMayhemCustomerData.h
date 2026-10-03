// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SupermarketMayhemCustomerData.generated.h"

UENUM(BlueprintType)
enum class ESupermarketMayhemCustomerState : uint8
{
	Idle,
	Walk,
	Shop,
	Paused
};

/** Small, reusable behavior profile for a supermarket customer. */
UCLASS(BlueprintType)
class SUPERMARKETMAYHEM_API USupermarketMayhemCustomerData : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Optional content label for the customer archetype. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Customer")
	FName CustomerType = FName(TEXT("DefaultCustomer"));

	/** Optional actor tag preference, for example CustomerDestination.Aisle. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Customer|Behavior")
	FName PreferredDestinationTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Customer|Movement", meta = (ClampMin = "0.0"))
	float MovementSpeed = 180.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Customer|Behavior", meta = (ClampMin = "0.0"))
	float MinimumIdleDuration = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Customer|Behavior", meta = (ClampMin = "0.0"))
	float MaximumIdleDuration = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Customer|Behavior", meta = (ClampMin = "0.0"))
	float MinimumShoppingDuration = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Customer|Behavior", meta = (ClampMin = "0.0"))
	float MaximumShoppingDuration = 6.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Customer|Reactions", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ReactionSensitivity = 0.5f;
};
