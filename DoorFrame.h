// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TriggerField.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "DoorFrame.generated.h"


UCLASS()
class FIRSTPROJECT_API ADoorFrame : public ATriggerField
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ADoorFrame();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Frame")
	class UStaticMeshComponent* StaticMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnTrigger(AActor* OtherActor) override;
	virtual void OffTrigger() override;

};
