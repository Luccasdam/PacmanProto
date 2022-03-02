// Original Author of this code: Luccas de Avila Machado.
//
// This code was made for learning purpose only.
// Hope you can appreciate and learn from this code.
//
// Credits to Youtube "Channel Retro Game Mechanics Explained" for a detailed breakdown of tha Pac-Man Ghost AI.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "LM_PacmanPawn.generated.h"

class USphereComponent;
class USpringArmComponent;
class UCameraComponent;
class UFloatingPawnMovement;
class ALM_Pacman_GM;

UCLASS(Abstract)
class PACMAN_API ALM_PacmanPawn : public APawn
{
	GENERATED_BODY()

	
public:
	ALM_PacmanPawn();

	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintNativeEvent)
	void Die();
		
	bool GetIsAlive() const {return bIsAlive;}

	
protected:
	
	virtual void BeginPlay() override;
	
	void MoveUp();
	void MoveDown();
	void MoveRight();
	void MoveLeft();

	void SetMoveDirection();

	UFUNCTION(BlueprintImplementableEvent)
	void SmoothSetRotation(FRotator DesiredRotation);
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UStaticMeshComponent* PacmanMesh;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	USpringArmComponent* SpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UFloatingPawnMovement* FloatingPawnMovementComponent;

	
	bool bIsAlive = true;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
	bool bCanMove = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Gameplay)
	float MovementSpeed = 450.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SoundFX)
	USoundBase* DieSound;

	
private:
		
	/** This one is just to adjust a better smooth movement when changing directions */
	UPROPERTY(EditInstanceOnly, Category = Gameplay, meta = (AllowPrivateAccess, ClampMin = 0.9f, ClampMax = 1.f))
	float CollisionShapeScaleForMovement = 0.9f;
	
	FVector MoveDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	FVector DesiredMoveDirection;

	UPROPERTY()
	ALM_Pacman_GM* PacmanGM;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool bShowDebugCollision = false;
	
};