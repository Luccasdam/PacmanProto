// Original Author of this code: Luccas de Avila Machado.
//
// This code was made for learning purpose only.
// Hope you can appreciate and learn from this code.
//
// Credits to Youtube "Channel Retro Game Mechanics Explained" for a detailed breakdown of tha Pac-Man Ghost AI.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LM_GhostPawn.generated.h"

#define DebugPrint(String) UKismetSystemLibrary::PrintString(this, TEXT(String), true, false, DebugColor, 0.f);

class USphereComponent;
class UFloatingPawnMovement;
class ALM_PacmanPawn;
class ALM_Pacman_GM;


UENUM()
enum class EGhostType : uint8
{
	Hunter,
	Ambusher,
	Trickster,
	Clyde
};


UENUM()
enum class EGhostState : uint8
{
	Chase,
	Scatter,
	Frightened,
	Eaten,
	Idle,
	Initialize
};

DECLARE_DELEGATE_OneParam(FOnSetNewGhostState, EGhostState)


UCLASS(Abstract)
class PACMAN_API ALM_GhostPawn : public APawn
{
	GENERATED_BODY()

public:
	ALM_GhostPawn();
	
	virtual void Tick(float DeltaTime) override;

protected:
	
	virtual void PostInitializeComponents() override;
	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GameStart();

	UFUNCTION()
	void SetGhostToFrightened();

	UFUNCTION(BlueprintCallable)
	void SetGhostState();
	
	/**
	 * Smoothly update CurrentMovementSpeed to NewMovespeed.
	 * @param ResetDelay	If ResetDelay = 0 MoveSpeed will not reset.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void SetMovementSpeedSmoothly(float NewSpeed, float ResetDelay = 0.f);

	UFUNCTION(BlueprintImplementableEvent)
	void SetRotationSmoothly(FRotator DesiredRotation);

	UFUNCTION(BlueprintImplementableEvent)
	void SetIdleLocationAndRotationSmoothly();


private:

	UFUNCTION()
	void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	ALM_GhostPawn* GetHunterPawn() const;
	
	void SetMoveDirection();
		
	void SetDesiredMoveDirection();

	void InvertMoveDirection();

	void CreateValidLocations();
	FVector CheckValidLocation(FVector Direction) const;

	FVector GetTargetMoveLocation();
	FVector GetBestDirection();
	
	void PrintDebugGhostState() const;


protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UStaticMeshComponent* GhostMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UStaticMeshComponent* GhostEye;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UFloatingPawnMovement* FloatingPawnMovementComponent;

	
	FTimerHandle TimerHandle_SetGhostState;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = Gameplay)
	EGhostType GhostType;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = Gameplay)
	FVector GhostColor = FVector::OneVector;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = Gameplay, meta = (MakeEditWidget))
	FVector ScatterLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	int32 ScoreValue = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float ChaseTime = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float ScatterTime = 7.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float IdleTime = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float InitializeGhostStateTimer = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float TimeToFullyUpdateMovementSpeed = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float MovementSpeed = 440.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float FrightenedMovementSpeed = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float EatenMovementSpeed = 600.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	int32 AmbusherTargetingTilesOffset = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	int32 TricksterTargetingTilesOffset = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	int32 ClydeTargetingTilesOffset = 8;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SoundFX)
	USoundBase* EatenSound;

	
private:

	/** Use Pac-mans original Targeting style */
	UPROPERTY(EditInstanceOnly, Category = Gameplay, meta = (AllowPrivateAccess))
	bool bUseOriginalTargeting = true;

	/** This one is just to adjust a better smooth movement when changing directions */
	UPROPERTY(EditInstanceOnly, Category = Gameplay, meta = (AllowPrivateAccess, ClampMin = 0.9f, ClampMax = 1.f))
	float CollisionShapeScaleForMovement = 0.9f;
	
	UPROPERTY()
	ALM_Pacman_GM* PacmanGM;
	
	UPROPERTY()
	ALM_PacmanPawn* PacmanPawn;
	
	UPROPERTY()
	ALM_GhostPawn* HunterPawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	FVector SpawnLocation;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	EGhostState GhostState = EGhostState::Idle;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	EGhostState NextGhostState = EGhostState::Idle;
	

	FVector MoveDirection;
	
	FVector DesiredMoveDirection;

	FVector CurrentTileLocation;

	UPROPERTY(VisibleAnywhere)
	TArray<FVector> ValidMoveLocations;

	FVector TargetMoveLocation;
			

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	FColor DebugColor = FColor::White;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	bool bShowDebugGhostState = false;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	bool bShowDebugTargetMoveLocation = false;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	bool bShowDebugMoveCollision = false;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	bool bShowDebugText = false;
};


inline void ALM_GhostPawn::SetGhostToFrightened()
{
	if (GhostState == EGhostState::Chase || GhostState == EGhostState::Scatter || GhostState == EGhostState::Frightened)
	{		
		NextGhostState = EGhostState::Frightened;
		SetGhostState();
	};
}

inline void ALM_GhostPawn::PrintDebugGhostState() const
{
	switch (GhostState)
	{
	case EGhostState::Chase:
		{
			DebugPrint("GhostState: Chase")
			break;
		}
	case EGhostState::Scatter:
		{
			DebugPrint("GhostState: Scatter")
			break;
		}
	case EGhostState::Frightened:
		{
			DebugPrint("GhostState: Frightened")
			break;
		}
	case EGhostState::Eaten:
		{
			DebugPrint("GhostState: Eaten")
			break;
		}
	case EGhostState::Idle:
		{
			DebugPrint("GhostState: Idle")
			break;
		}
	case EGhostState::Initialize:
		{
			DebugPrint("GhostState: Initialize")
			break;
		}
	}
}
