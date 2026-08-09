// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SupermarketMayhemTypes.generated.h"

/**
 *  Gameplay role a player currently has in a match.
 *  See Docs/GAME_DESIGN.md, "Roles" and Docs/GAMEPLAY.md.
 */
UENUM(BlueprintType)
enum class ESupermarketMayhemPlayerRole : uint8
{
	/** No role has been assigned yet (e.g. lobby, before role assignment). */
	None,

	/** Player disguises as a supermarket product/prop and hides. */
	Hider,

	/** Player searches for Hiders and uses weapons to eliminate them. */
	Hunter
};

/**
 *  Phase of the current round.
 *  See Docs/GAMEPLAY.md, "Round".
 */
UENUM(BlueprintType)
enum class ESupermarketMayhemRoundState : uint8
{
	/** Round has not started yet (e.g. waiting for players/lobby). */
	WaitingToStart,

	/** Hiders choose a disguise and hiding spot before Hunters are released. */
	Preparation,

	/** Hunters are released and actively search for Hiders. */
	Hunt,

	/** Hunt has ended; results are shown before a rematch/queue. */
	Result
};
