// Original Author of this code: Luccas de Avila Machado.
//
// This code was made for learning purpose only.
// Hope you can appreciate and learn from this code.
//
// Credits to Youtube "Channel Retro Game Mechanics Explained" for a detailed breakdown of tha Pac-Man Ghost AI.


#include "LM_Pickup.h"
#include "LM_Pacman_GM.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"



ALM_Pickup::ALM_Pickup()
{
	PrimaryActorTick.bCanEverTick = false;
	Tags.Add("Pickup");

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);
	
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(SphereComp);
}


void ALM_Pickup::PostInitializeComponents()
{
	Super::PostInitializeComponents();
		
	OnActorBeginOverlap.AddDynamic(this, &ALM_Pickup::OnOverlap);
}


void ALM_Pickup::OnOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (BaseMesh->IsVisible() && PickupType != EPickup::GhostMovementTile)
	{
		if (OtherActor->ActorHasTag("Player"))
		{			
			ALM_Pacman_GM* PacmanGM = Cast<ALM_Pacman_GM>(GetWorld()->GetAuthGameMode());
			
			BaseMesh->SetVisibility(false);			
			UGameplayStatics::PlaySound2D(this, PickupSound);

			PacmanGM->AddScoreToPlayer(ScoreValue[ScoreIndex]);

			switch (PickupType)
			{
			case EPickup::Food:
				{
					PacmanGM->CountPlayerPicks();
					break;
				}
			case EPickup::Fruit:
				{
					if (ScoreValue.IsValidIndex(ScoreIndex + 1))	++ScoreIndex;
					
					FTimerHandle TimerHandle_RespawnFruit;
					GetWorldTimerManager().SetTimer(TimerHandle_RespawnFruit, this, &ALM_Pickup::RespawnFruit, RespawnTime);
					break;
				}
			case EPickup::PowerUp:
				{
					if (PickupType == EPickup::PowerUp)		PacmanGM->PlayerPoweredUp();
					break;
				}
			}
		}
	}		
}

void ALM_Pickup::RespawnFruit()
{
	BaseMesh->SetVisibility(true);
	
	if (FruitsMesh.IsValidIndex(ScoreIndex))	BaseMesh->SetStaticMesh(FruitsMesh[ScoreIndex]);
}


