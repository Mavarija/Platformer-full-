// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshSpawner.h"

// Sets default values
AMeshSpawner::AMeshSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

/*
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
*/

}

// Called when the game starts or when spawned
void AMeshSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMeshSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

