// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorFrame.h"

// Sets default values
ADoorFrame::ADoorFrame()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMesh->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void ADoorFrame::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ADoorFrame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADoorFrame::OnTrigger(AActor* OtherActor)
{
	Super::OnTrigger(OtherActor);

}
void ADoorFrame::OffTrigger()
{
	Super::OffTrigger();

}
