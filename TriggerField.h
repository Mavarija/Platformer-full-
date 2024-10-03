// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Main.h"
#include "Collectable.h"
#include "Triggerable.h"
#include "Components/BoxComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TriggerField.generated.h"

UENUM(BlueprintType)
enum class EOverlapTriggerTypes :uint8
{	
	ETT_Enter			UMETA(DisplayName = "Enter"),			// Activates once, when actor enters trigger box
	ETT_EnterExit		UMETA(DisplayName = "EnterExit"),		// Activates when actor enters trigger box - deactivates when actor leaves trigger box
	ETT_Overlap			UMETA(DisplayName = "Overlap"),			// Activates when actor enters trigger box - stays active (can add other behaviors)
	ETT_OverlapExit		UMETA(DisplayName = "OverlapExit"),		// Activates when actor enters trigger box - deactivates when actor leaves trigger box - after a set amount of time
	ETT_Button			UMETA(DisplayName = "Button"),			// Activates once, when actor presses a button inside the trigger box - deactivates when actor leaves trigger box - after set amount of time
	ETT_ButtonToggle	UMETA(DisplayName = "ButtonToggle"),	// Activates once, when actor presses a button inside the trigger box - deactivates per button press
	ETT_Object			UMETA(DisplayName = "Object"),			// Activates when actor has an object equipped and is inside the trigger box - deactivates when actor/object leaves trigger box - after set amount of time

	ETT_PainBound		UMETA(DisplayName = "PainBound"),		// Sap health for as long as the player is within bound
	ETT_DeathBound		UMETA(DisplayName = "DeathBound"),		// Kill player on entry
	ETT_KnockBack		UMETA(DisplayName = "KnockBack")		// Knock the player back and out of bound + sap health
};

UCLASS()
class FIRSTPROJECT_API ATriggerField : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATriggerField();

	// Scene root component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TriggerField")
	class USceneComponent* SceneComponent;
	// Overlap volume for functionality to be triggered
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TriggerField")
	class UBoxComponent* TriggerBox;

	// List of Triggerable actors to trigger when the switch is activated
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TriggerField")
	TArray<ATriggerable*>ActorsToTrigger;

	// Types of triggers
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TriggerField")
	EOverlapTriggerTypes TriggerType;
	// Types of items to use for trigger box
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectable")
	ECollectableType ObjectType;

	// Pointer to character for overlap functionality
	class AMain* CharacterOverlap;
	// Pointer to actor for trigger functionality
	class AActor* TriggeredActor;

	// Variable to set it happen only once (e.g.: doors stay open)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TriggerField")
	bool bDontUntrigger;
	// Variable for trigger field to activate only once per game
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TriggerField")
	bool bOneShot;
	// Variable for how long the switch lasts
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TriggerField")
	float SwitchTime;
	// Variable to check if the trigger has been activated
	UPROPERTY(BlueprintReadWrite, Category = "TriggerField")
	bool bHasTriggered;
	// Variable to check if the trigger has been deactivated
	bool bHasUnTriggered;
	// Variable to allow the functionality to trigger
	bool bCanTrigger;
	// Timer to call the given native function at a set interval
	FTimerHandle SwitchHandle;

	UPROPERTY(BlueprintReadWrite, Category = "TriggerField")
	bool bDisableTrigger;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Trigger / Detrigger actor
	UFUNCTION()
	virtual void TriggerActor();
	UFUNCTION()
	virtual void DetriggerActor();

	// For specific events when trigger happens
	UFUNCTION()
	virtual void OnTrigger(AActor* OtherActor);
	UFUNCTION()
	virtual void OffTrigger();

	// Overlap begin/end functionality
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};