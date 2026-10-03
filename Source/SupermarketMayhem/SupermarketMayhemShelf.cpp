// Copyright Epic Games, Inc. All Rights Reserved.

#include "SupermarketMayhemShelf.h"

#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"

ASupermarketMayhemShelf::ASupermarketMayhemShelf()
{
	DisplayName = NSLOCTEXT("SupermarketMayhem", "ShelfDisplayName", "Shelf");
	InteractionType = FName(TEXT("Shop"));
	InteractionBoundsExtent = FVector(50.0f);
	VisualMesh->SetRelativeScale3D(FVector(2.0f, 0.8f, 2.2f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaceholderMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (PlaceholderMesh.Succeeded())
	{
		VisualMesh->SetStaticMesh(PlaceholderMesh.Object);
	}
	Tags.AddUnique(FName(TEXT("CustomerDestination")));
	Tags.AddUnique(FName(TEXT("ShoppingTarget")));
}

void ASupermarketMayhemShelf::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASupermarketMayhemShelf, Products);
}
