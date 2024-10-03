// Fill out your copyright notice in the Description page of Project Settings.

#include "FloorSwitch.h"



// Sets default values
AFloorSwitch::AFloorSwitch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FloorSwitch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorSwitch"));
	FloorSwitch->SetupAttachment(GetRootComponent());

	bCharacterOnSwitch	= false;

}

// Called when the game starts or when spawned
void AFloorSwitch::BeginPlay()
{
	Super::BeginPlay();

	InitialSwitchLocation = FloorSwitch->GetComponentLocation();
	
}

// Called every frame
void AFloorSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFloorSwitch::UpdateFloorSwitchLocation(float Z)
{
	FVector NewLocation = InitialSwitchLocation;
	NewLocation.Z += Z;
	FloorSwitch->SetWorldLocation(NewLocation);
}

void AFloorSwitch::OnTrigger(AActor* OtherActor)
{
	Super::OnTrigger(OtherActor);

	if (!bCharacterOnSwitch) bCharacterOnSwitch = true;

	LowerFloorSwitch();
}
void AFloorSwitch::OffTrigger()
{
	Super::OffTrigger();

	if (bCharacterOnSwitch) bCharacterOnSwitch = false;

	RaiseFloorSwitch();

}