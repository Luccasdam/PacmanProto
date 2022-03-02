// Original Author of this code: Luccas de Avila Machado.
//
// This code was made for learning purpose only.
// Hope you can appreciate and learn from this code.
//
// Credits to Youtube "Channel Retro Game Mechanics Explained" for a detailed breakdown of tha Pac-Man Ghost AI.



#include "LM_GhostPawn.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "LM_Pacman_GM.h"
#include "LM_PacmanPawn.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

#define ECC_GhostWall ECC_GameTraceChannel1


ALM_GhostPawn::ALM_GhostPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	Tags.AddUnique("Ghost");

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Comp"));
	SetRootComponent(SphereComp);
	SphereComp->SetSphereRadius(45.f);
	SphereComp->SetCollisionProfileName("GhostPawn");
	
	GhostMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	GhostMesh->SetupAttachment(SphereComp);
	GhostMesh->SetCollisionProfileName("NoCollision");

	GhostEye = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ghost Eye"));
	GhostEye->SetupAttachment(SphereComp);
	GhostEye->SetCollisionProfileName("NoCollision");
	GhostEye->SetVisibility(false);

	FloatingPawnMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Floating Pawn Movement"));
	FloatingPawnMovementComponent->MaxSpeed = MovementSpeed;
	FloatingPawnMovementComponent->TurningBoost = 9000;
}

void ALM_GhostPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OnActorBeginOverlap.AddDynamic(this, &ALM_GhostPawn::OnBeginOverlap);
}

void ALM_GhostPawn::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{	
	if (OtherActor->ActorHasTag("Pickup"))
	{
		CurrentTileLocation = OtherActor->GetActorLocation();
		
		SetDesiredMoveDirection();
	}

	if (OtherActor->ActorHasTag("Player"))
	{
		if (GhostState == EGhostState::Chase || GhostState == EGhostState::Scatter)		PacmanPawn->Die();
		
		else if (GhostState == EGhostState::Frightened)
		{
			const int32 ScoreToAdd = ScoreValue * PacmanGM->GetScoreMultiplier();
			PacmanGM->AddScoreToPlayer(ScoreToAdd);

			const FVector TextLocation = GetActorLocation() + FVector(0.f, 0.f, 100.f);
			DrawDebugString(GetWorld(), TextLocation, FString::Printf(TEXT("%i"), ScoreToAdd), 0, FColor::White, 0.5f);

			UGameplayStatics::PlaySound2D(this, EatenSound);
			
			NextGhostState = EGhostState::Eaten;
			SetGhostState();
		}
	}
}

void ALM_GhostPawn::BeginPlay()
{
	Super::BeginPlay();
		
	SpawnLocation = GetActorLocation();
	CurrentTileLocation = SpawnLocation;

	PacmanPawn = Cast<ALM_PacmanPawn>(UGameplayStatics::GetActorOfClass(this, ALM_PacmanPawn::StaticClass()));
	HunterPawn = GhostType == EGhostType::Trickster ? GetHunterPawn() : nullptr;	

	PacmanGM = Cast<ALM_Pacman_GM>(GetWorld()->GetAuthGameMode());
	if (ensure(PacmanGM))	PacmanGM->OnPlayerOverpowered.AddDynamic(this, &ALM_GhostPawn::SetGhostToFrightened);
}

// Called on BP Begin Play, so you can implement some BeginPlay logic before the game start, since the C++ Begin Play is called before BP Begin Play.
void ALM_GhostPawn::GameStart_Implementation()
{		
	SetGhostState();
	SetDesiredMoveDirection();
}

ALM_GhostPawn* ALM_GhostPawn::GetHunterPawn() const
{
	TArray<AActor*> GhostActors;
	UGameplayStatics::GetAllActorsOfClass(this, ALM_GhostPawn::StaticClass(), GhostActors);
	for (AActor* GhostActor : GhostActors)
	{
		ALM_GhostPawn* Ghost = Cast<ALM_GhostPawn>(GhostActor);
		if (Ghost->GhostType == EGhostType::Hunter)		{return Ghost;}
	}	
	return nullptr;
}


void ALM_GhostPawn::SetGhostState()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_SetGhostState);	
	float NextStateTimer = 0.f;
	
	switch (NextGhostState)
	{
	case EGhostState::Chase:
		{
			GhostState = EGhostState::Chase;
			NextGhostState = EGhostState::Scatter;
			NextStateTimer = ChaseTime;			
			break;
		}		
	case EGhostState::Scatter:
		{
			GhostState = EGhostState::Scatter;
			NextGhostState = EGhostState::Chase;
			NextStateTimer = ScatterTime;
			
			InvertMoveDirection();			
			GhostMesh->SetScalarParameterValueOnMaterials("IsFrightened", false);			
			break;
		}
	case EGhostState::Frightened:
		{			
			GhostState = EGhostState::Frightened;
			NextGhostState = EGhostState::Scatter;
			NextStateTimer = PacmanGM->GetGhostFrightenedTime();

			InvertMoveDirection();			
			GhostMesh->SetScalarParameterValueOnMaterials("IsFrightened", true);
			SetMovementSpeedSmoothly(FrightenedMovementSpeed, NextStateTimer);
			break;
		}
	case EGhostState::Eaten:
		{
			GhostState = EGhostState::Eaten;
			NextGhostState = EGhostState::Idle;
			NextStateTimer = 0.f;	// This one does not use the Handle. The tick event take care of this one.
			
			GhostEye->SetVisibility(true);
			GhostMesh->SetVisibility(false);
			GhostMesh->SetScalarParameterValueOnMaterials("IsFrightened", false);
			SetMovementSpeedSmoothly(EatenMovementSpeed);
			break;
		}
	case EGhostState::Idle:
		{
			GhostState = EGhostState::Idle;
			NextGhostState = EGhostState::Initialize;
			NextStateTimer = IdleTime;
			SetMovementSpeedSmoothly(MovementSpeed);
			break;
		}
	case EGhostState::Initialize:
		{
			GhostState = EGhostState::Initialize;
			NextGhostState = EGhostState::Chase;
			NextStateTimer = InitializeGhostStateTimer;			
			break;
		}
	}	
	if (NextStateTimer != 0.f)	{GetWorldTimerManager().SetTimer(TimerHandle_SetGhostState, this, &ALM_GhostPawn::SetGhostState, NextStateTimer);}
}


void ALM_GhostPawn::InvertMoveDirection()
{
	DesiredMoveDirection = -MoveDirection;

	// If inverting direction wont trigger another direction change, this will force this change.
	const FVector BehindLocation = CheckValidLocation(-MoveDirection);
	if (BehindLocation == FVector::ZeroVector)	SetDesiredMoveDirection();
}



void ALM_GhostPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GhostState != EGhostState::Idle)
	{
		SetMoveDirection();
		AddMovementInput(MoveDirection, 1);
	}

	// DEBUG Visualization
	if (bShowDebugGhostState)	PrintDebugGhostState();
	if (bShowDebugTargetMoveLocation)	DrawDebugBox(GetWorld(), TargetMoveLocation, FVector::OneVector * PacmanGM->GetTileSize()/2, DebugColor, false, -1.f, 0, 5.f);
}


void ALM_GhostPawn::SetMoveDirection()
{
	if (MoveDirection != DesiredMoveDirection)
	{
		FHitResult HitResult;		
		const FVector EndLocation = GetActorLocation() + (DesiredMoveDirection * PacmanGM->GetTileSize());
		const FVector BoxExtend = FVector::OneVector * PacmanGM->GetTileSize()/3 * CollisionShapeScaleForMovement;

		FCollisionShape CollisionShape;
		CollisionShape.SetBox(BoxExtend);

		FCollisionObjectQueryParams CollisionObjectQueryParams;
		CollisionObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);		
		if (GhostState == EGhostState::Chase || GhostState == EGhostState::Scatter || GhostState == EGhostState::Frightened)	CollisionObjectQueryParams.AddObjectTypesToQuery((ECC_GhostWall));
		
		
		if(!GetWorld()->SweepSingleByObjectType(HitResult, GetActorLocation(), EndLocation, FQuat::Identity, CollisionObjectQueryParams, CollisionShape))
		{
			MoveDirection = DesiredMoveDirection;
			SetRotationSmoothly(MoveDirection.Rotation());
		}
	}	
}

void ALM_GhostPawn::SetDesiredMoveDirection()
{		
	CreateValidLocations();

	TargetMoveLocation = GetTargetMoveLocation();
	
	DesiredMoveDirection = GetBestDirection();
}

void ALM_GhostPawn::CreateValidLocations()
{
	ValidMoveLocations.Reset();

	// The Create order is to keep the priority equal to the original
	// Since the game logic is playing using the Plane XY, Forward and Backward Vectors are equivalent to Up and Down Vectors.
	
	const FVector UpLocation = CheckValidLocation(FVector::ForwardVector);
	if (UpLocation != FVector::ZeroVector)		ValidMoveLocations.AddUnique((UpLocation));

	const FVector LeftLocation = CheckValidLocation(FVector::LeftVector);
	if (LeftLocation != FVector::ZeroVector)	ValidMoveLocations.AddUnique((LeftLocation));

	const FVector DownLocation = CheckValidLocation(FVector::BackwardVector);
	if (DownLocation != FVector::ZeroVector)	ValidMoveLocations.AddUnique((DownLocation));

	const FVector RightLocation = CheckValidLocation(FVector::RightVector);
	if (RightLocation != FVector::ZeroVector)	ValidMoveLocations.AddUnique((RightLocation));
}

FVector ALM_GhostPawn::CheckValidLocation(FVector Direction) const
{
	FHitResult HitResult;
	const FVector CheckLocation = CurrentTileLocation + (Direction * PacmanGM->GetTileSize());

	const FVector BoxExtend = FVector::OneVector * PacmanGM->GetTileSize()/4 * CollisionShapeScaleForMovement;
	FCollisionShape CollisionShape;
	CollisionShape.SetBox(BoxExtend);
	
	FCollisionObjectQueryParams CollisionObjectQueryParams;
	CollisionObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);	
	if (GhostState == EGhostState::Chase || GhostState == EGhostState::Scatter || GhostState == EGhostState::Frightened)
	{
		CollisionObjectQueryParams.AddObjectTypesToQuery((ECC_GhostWall));
	}
		
	if(GetWorld()->SweepSingleByObjectType(HitResult, CurrentTileLocation, CheckLocation, FQuat::Identity, CollisionObjectQueryParams, CollisionShape) == false)
	{
		// DEBUG Visualization
		if (bShowDebugMoveCollision)	DrawDebugLine(GetWorld(), CurrentTileLocation, HitResult.TraceEnd, FColor::Green, false, .5f, 0, 5.f);
		
		return CheckLocation;		
	}
	else	return FVector::ZeroVector;
}

FVector ALM_GhostPawn::GetTargetMoveLocation()
{
	switch (GhostState)
	{		
	case EGhostState::Chase:
		{
			switch (GhostType)
			{
			case EGhostType::Hunter:	{return PacmanPawn->GetActorLocation();}
				
			case EGhostType::Ambusher:
				{
					FVector TargetLocation = (PacmanPawn->GetActorLocation() + (PacmanPawn->GetActorForwardVector() * AmbusherTargetingTilesOffset * PacmanGM->GetTileSize()));

					if (bUseOriginalTargeting && PacmanPawn->GetActorForwardVector() == FVector::ForwardVector)
					{
						TargetLocation += (FVector::LeftVector * AmbusherTargetingTilesOffset  * PacmanGM->GetTileSize());
					}										
					return TargetLocation;
				}
				
			case EGhostType::Trickster:
				{
					FVector BaseTargetLocation = PacmanPawn->GetActorLocation() + (PacmanPawn->GetActorForwardVector() * TricksterTargetingTilesOffset * PacmanGM->GetTileSize());

					if (bUseOriginalTargeting && PacmanPawn->GetActorForwardVector() == FVector::ForwardVector)
					{
							BaseTargetLocation += (FVector::LeftVector * TricksterTargetingTilesOffset * PacmanGM->GetTileSize());
					}					
					const FVector TargetLocation = (BaseTargetLocation - HunterPawn->GetActorLocation()) + BaseTargetLocation;
										
					return TargetLocation;
				}
				
			case EGhostType::Clyde:
				{
					const float DistanceToPlayer = (GetActorLocation() -  PacmanPawn->GetActorLocation()).Size();
					const FVector TargetLocation = DistanceToPlayer > (ClydeTargetingTilesOffset  * PacmanGM->GetTileSize()) ? PacmanPawn->GetActorLocation() : ScatterLocation; 
															
					return TargetLocation;
				}
			}
		}
		
	case EGhostState::Scatter:	{return ScatterLocation;}

		// -MoveDirection is already out of scope
	case EGhostState::Frightened:
		{
			while (true)
			{
				FVector TargetLocation = ValidMoveLocations[FMath::RandHelper(ValidMoveLocations.Num())];
				if (TargetLocation != MoveDirection)	{return TargetLocation;}
			}
		}

	case EGhostState::Eaten:
		{
			if (FVector::PointsAreNear(GetActorLocation(), SpawnLocation, PacmanGM->GetTileSize()))
			{
				SetIdleLocationAndRotationSmoothly();
				NextGhostState = EGhostState::Idle;
				SetGhostState();
			}		
			
			return SpawnLocation;
		}

	case EGhostState::Idle:	{return SpawnLocation;}

	case EGhostState::Initialize: {return (SpawnLocation + (SpawnLocation + (FVector::ForwardVector * 10.f * PacmanGM->GetTileSize())));}
	}
	
	return ScatterLocation;
}

FVector ALM_GhostPawn::GetBestDirection()
{		
	if (ensure(ValidMoveLocations.IsValidIndex(0)))
	{
		float ShortestDistance = 9000.f;
		int32 BestIndex = 0;
		
		for (uint8 i = 0; i < ValidMoveLocations.Num(); ++i)
		{
			FVector TargetDirection = (ValidMoveLocations[i] - CurrentTileLocation).GetSafeNormal();			
			
			if (TargetDirection == -MoveDirection)	{continue;}
			
			const float DistanceToTargetLocation = (TargetMoveLocation - ValidMoveLocations[i]).Size();
			
			if (DistanceToTargetLocation < ShortestDistance)
				{
					ShortestDistance = DistanceToTargetLocation;
					BestIndex = i;
				}
		}		
		return (ValidMoveLocations[BestIndex] - CurrentTileLocation).GetSafeNormal();
	}
	
	return FVector::ZeroVector;
}

