// Original Author of this code: Luccas de Avila Machado.
//
// This code was made for learning purpose only.
// Hope you can appreciate and learn from this code.
//
// Credits to Youtube "Channel Retro Game Mechanics Explained" for a detailed breakdown of tha Pac-Man Ghost AI.


#include "LM_GhostWallVolume.h"
#include "Components/BrushComponent.h"


ALM_GhostWallVolume::ALM_GhostWallVolume()
{
	Tags.AddUnique("GhostWall");
	
	GetBrushComponent()->SetCollisionProfileName("GhostWall");
}



