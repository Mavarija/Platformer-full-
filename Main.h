// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WeaponContainer.h"
#include "Projectile.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

// How far to trace a ray from the camera into the world to find a hit point under the crosshairs.
#define BULLET_TRACE_DISTANCE 5000.0

// Uncomment to draw debugging lines showing weapon projectile traces
//#define SHOW_DEBUG_WEAPON_TRACELINES

USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	float Health;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	float MaxHealth;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	float Stamina;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	float MaxStamina;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	FVector Location;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	FRotator Rotation;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	int32 Coins;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	FString WeaponName;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	bool bWeaponParticles;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	FString LevelName;

};

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Dead UMETA(DisplayName = "Dead"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EStaminaStatus :uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),

	ESS_MAX UMETA(DisplayName = "DefaultMax")

};

UENUM(BlueprintType)
enum class EWeaponType :uint8
{
	EWT_NoWeapon	UMETA(DisplayName = "No Weapon"),
	EWT_Sword		UMETA(DisplayName = "Sword"),
	EWT_Gun			UMETA(DisplayName = "Gun")
};

UCLASS()
class FIRSTPROJECT_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();



	int IsKeyPressed;
	int KeyPressCount;

	void KeyPressedUp();
	void KeyPressedDown();

	FCharacterStats CharacterStats;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHasCombatTarget;

	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; }

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Combat")
	FVector CombatTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class AMainPlayerController* MainPlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* HitSound;

	TArray<FVector> PickupLocations;
	
	UFUNCTION(BlueprintCallable)
	void ShowPickupLocations();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;

	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina;

	float InterpSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bInterpToEnemy;
	void SetInterpToEnemy(bool Interp);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AEnemy* CombatTarget;

	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }

	FRotator GetLookAtRotationYaw(FVector Target);

	/** Set movement status and running speed */
	void SetMovementStatus(EMovementStatus Status);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintingSpeed;

	bool bShiftKeyDown;

	/** Pressed down to enable sprinting */
	void ShiftKeyDown();

	/** Released to stop sprinting */
	void ShiftKeyUp();

	/** Camera boom positioning the camera behind the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow Camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Base turn rates to scale turning functions for the camera */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	float CameraZoomInSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	float CameraZoomOutSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float MaxPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float MinPitch;

	/**
	/*
	/* Player Stats
	/*
	*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 Coins;

	void DecrementHealth(float Amount);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void IncrementCoins(int32 Amount);

	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float Amount);

	void Die();

	virtual void Jump() override;

	// Gun muzzle offset from the camera location.
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	FVector MuzzleOffset;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// HUD
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void HUDEnabledBP();

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void HUDDisabledBP();


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for forwards/backwards input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	bool bMovingForward;
	bool bMovingRight;



	// Camera zoom smoothening functionality
//	template <typename T>

	float SmoothDamper(const float& from, const float& to, float& vel, float SmoothTime, float DeltaTime);
	void SetCameraZoomTarget(float ZoomTarget, float ZoomSpeed);

	float CameraZoomVelocity;
	float CameraZoomTarget;
	float CameraZoomSpeed;



	/** Called via input to turn at a given rate
	* @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/** Called via input to look up/down at a given rate
	* @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired look up/down rate
	*/
	void LookUpAtRate(float Rate);

	/** Called for mouse input add yaw rotation */
	void Turn(float Value);

	/** Called for mouse input add pitch rotation */
	void LookUp(float Value);

	bool bLMBDown;
	void EquipWeapon();
	void LMBUp();

	bool bRMBDown;
	void AimDown();
	void AimUp();

	bool bESCDown;
	void ESCDown();
	void ESCUp();;

	bool bIsGunEquipped;

	void Fire( float rate );

	bool AllowInput(float Value);

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Items)
	class AWeapon* EquippedWeapon;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Items)
	class ACollectable* Collectable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
	class AItem* ActiveOverlappingItem;

	void SetEquippedWeapon(AWeapon* WeaponToSet);

	UFUNCTION(BlueprintCallable)
	AWeapon* GetEquippedWeapon() { return EquippedWeapon; }

	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* GunCombatMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* SwordCombatMontage;

	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	void UpdateCombatTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AEnemy> EnemyFilter;

	void SwitchLevel(FName LevelName);

	UFUNCTION(BlueprintCallable)
	void SaveGame();

	UFUNCTION(BlueprintCallable)
	void LoadGame(bool LoadPosition);

	UFUNCTION(BlueprintCallable)
	void LoadGameNoSwitch();

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	//TSubclassOf<UWeaponContainer> WeaponContainer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<class AWeaponContainerActor> WeaponContainer;
};
