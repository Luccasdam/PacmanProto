// Original Author of this code: Luccas de Avila Machado.
//
// This code was made for learning purpose only.
// Hope you can appreciate and learn from this code.
//
// Credits to Youtube "Channel Retro Game Mechanics Explained" for a detailed breakdown of tha Pac-Man Ghost AI.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LM_Pickup.generated.h"


UENUM(BlueprintType)
enum class EPickup : uint8
{
	GhostMovementTile,
	Food,
	Fruit,
	PowerUp
};


class USphereComponent;

UCLASS(Abstract)
class PACMAN_API ALM_Pickup : public AActor
{
	GENERATED_BODY()

public:
	ALM_Pickup();

	
protected:

	virtual void PostInitializeComponents() override;
	
	UFUNCTION()
	void OnOverlap(AActor* OverlappedActor, AActor* OtherActor);	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UStaticMeshComponent* BaseMesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Gameplay)
	EPickup PickupType = EPickup::Food;

	/**
	 * Use index 0 for the default value. Other indexes are only valid for Fruits.
	 * GhostMovementTile does not use Score.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Gameplay)
	TArray<int32> ScoreValue;

	/**
	 * This is only used by Fruit Type.
	 * Use the same Static Mesh for BaseMesh at Array 0.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Gameplay)
	TArray<UStaticMesh*> FruitsMesh;

	/**
	 * This is only used by Fruit Type.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float RespawnTime = 30.f;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SoundFX)
	USoundBase* PickupSound;

private:

	int32 ScoreIndex = 0;

	void RespawnFruit();
};
