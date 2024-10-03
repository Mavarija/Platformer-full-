// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Main.h"
#include "TriggerField.h"
#include "Components/StaticMeshComponent.h"
#include "Switchable.generated.h"

/**
 * 
 */

UCLASS()
class FIRSTPROJECT_API ASwitchable : public ATriggerField
{
	GENERATED_BODY()
	
public:

	ASwitchable();

	// Switch for the character to toggle
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Switchable")
	class UStaticMeshComponent* SwitchOnMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Switchable")
	class UStaticMeshComponent* SwitchOffMesh;

	// Enum for collectibles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Switchable")
	ECollectableType CollectableType;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// For specific events when trigger happens
	virtual void OnTrigger(AActor* OtherActor) override;
	virtual void OffTrigger() override;
};
