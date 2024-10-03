// Fill out your copyright notice in the Description page of Project Settings.


#include "NewEnemy.h"
#include "AIController.h"
#include "Sound/SoundCue.h"
#include "Perception/PawnSensingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Main.h"


// Sets default values
ANewEnemy::ANewEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

/*
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
*/

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);

}

// Called when the game starts or when spawned
void ANewEnemy::BeginPlay()
{
	Super::BeginPlay();

	// roaming between path points
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);

	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &ANewEnemy::PawnSeen);
	}

	// cluck sound at random intervals
	GetWorldTimerManager().SetTimer(SoundHandle, this, &ANewEnemy::EnemyIdleSound, FMath::RandRange(2.0f, 5.0f));

}


// Called every frame
void ANewEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AIState > EAIStateMachine::ASM_Roam)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}


/*
	// AI State Machine					FOR REWORK WITH PAULIE
	switch (AIStateMachine)
	{
	case EAIStateMachine::ASM_Idle:

		break;
	case EAIStateMachine::ASM_Roam:

		break;
	case EAIStateMachine::ASM_Chase:

		break;
	case EAIStateMachine::ASM_Attack:

		break;
	case EAIStateMachine::ASM_Return:

		break;
	case EAIStateMachine::ASM_Dead:

		break;
	default:
		;
	}
*/	

}


// Called to bind functionality to input
void ANewEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANewEnemy::CheckCombatTarget()
{
	if (!InTargetRange(CombatTarget, CombatRadius))
	{
		// clears combat target when we are out of range
		CombatTarget = nullptr;
		AIState = EAIStateMachine::ASM_Roam;
		GetCharacterMovement()->MaxWalkSpeed = 200.0f;
		MoveToTarget(PatrolTarget);

		UE_LOG(LogTemp, Warning, TEXT("Lose Interest."));
	}
	else if (!InTargetRange(CombatTarget, AttackRadius) && AIState != EAIStateMachine::ASM_Chase)
	{
		// Outside attack range, chase character
		AIState = EAIStateMachine::ASM_Chase;
		GetCharacterMovement()->MaxWalkSpeed = 300.0f;
		MoveToTarget(CombatTarget);

		UE_LOG(LogTemp, Warning, TEXT("Chasing Player."));
	}
	else if (InTargetRange(CombatTarget, AttackRadius) && AIState != EAIStateMachine::ASM_Attack)
	{
		// Inside attack range, attack character
		AIState = EAIStateMachine::ASM_Attack;

		AttackStart();
		
		UE_LOG(LogTemp, Warning, TEXT("Attack!"));
	}
}

void ANewEnemy::PawnSeen(APawn* SeenPawn)
{
	if (AIState == EAIStateMachine::ASM_Chase) return;
	if (SeenPawn->ActorHasTag(FName("MainCharacter"))) // main = seen pawn
	{
		GetWorldTimerManager().ClearTimer(PatrolTimer);	// clears timer for wait time on a patrol point
		GetCharacterMovement()->MaxWalkSpeed = 300.0f;
		CombatTarget = SeenPawn;

		if (AIState != EAIStateMachine::ASM_Attack)
		{
			AIState = EAIStateMachine::ASM_Chase;
			MoveToTarget(CombatTarget);

			UE_LOG(LogTemp, Warning, TEXT("Pawn Seen, Chase Player"));
		}
	}
}

void ANewEnemy::AttackStart()
{
	// randomly jumps up + makes a sound + attack
	if (FMath::RandBool()) // RandBool 50/50 chance to do something
	{
		if (GetCharacterMovement()->IsMovingOnGround())
		{
			FVector jumpImpulse = FVector(0.0f, 0.0f, 1.0f) * jumpForce;
			GetCharacterMovement()->AddImpulse(jumpImpulse, true);
			EnemySwingSound();
		}
		// rotates character to "attack" target
		if (!bIsRotated)
		{
			FRotator currentRotation = GetActorRotation();
			FRotator newRotation = FRotator(-30.0f, currentRotation.Yaw, currentRotation.Roll);
			SetActorRotation(newRotation);
			bIsRotated = true;

		}
	}
}
void ANewEnemy::Landed(const FHitResult& _hit)
{
	Super::Landed(Hit);
	if (bIsRotated)
	{
		FRotator currentRotation = GetActorRotation();
		FRotator newRotation = FRotator(0.0f, currentRotation.Yaw, currentRotation.Roll);
		SetActorRotation(newRotation);
		bIsRotated = false;
	}
}

void ANewEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(WaitMin, WaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &ANewEnemy::PatrolTimerFinished, WaitTime);
		bIsWaitingForNewPartol = true;
	}
	if (bIsWaitingForNewPartol)
	{
		if (FMath::RandBool())
		{
			PlayEnemyAnimation();
		}
	}
}

AActor* ANewEnemy::ChoosePatrolTarget()
{
	// Array to make sure we do not select the already selected target point
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}
	// Selecting a target point
	const int32 NumPatrolTargets = ValidTargets.Num();
	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[TargetSelection];
	}
	return nullptr;
}

void ANewEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(15.0f);
	EnemyController->MoveTo(MoveRequest);
}

void ANewEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
	bIsWaitingForNewPartol = false;
}

void ANewEnemy::EnemyHitParticle()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, GetActorLocation(), FRotator(0.f), true);
}

void ANewEnemy::EnemyIdleSound()
{
	UGameplayStatics::PlaySoundAtLocation(this, IdleSound, this->GetActorLocation(), this->GetActorRotation());

	GetWorldTimerManager().SetTimer(SoundHandle, this, &ANewEnemy::EnemyIdleSound, FMath::RandRange(2.0f, 5.0f));
}

void ANewEnemy::EnemySwingSound()
{
	UGameplayStatics::PlaySoundAtLocation(this, SwingSound, this->GetActorLocation(), this->GetActorRotation());
}

void ANewEnemy::EnemyDeathSound()
{
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, this->GetActorLocation(), this->GetActorRotation());
}

bool ANewEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();

	return DistanceToTarget <= Radius;
}
