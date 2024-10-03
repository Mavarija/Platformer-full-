// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	DoorHinge = CreateDefaultSubobject<USceneComponent>(TEXT("DoorHinge"));
	DoorHinge->AttachToComponent(SceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->AttachToComponent(DoorHinge, FAttachmentTransformRules::KeepRelativeTransform);

	DoorDirection = EDoorDirection::EDD_ZPos;
	DoorRotation = EDoorRotation::EDR_Pitch;
	DoorAnimationType = EDoorAnimationType::EDAT_SlideEaseInOut;

	SlideDistance = 100.0f;
	RotationDegree = 100.0f;

	TimeInSeconds = 1.0f;

	bDoorStop = false;
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
	InitialDoorLocation = DoorHinge->GetComponentLocation();
	InitialDoorRotation = DoorHinge->GetComponentRotation();
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADoor::TriggerActor()
{
	if (bHasTriggered)
		return;

	Super::TriggerActor();

	if (DoorOpenSound)
	{
		UGameplayStatics::PlaySound2D(this, DoorOpenSound);
	}

	OnDoorTriggeredBP();
}

void ADoor::DetriggerActor()
{
	if (bDoorStop)
	{
		return;
	}

	Super::DetriggerActor();

	if (DoorOpenSound)
	{
		UGameplayStatics::PlaySound2D(this, DoorOpenSound);
	}

	OnDoorDetriggeredBP();
}

void ADoor::UpdateDoorLocation(float Direction, float Rotation) 
{
	FVector NewLocation = InitialDoorLocation;
	FRotator NewRotation = InitialDoorRotation;

	Direction *= SlideDistance;
	Rotation *= RotationDegree;

	switch (DoorDirection)
	{
		case EDoorDirection::EDD_ZPos:
			NewLocation.Z += Direction;
			break;
		case EDoorDirection::EDD_ZNeg:
			NewLocation.Z -= Direction;
			break;
		case EDoorDirection::EDD_XPos:
			NewLocation.X += Direction;
			break;
		case EDoorDirection::EDD_XNeg:
			NewLocation.X -= Direction;
			break;
		case EDoorDirection::EDD_YPos:
			NewLocation.Y += Direction;
			break;
		case EDoorDirection::EDD_YNeg:
			NewLocation.Y -= Direction;
			break;
		default:
			break;
	}

	switch (DoorRotation)
	{
		case EDoorRotation::EDR_Pitch:
			NewRotation.Pitch += Rotation;
			break;
		case EDoorRotation::EDR_Roll:
			NewRotation.Roll += Rotation;
			break;
		case EDoorRotation::EDR_Yaw:
			NewRotation.Yaw += Rotation;
			break;
		default:
			break;
	}

	DoorHinge->SetWorldLocation(NewLocation);
	DoorHinge->SetWorldRotation(NewRotation);
}
