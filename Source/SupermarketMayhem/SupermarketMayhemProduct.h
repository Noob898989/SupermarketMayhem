// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SupermarketMayhemInteractiveActor.h"
#include "SupermarketMayhemProduct.generated.h"

/** Minimal replicated grocery item; inventory, pickup and throwing are future work. */
UCLASS(Blueprintable)
class SUPERMARKETMAYHEM_API ASupermarketMayhemProduct : public ASupermarketMayhemInteractiveActor
{
	GENERATED_BODY()

public:
	ASupermarketMayhemProduct();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	FName GetProductId() const { return ProductId; }

protected:
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Supermarket Mayhem|Product")
	FName ProductId = FName(TEXT("Product"));

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Supermarket Mayhem|Product")
	FName ProductType = FName(TEXT("Grocery"));

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Supermarket Mayhem|Product", meta = (ClampMin = "0.0"))
	float Weight = 1.0f;
};
