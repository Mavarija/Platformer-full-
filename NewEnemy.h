// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NewEnemy.generated.h"


// AI State Machine
UENUM(BlueprintType)
enum class EAIStateMachine :uint8
{		
	ASM_Idle			UMETA(DisplayName = "Idle"),			// peck the ground (once or twice)
	ASM_Roam			UMETA(DisplayName = "Roam"),			// move between the target points
	ASM_Chase			UMETA(DisplayName = "Chase"),			// run towards the player
	ASM_Attack			UMETA(DisplayName = "Attack"),			// peck the players feet/jump + sound
	ASM_Return			UMETA(DisplayName = "Return"),			// return to a previous/random target point
	ASM_Dead			UMETA(DisplayName = "Dead")				// fall over + stop moving + disappear
};


class UPawnSensingComponent;

UCLASS()
class FIRSTPROJECT_API ANewEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANewEnemy();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EAIStateMachine AIStateMachine;

			/* Sound */
	FTimerHandle SoundHandle;

	// idle sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Sounds")
	class USoundCue* IdleSound;
	UFUNCTION(BlueprintCallable)
	void EnemyIdleSound();

	// attack sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Sounds")
	USoundCue* SwingSound;
	void EnemySwingSound();

	// death sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Sounds")
	USoundCue* DeathSound;
	void EnemyDeathSound();

			/* Particles */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Particles")
	class UParticleSystem* HitParticles;
	void EnemyHitParticle();

			/* Animation */
	UFUNCTION(BlueprintImplementableEvent, Category = "Animation")
	void PlayEnemyAnimation();

	// Idle animation
	bool bIsWaitingForNewPartol = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

private:

			/* Components */
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;


			/* Combat */
	UPROPERTY(EditAnywhere, Category = "AI | Combat")
	float ChasingSpeed = 300.0f;

	UPROPERTY()
	class AActor* CombatTarget;
	UPROPERTY(EditAnywhere, Category = "AI | Combat")
	double CombatRadius = 500.0f;
	UPROPERTY(EditAnywhere, Category = "AI | Combat")
	double AttackRadius = 150.0f;
	void CheckCombatTarget();
	
	FTimerHandle RotationHandle;
	float delayBeforeReset = 10.0f;
	UFUNCTION(BlueprintCallable)
	void AttackStart();
	UFUNCTION(BlueprintCallable)
	void Landed(const FHitResult& _hit) override;

	FHitResult Hit;


			/* Navigation */
	UPROPERTY()
	class AAIController* EnemyController;

	// Current patrol target
	UPROPERTY(EditInstanceOnly, Category = "AI | Navigation")
	AActor* PatrolTarget;
	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.0f;

	// Array of potential target points (to be set in UE) for the enemy to navigate to
	UPROPERTY(EditInstanceOnly, Category = "AI | Navigation")
	TArray<AActor*> PatrolTargets;
	AActor* ChoosePatrolTarget();
	void CheckPatrolTarget();
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);

	// Patrol timer + wait times
	FTimerHandle PatrolTimer;
	UPROPERTY(EditAnywhere, Category = "AI | Navigation")
	float WaitMin = 0.5f;
	UPROPERTY(EditAnywhere, Category = "AI | Navigation")
	float WaitMax = 5.0f;
	void PatrolTimerFinished();
	UPROPERTY(EditAnywhere, Category = "AI | Navigation")
	float jumpForce = 300.0f;
	bool bIsRotated = false;


			/* State Machine */
	EAIStateMachine AIState = EAIStateMachine::ASM_Roam;
	
};
