// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeshSpawner.generated.h"

UCLASS()
class FIRSTPROJECT_API AMeshSpawner : public AActor
{
	GENERATED_BODY()
	
public:
/*
	// Scene component.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MeshSpawner")
*/

/*
	// Skeletal mesh component.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MeshSpawner")
	class USkeletalMeshComponent* SkeletalMesh;
*/

/*
	// Sphere collision component.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MeshSpawner")
	class USphereComponent* CollisionComponent;
*/


public:	
	// Sets default values for this actor's properties
	AMeshSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
