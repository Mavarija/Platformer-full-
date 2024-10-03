// Fill out your copyright notice in the Description page of Project Settings.


#include "Switchable.h"

// Sets default values
ASwitchable::ASwitchable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Switch Meshes attached to the root component
	SwitchOnMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwitchOnMesh"));
	SwitchOnMesh->SetupAttachment(GetRootComponent());

	SwitchOffMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwitchOffMesh"));
	SwitchOffMesh->SetupAttachment(GetRootComponent());


}

// Called when the game starts or when spawned
void ASwitchable::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASwitchable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Functionality for when a trigger begins
void ASwitchable::OnTrigger(AActor* OtherActor)
{
	Super::OnTrigger(OtherActor);
	SwitchOnMesh->SetVisibility(true);
	SwitchOffMesh->SetVisibility(false);
}
// Functionality for when a trigger ends
void ASwitchable::OffTrigger()
{
	Super::OffTrigger();
	SwitchOnMesh->SetVisibility(false);
	SwitchOffMesh->SetVisibility(true);
}