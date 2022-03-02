// Original Author of this code: Luccas de Avila Machado.
//
// This code was made for learning purpose only.
// Hope you can appreciate and learn from this code.
//
// Credits to Youtube "Channel Retro Game Mechanics Explained" for a detailed breakdown of tha Pac-Man Ghost AI.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "LM_GhostWallVolume.generated.h"

UCLASS(BlueprintType)
class PACMAN_API ALM_GhostWallVolume : public ATriggerVolume
{
	GENERATED_BODY()

public:
	ALM_GhostWallVolume();
	
	
};
