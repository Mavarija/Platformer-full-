// Fill out your copyright notice in the Description page of Project Settings.


#include "PulsingLight.h"

// Sets default values
APulsingLight::APulsingLight()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(DefaultSceneRoot);

	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->AttachToComponent(DefaultSceneRoot, FAttachmentTransformRules::KeepRelativeTransform);

}

// Called when the game starts or when spawned
void APulsingLight::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APulsingLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

