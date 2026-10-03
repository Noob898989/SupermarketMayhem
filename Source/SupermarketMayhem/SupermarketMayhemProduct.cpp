// Copyright Epic Games, Inc. All Rights Reserved.

#include "SupermarketMayhemProduct.h"

#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"

ASupermarketMayhemProduct::ASupermarketMayhemProduct()
{
	InteractionType = FName(TEXT("Product"));
	DisplayName = NSLOCTEXT("SupermarketMayhem", "ProductDisplayName", "Product");
	InteractionBoundsExtent = FVector(40.0f);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaceholderMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (PlaceholderMesh.Succeeded())
	{
		VisualMesh->SetStaticMesh(PlaceholderMesh.Object);
		VisualMesh->SetRelativeScale3D(FVector(0.35f));
	}
}

void ASupermarketMayhemProduct::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASupermarketMayhemProduct, ProductId);
	DOREPLIFETIME(ASupermarketMayhemProduct, ProductType);
	DOREPLIFETIME(ASupermarketMayhemProduct, Weight);
}
