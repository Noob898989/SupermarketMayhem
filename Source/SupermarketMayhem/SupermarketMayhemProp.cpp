// Copyright Epic Games, Inc. All Rights Reserved.

#include "SupermarketMayhemProp.h"
#include "Components/StaticMeshComponent.h"

ASupermarketMayhemProp::ASupermarketMayhemProp()
{
	PrimaryActorTick.bCanEverTick = false;

	PropMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PropMesh"));
	SetRootComponent(PropMesh);

	CachedCollisionEnabled = ECollisionEnabled::QueryAndPhysics;
}

void ASupermarketMayhemProp::SetWornByHider(bool bWorn)
{
	if (!PropMesh)
	{
		return;
	}

	if (bWorn)
	{
		CachedCollisionEnabled = PropMesh->GetCollisionEnabled();

		PropMesh->SetVisibility(false, true);
		PropMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		PropMesh->SetVisibility(true, true);
		PropMesh->SetCollisionEnabled(CachedCollisionEnabled);
	}
}
