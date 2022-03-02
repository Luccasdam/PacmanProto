// Original Author of this code: Luccas de Avila Machado.
//
// This code was made for learning purpose only.
// Hope you can appreciate and learn from this code.
//
// Credits to Youtube "Channel Retro Game Mechanics Explained" for a detailed breakdown of tha Pac-Man Ghost AI.


#include "LM_TeleportVolume.h"
#include "Components/BrushComponent.h"


ALM_TeleportVolume::ALM_TeleportVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	
	GetBrushComponent()->SetCollisionProfileName("Trigger");
	GetBrushComponent()->SetGenerateOverlapEvents(true);
}

void ALM_TeleportVolume::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OnActorBeginOverlap.AddDynamic(this, &ALM_TeleportVolume::OnActorOverlap);
}

void ALM_TeleportVolume::OnActorOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	GetWorldTimerManager().SetTimerForNextTick([OtherActor, this]() {TeleportActorTo(OtherActor); });
}

void ALM_TeleportVolume::TeleportActorTo(AActor* ActorToTeleport)
{
	const FVector TeleportLocation = ReferenceActorToTeleport->GetActorLocation() + (ReferenceActorToTeleport->GetActorForwardVector() * TeleportDistance);

	ActorToTeleport->TeleportTo(TeleportLocation, ActorToTeleport->GetActorRotation(), false, true);
}

