// Original Author of this code: Luccas de Avila Machado.
//
// This code was made for learning purpose only.
// Hope you can appreciate and learn from this code.
//
// Credits to Youtube "Channel Retro Game Mechanics Explained" for a detailed breakdown of tha Pac-Man Ghost AI.


#include "LM_PacmanPawn.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "LM_Pacman_GM.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

#define ECC_GhostWall ECC_GameTraceChannel1


ALM_PacmanPawn::ALM_PacmanPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Comp"));
	SetRootComponent(SphereComp);
	SphereComp->SetSphereRadius(49.f);
	
	PacmanMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	PacmanMesh->SetupAttachment(SphereComp);
		
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Long Arm"));
	SpringArm->SetupAttachment(SphereComp);
	SpringArm->bDoCollisionTest = false;
	SpringArm->bInheritYaw = false;
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Far Camera"));
	Camera->SetupAttachment(SpringArm);
		
	FloatingPawnMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Floating Pawn Movement"));
	FloatingPawnMovementComponent->MaxSpeed = MovementSpeed;
}

void ALM_PacmanPawn::BeginPlay()
{
	Super::BeginPlay();

	MoveDirection = GetActorForwardVector();

	PacmanGM = Cast<ALM_Pacman_GM>(GetWorld()->GetAuthGameMode());
}

void ALM_PacmanPawn::Die_Implementation()
{
	if (bIsAlive)
	{
		bIsAlive = false;
		DisableInput(nullptr);
		PacmanMesh->SetScalarParameterValueOnMaterials("TimeToDissolve", GetWorld()->GetTimeSeconds());

		UGameplayStatics::PlaySound2D(this, DieSound);
	}	
}

void ALM_PacmanPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCanMove)
	{
		SetMoveDirection();	
		AddMovementInput(MoveDirection, 1);
	}
}

void ALM_PacmanPawn::SetMoveDirection()
{
	if (MoveDirection != DesiredMoveDirection)
	{
		FHitResult HitResult;
		const FVector EndLocation = GetActorLocation() + (DesiredMoveDirection * PacmanGM->GetTileSize());

		const FVector BoxExtend = FVector::OneVector * PacmanGM->GetTileSize()/2 * CollisionShapeScaleForMovement;
		FCollisionShape CollisionShape;
		CollisionShape.SetBox(BoxExtend);
	
		FCollisionObjectQueryParams CollisionObjectQueryParams;
		CollisionObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		CollisionObjectQueryParams.AddObjectTypesToQuery(ECC_GhostWall);
	
		if(GetWorld()->SweepSingleByObjectType(HitResult, GetActorLocation(), EndLocation, FQuat::Identity, CollisionObjectQueryParams, CollisionShape) == false)
		{
			MoveDirection = DesiredMoveDirection;
			SmoothSetRotation(MoveDirection.Rotation());
		}

		if (bShowDebugCollision)
		{
			const FColor BoxColor = HitResult.bBlockingHit ? FColor::Red : FColor::Green;	
			DrawDebugBox(GetWorld(), EndLocation, BoxExtend * 1.2f, BoxColor, false, -1.f, 0, 2.f);
		}		
	}
}

void ALM_PacmanPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("MoveUp", IE_Pressed, this, &ALM_PacmanPawn::MoveUp);
	PlayerInputComponent->BindAction("MoveDown", IE_Pressed, this, &ALM_PacmanPawn::MoveDown);
	PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &ALM_PacmanPawn::MoveRight);
	PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &ALM_PacmanPawn::MoveLeft);
}

void ALM_PacmanPawn::MoveUp()		{DesiredMoveDirection = FVector::ForwardVector;}
void ALM_PacmanPawn::MoveLeft()		{DesiredMoveDirection = FVector::LeftVector;}
void ALM_PacmanPawn::MoveDown()		{DesiredMoveDirection = FVector::BackwardVector;}
void ALM_PacmanPawn::MoveRight()	{DesiredMoveDirection = FVector::RightVector;}
