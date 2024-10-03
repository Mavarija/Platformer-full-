// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Triggerable.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

UENUM(BlueprintType)
enum class EDoorDirection :uint8
{
	EDD_ZPos		UMETA(DisplayName = "ZPos"),
	EDD_ZNeg		UMETA(DisplayName = "ZNeg"),
	EDD_XPos		UMETA(DisplayName = "XPos"),
	EDD_XNeg		UMETA(DisplayName = "XNeg"),
	EDD_YPos		UMETA(DisplayName = "YPos"),
	EDD_YNeg		UMETA(DisplayName = "YNeg")
};

UENUM(BlueprintType)
enum class EDoorRotation :uint8
{
	EDR_Pitch	UMETA(DisplayName = "Pitch"),
	EDR_Yaw		UMETA(DisplayName = "Yaw"),
	EDR_Roll	UMETA(DisplayName = "Roll")
};

UENUM(BlueprintType)
enum class EDoorAnimationType :uint8
{
	EDAT_SlideEaseInOut		UMETA(DisplayName = "SlideEaseInOut"),
	EDAT_RotateEaseInOut	UMETA(DisplayName = "RotateEaseInOut"),
	EDAT_SlideSaloon		UMETA(DisplayName = "SlideSaloon"),
	EDAT_RotateLinear		UMETA(DisplayName = "RotateLinear")
};

UCLASS()
class FIRSTPROJECT_API ADoor : public ATriggerable
{
	GENERATED_BODY()
	
public:	
	
	// Sets default values for this actor's properties
	ADoor();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Door")
	class USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Door")
	class USceneComponent* DoorHinge;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Door" )
	UStaticMeshComponent* Door;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	class USoundCue* DoorOpenSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	EDoorDirection DoorDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	EDoorRotation DoorRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	EDoorAnimationType DoorAnimationType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float SlideDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float RotationDegree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float TimeInSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	bool bDoorStop;


	// Initial location for the door 
	UPROPERTY(BlueprintReadWrite, Category = "Door")
	FVector InitialDoorLocation;

	UPROPERTY(BlueprintReadWrite, Category = "Door")
	FRotator InitialDoorRotation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void TriggerActor() override;
	virtual void DetriggerActor() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Door")
	void OnDoorTriggeredBP();

	UFUNCTION(BlueprintImplementableEvent, Category = "Door")
	void OnDoorDetriggeredBP();

	UFUNCTION(BlueprintCallable, Category = "Door")
	void UpdateDoorLocation(float Direction, float Rotation);
};
