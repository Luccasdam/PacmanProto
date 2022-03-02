// Original Author of this code: Luccas de Avila Machado.
//
// This code was made for learning purpose only.
// Hope you can appreciate and learn from this code.
//
// Credits to Youtube "Channel Retro Game Mechanics Explained" for a detailed breakdown of tha Pac-Man Ghost AI.


#include "LM_Pacman_GM.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

void ALM_Pacman_GM::BeginPlay()
{
	Super::BeginPlay();

	GetPickUpsNum();
}


void ALM_Pacman_GM::PlayerPoweredUp()
{
	OnPlayerOverpowered.Broadcast();

	FrightenedSoundComp = UGameplayStatics::SpawnSound2D(this, GhostFrightenedSound);

	FTimerHandle TimerHandle_ResetScoreMultiplier;
	GetWorldTimerManager().SetTimer(TimerHandle_ResetScoreMultiplier, this, &ALM_Pacman_GM::EndPowerUp, GhostFrightenedTime);
}

void ALM_Pacman_GM::EndPowerUp()
{
	FrightenedSoundComp->Deactivate();
	GhostScoreMultiplier = 1;
}

void ALM_Pacman_GM::AddScoreToPlayer(int32 inDeltaScore)
{
	PlayerScore += inDeltaScore;
	OnPlayerScoreUpdate.Broadcast(PlayerScore);
}

int32 ALM_Pacman_GM::GetScoreMultiplier()
{
	GhostScoreMultiplier *= 2;
	return GhostScoreMultiplier;
}

void ALM_Pacman_GM::CountPlayerPicks()
{
	--PickupCount;		
	if (PickupCount == 0)	{OnPlayerWin.Broadcast();}
}