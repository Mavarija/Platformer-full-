// Fill out your copyright notice in the Description page of Project Settings.


#include "GenericTriggered.h"


// Sets default values
AGenericTriggered::AGenericTriggered()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGenericTriggered::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AGenericTriggered::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGenericTriggered::TriggerActor()
{
	if (bHasTriggered)
		return;

	Super::TriggerActor();

	OnGenericTriggeredBP();
}

void AGenericTriggered::DetriggerActor()
{
	Super::DetriggerActor();

	OnGenericDetriggeredBP();
}

