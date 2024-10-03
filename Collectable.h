// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Main.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Item.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/Actor.h"
#include "Collectable.generated.h"

UENUM(BlueprintType)
enum class ECollectableType :uint8
{
	ECT_YellowKey	UMETA(DisplayName = "YellowKey"),
	ECT_RedKey		UMETA(DisplayName = "RedKey"),
	ECT_BlackKey	UMETA(DisplayName = "BlackKey")
};

UCLASS()
class FIRSTPROJECT_API ACollectable : public AItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACollectable();

	// Types of items to use for trigger box
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectable")
	ECollectableType CollectableType;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
};
