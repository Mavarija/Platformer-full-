// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Triggerable.h"
#include "GenericTriggered.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API AGenericTriggered : public ATriggerable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGenericTriggered();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void TriggerActor() override;
	virtual void DetriggerActor() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "GenericTrigger")
	void OnGenericTriggeredBP();

	UFUNCTION(BlueprintImplementableEvent, Category = "GenericTrigger")
	void OnGenericDetriggeredBP();

};
