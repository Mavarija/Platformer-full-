// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PointLightComponent.h"
#include "PulsingLight.generated.h"

UCLASS()
class FIRSTPROJECT_API APulsingLight : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APulsingLight();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="PulsingLight")
	TObjectPtr<UPointLightComponent> PointLight;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="PulsingLight")
	TObjectPtr<USceneComponent> DefaultSceneRoot;
		
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="PulsingLight")
	double PulseSpeedInSeconds;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="PulsingLight")
	double IntensityMin;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="PulsingLight")
	double IntensityMax;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="PulsingLight")
	int32 InternalLoopCount;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="PulsingLight")
	int32 LoopCount;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="PulsingLight")
	FLinearColor StartColour;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="PulsingLight")
	FLinearColor EndColour;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
