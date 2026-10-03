// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SupermarketMayhemInteractiveActor.h"
#include "SupermarketMayhemShelf.generated.h"

class ASupermarketMayhemProduct;

/** A tagged shopping destination that can reference one or more placed products. */
UCLASS(Blueprintable)
class SUPERMARKETMAYHEM_API ASupermarketMayhemShelf : public ASupermarketMayhemInteractiveActor
{
	GENERATED_BODY()

public:
	ASupermarketMayhemShelf();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	const TArray<TObjectPtr<ASupermarketMayhemProduct>>& GetProducts() const { return Products; }

protected:
	UPROPERTY(EditInstanceOnly, Replicated, BlueprintReadOnly, Category = "Supermarket Mayhem|Shelf|Products")
	TArray<TObjectPtr<ASupermarketMayhemProduct>> Products;
};
