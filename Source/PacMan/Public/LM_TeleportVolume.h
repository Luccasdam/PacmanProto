// Original Author of this code: Luccas de Avila Machado.
//
// This code was made for learning purpose only.
// Hope you can appreciate and learn from this code.
//
// Credits to Youtube "Channel Retro Game Mechanics Explained" for a detailed breakdown of tha Pac-Man Ghost AI.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "LM_TeleportVolume.generated.h"



UCLASS()
class PACMAN_API ALM_TeleportVolume : public ATriggerVolume
{
	GENERATED_BODY()

public:
	ALM_TeleportVolume();

protected:
	
	virtual void PostInitializeComponents() override;

private:
	
	UFUNCTION()
	void OnActorOverlap(AActor* OverlappedActor, AActor* OtherActor);

	void TeleportActorTo(AActor* ActorToTeleport);

protected:

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Gameplay)
	AActor* ReferenceActorToTeleport;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Gameplay)
	float TeleportDistance = 100.f;
};
