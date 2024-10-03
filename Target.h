// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundCue.h"
#include "Triggerable.h"
#include "Target.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API ATarget : public ATriggerable
{
	GENERATED_BODY()
	
public:
	ATarget();

	/** Overlap volume for functionality to be triggered */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Target")
	class UBoxComponent* TriggerBox;

	/** Switch for the character to step on */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Target")
	class UStaticMeshComponent* TargetMesh;

	FTimerHandle SwitchHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
	bool bOneShot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
	float SwitchTime;
	bool bHasTriggered;

	void Detrigger();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target | Sound")
	class USoundCue* OnHitSound;

	// Initial location for the target
	UPROPERTY(BlueprintReadWrite, Category = "Target")
	FVector InitialTargetLocation;

	UPROPERTY(BlueprintReadWrite, Category = "Target")
	FRotator InitialTargetRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float TimeInSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
	float RotationDegree;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void TriggerActor() override;
	virtual void DetriggerActor() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Target")
	void OnTargetTriggeredBP();

	UFUNCTION(BlueprintImplementableEvent, Category = "Target")
	void OnTargetDetriggeredBP();

	UFUNCTION(BlueprintCallable, Category = "Target")
	void UpdateTargetLocation(float Rotation);

/*
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);*/
};