// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TriggerField.h"
#include "GameFramework/Actor.h"
#include "VolumeTrigger.generated.h"

UCLASS()
class FIRSTPROJECT_API AVolumeTrigger : public ATriggerField
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVolumeTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// For specific events when trigger happens
	virtual void OnTrigger(AActor* OtherActor);
	virtual void OffTrigger();

	UFUNCTION(BlueprintImplementableEvent, Category = "Volume")
	void OnVolumeTriggeredBP();
	UFUNCTION(BlueprintImplementableEvent, Category = "Volume")
	void OnVolumeDetriggeredBP();

};
