// Original Author of this code: Luccas de Avila Machado.
//
// This code was made for learning purpose only.
// Hope you can appreciate and learn from this code.
//
// Credits to Youtube "Channel Retro Game Mechanics Explained" for a detailed breakdown of tha Pac-Man Ghost AI.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LM_Pacman_GM.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerOverpowered);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerWin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerScoreUpdate, int32, inPlayerScore);

UCLASS(Abstract)
class PACMAN_API ALM_Pacman_GM : public AGameModeBase
{
	GENERATED_BODY()

public:	

	UFUNCTION(BlueprintCallable)
	void PlayerPoweredUp();
		
	int32 GetScoreMultiplier();

	void AddScoreToPlayer(int32 inDeltaScore);

	void CountPlayerPicks();
	

	float GetGhostFrightenedTime() const	{return GhostFrightenedTime;}

	float GetTileSize() const	{return TileSize;}
	

protected:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void GetPickUpsNum();


private:

	void EndPowerUp();
	

public:

	UPROPERTY(BlueprintAssignable)
	FOnPlayerScoreUpdate OnPlayerScoreUpdate;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerOverpowered OnPlayerOverpowered;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerWin OnPlayerWin;

	
private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 PickupCount;
	
	int32 PlayerScore = 0;

	int32 GhostScoreMultiplier = 1;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess))
	float GhostFrightenedTime = 8.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SoundFx, meta = (AllowPrivateAccess))
	USoundBase* GhostFrightenedSound;
	
	UPROPERTY()
	UAudioComponent* FrightenedSoundComp;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Gameplay, meta = (AllowPrivateAccess))
	float TileSize = 100.f;
};
