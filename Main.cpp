// Fill out your copyright notice in the Description page of Project Settings.

#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"
#include "MainPlayerController.h"
#include "FirstSaveGame.h"
#include "Critter.h"
#include "WeaponContainerActor.h"
#include <DrawDebugHelpers.h>

#include "Engine/SkeletalMeshSocket.h"

// Sets default values
AMain::AMain()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	IsKeyPressed = 0;
	KeyPressCount = 0;

	// Create Camera Boom (pulls towards the player if there's a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 350.0f; // Camera follows at this distance
	CameraBoom->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	CameraBoom->bUsePawnControlRotation = true; // Rotate arm based on controller

	// Set size for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(48.0f, 105.0f);

	// Create Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match
	// the controller orientation
	FollowCamera->bUsePawnControlRotation = false;
	// Set our turn rates for input
	BaseTurnRate = 65.0f;
	BaseLookUpRate = 65.0f;

	CameraZoomInSpeed = 1.0f;
	CameraZoomOutSpeed = 0.5f;
	CameraZoomSpeed = CameraZoomInSpeed;

	CameraZoomTarget = CameraBoom->TargetArmLength;

	Collectable = nullptr;

	// Don't rotate when the controller rotates.
	// Let that just affect the camera.
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 650.0f;
	GetCharacterMovement()->AirControl = 0.2f;

	MaxHealth = 100.0f;
	Health = 65.0f;
	MaxStamina = 150.0f;
	Stamina = 120.0f;
	Coins = 0;

	RunningSpeed = 650.0f;
	SprintingSpeed = 950.0f;

	bShiftKeyDown = false;
	bLMBDown = false;
	bRMBDown = false;

	bIsGunEquipped = false;

	//Initialize Enums
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	StaminaDrainRate = 25.0f;
	MinSprintStamina = 50.0f;

	InterpSpeed = 15.f;
	bInterpToEnemy = false;

	bHasCombatTarget = false;

	bMovingForward = false;
	bMovingRight = false;

	bESCDown = false;

	MinPitch = -45.0f;
	MaxPitch = 45.0f;

}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();
	
	// Tags
	Tags.Add(FName("MainCharacter"));

	MainPlayerController = Cast<AMainPlayerController>(GetController());

	//LoadGameNoSwitch();
	if (MainPlayerController)
	{
		MainPlayerController->GameModeOnly();

		// Limit the chase camera's pitch
		MainPlayerController->PlayerCameraManager->ViewPitchMax = MaxPitch;
		MainPlayerController->PlayerCameraManager->ViewPitchMin = MinPitch;
	}
}

float AMain::SmoothDamper(const float& from, const float& to, float& vel, float SmoothTime, float DeltaTime)
{
	{
		float omega = (2.0f / SmoothTime);
		float x = (omega * DeltaTime);
		float exp = 1.0f / (1.0f + x + (0.48f * x * x) + (0.235f * x * x * x));
		float change = from - to;
		float temp = (vel + omega * change) * DeltaTime;
		vel = (vel - omega * temp) * exp;
		return to + (change + temp) * exp;
	}
}

void AMain::SetCameraZoomTarget(float ZoomTarget, float ZoomSpeed)
{
	CameraZoomTarget = ZoomTarget;
	CameraZoomVelocity = 0.0f;
	CameraZoomSpeed = ZoomSpeed;
}


// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UE_LOG(LogTemp, Warning, TEXT("%f"), CameraZoomSpeed);

	// Camera smooth dampening
	CameraBoom->TargetArmLength = SmoothDamper(CameraBoom->TargetArmLength, CameraZoomTarget, CameraZoomVelocity, CameraZoomSpeed, DeltaTime);

	UE_LOG(LogTemp, Warning, TEXT("Vel %f"), CameraZoomVelocity)

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	float DeltaStamina = StaminaDrainRate * DeltaTime;

	switch (StaminaStatus)
	{
		case EStaminaStatus::ESS_Normal:
			if (bShiftKeyDown)
			{
				if (Stamina - DeltaStamina <= MinSprintStamina)
				{
					SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
					Stamina -= DeltaStamina;
				}
				else
				{
					Stamina -= DeltaStamina;
				}
				if (bMovingForward || bMovingRight)
				{
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
				}
				else
				{
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
			}
			else // Shift key up
			{
				if (Stamina + DeltaStamina >= MaxStamina)
				{
					Stamina = MaxStamina;
				}
				else
				{
					Stamina += DeltaStamina;
				}
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			break;

		case EStaminaStatus::ESS_BelowMinimum:
			if (bShiftKeyDown)
			{
				if (Stamina - DeltaStamina <= 0.0f)
				{
					SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
					Stamina = 0;
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
				else
				{
					Stamina -= DeltaStamina;
					if (bMovingForward || bMovingRight)
					{
						SetMovementStatus(EMovementStatus::EMS_Sprinting);
					}
					else
					{
						SetMovementStatus(EMovementStatus::EMS_Normal);
					}
				}
			}
			else // Shift key up
			{
				if (Stamina + DeltaStamina >= MinSprintStamina)
				{
					SetStaminaStatus(EStaminaStatus::ESS_Normal);
					Stamina += DeltaStamina;
				}
				else
				{
					Stamina += DeltaStamina;
				}
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			break;

		case EStaminaStatus::ESS_Exhausted:
			if (bShiftKeyDown)
			{
				Stamina = 0.0f;
			}
			else // Shift key up
			{
				SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
			break;

		case EStaminaStatus::ESS_ExhaustedRecovering:
			if (Stamina + DeltaStamina >= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Stamina += DeltaStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
			break;

		default:
			;
	}

	if (bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}

	if (CombatTarget)
	{
		CombatTargetLocation = CombatTarget->GetActorLocation();
		if (MainPlayerController)
		{
			MainPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}

	// Follow camera when aiming
	if (bIsGunEquipped)
	{
		FRotator rotation = FollowCamera->GetComponentRotation();
		SetActorRotation(rotation);

	}
}

FRotator AMain::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.0f, LookAtRotation.Yaw, 0.0f);
	return LookAtRotationYaw;
}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMain::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMain::ESCDown);
	PlayerInputComponent->BindAction("ESC", IE_Released, this, &AMain::ESCUp);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::ShiftKeyUp);
	
	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMain::EquipWeapon); // This is actually a 'T' key now instead of the LMB
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMain::LMBUp);

	PlayerInputComponent->BindAction("Key", IE_Released, this, &AMain::KeyPressedUp);
	PlayerInputComponent->BindAction("Key", IE_Pressed, this, &AMain::KeyPressedDown);

	PlayerInputComponent->BindAction("RMB", IE_Pressed, this, &AMain::AimDown);
	PlayerInputComponent->BindAction("RMB", IE_Released, this, &AMain::AimUp);

	PlayerInputComponent->BindAxis("Fire", this, &AMain::Fire);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AMain::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMain::LookUp);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpAtRate);

}

void AMain::KeyPressedDown()
{
	IsKeyPressed = 1;
}

void AMain::KeyPressedUp()
{
	IsKeyPressed = 0;
}


bool AMain::AllowInput(float Value)
{
	if ( EquippedWeapon )
	{
		if (EquippedWeapon->bIsProjectile)
		{
			return true;
		}
	}

	bool allow = (Controller != nullptr) &&
		(Value != 0.0f) && 
		(!bAttacking) &&
		(MovementStatus != EMovementStatus::EMS_Dead) &&
		!MainPlayerController->bPauseMenuOpen;

	return(allow);
}

void AMain::Turn(float Value)
{
	if (AllowInput(Value))
	{
		AddControllerYawInput(Value);
	}
}

void AMain::LookUp(float Value)
{
	if (AllowInput(Value))
	{
		AddControllerPitchInput(Value);
	}
}

void AMain::MoveForward(float Value)
{
	bMovingForward = false;
	if (AllowInput(Value)/* && !GetCharacterMovement()->IsFalling()*/ )
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);

		bMovingForward = true;
	}
}

void AMain::MoveRight(float Value)
{
	bMovingRight = false;
	if (AllowInput(Value) /*&& !GetCharacterMovement()->IsFalling()*/)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);

		bMovingRight = true;
	}
}

void AMain::TurnAtRate(float Rate)
{
	if (AllowInput(Rate))
	{
		float a = Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds();
		float b = FMath::Clamp(a, -1.0f, 1.0f);
		AddControllerYawInput(Rate);
	}
}

void AMain::LookUpAtRate(float Rate)
{
	if (AllowInput(Rate))
	{
		float a = Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds();
		float b = FMath::Clamp(a, -0.5f, 0.5f);
		//AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
		AddControllerPitchInput(b);
	}
}


void AMain::EquipWeapon() // Equip weapon
{
	bLMBDown = true;
	bIsGunEquipped = false;

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	if (ActiveOverlappingItem)
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon)
		{
			Weapon->Equip(this);
			SetActiveOverlappingItem(nullptr);

			if (Weapon->bIsProjectile)
			{
				bIsGunEquipped = true;

				// CameraBoom->TargetArmLength = 300.0f;
				SetCameraZoomTarget(200.0f, CameraZoomInSpeed);
				CameraBoom->SetRelativeLocation(FVector(0.0f, 50.0f, 50.0f));

				HUDEnabledBP();
			}
			else
			{
				bIsGunEquipped = false;
				// CameraBoom->TargetArmLength = 600.0f;
				SetCameraZoomTarget(350.0f, CameraZoomOutSpeed);

				HUDDisabledBP();
			}
		}
	}
}

void AMain::LMBUp()
{
	bLMBDown = false;
}


void AMain::AimDown()
{
	if (EquippedWeapon)
	{
		if (EquippedWeapon->bIsProjectile)
		{
			bRMBDown = true;

			// CameraBoom->TargetArmLength = 0.0f;
			SetCameraZoomTarget(0.0f, CameraZoomInSpeed);
			CameraBoom->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		}
	}
}
void AMain::AimUp()
{	
	if (EquippedWeapon)
	{
		if (EquippedWeapon->bIsProjectile)
		{
			bRMBDown = false;

			// CameraBoom->TargetArmLength = 300.0f;
			SetCameraZoomTarget(200.0f, CameraZoomOutSpeed);
			CameraBoom->SetRelativeLocation(FVector(0.0f, 50.0f, 50.0f));
		}
	}
}


void AMain::ESCDown()
{
	bESCDown = true;

	if (MainPlayerController)
	{
		MainPlayerController->TogglePauseMenu();
	}
}

void AMain::Fire( float rate )
{
	// Attempt to fire the projectile.
	if (EquippedWeapon)
	{
		if (rate < 0.75f) // how far the button has been pressed.
		{
			return;
		}
		if (EquippedWeapon->ProjectileClass && EquippedWeapon->bIsProjectile)
		{
			if (EquippedWeapon->ReloadCount)
			{
				return;
			}

			EquippedWeapon->ReloadCount = EquippedWeapon->ReloadTicks;

			FTransform MuzzleTransform = EquippedWeapon->SkeletalMeshRight->GetSocketTransform("MuzzleSocket");
			MuzzleTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

			// To make sure we have a World to work with. Sometimes there might be a load error - can't load the world. UE thing.
			UWorld* World = GetWorld();
			if (World)
			{
				FCollisionQueryParams CollisionParams;
				FHitResult OutHit2;

				// avoid colliding with the collision volume(s) around the weapon we're firing from!
				CollisionParams.bTraceComplex = true;
				CollisionParams.AddIgnoredActor(EquippedWeapon);

				// Fire a ray from the camera's position in it's forward direction to find an impact point straight through the centered crosshairs
				bool isHit = World->LineTraceSingleByChannel(OutHit2, FollowCamera->GetComponentLocation(), FollowCamera->GetComponentLocation() + (FollowCamera->GetComponentRotation().Vector() * BULLET_TRACE_DISTANCE), ECC_GameTraceChannel1, CollisionParams);

				FVector HitLocation;
				if (isHit)
				{
					HitLocation = OutHit2.ImpactPoint;
				}
				else
				{
					// If we didn't hit anything just use the end point of the ray we fired.
					HitLocation = OutHit2.TraceEnd;
				}

#ifdef SHOW_DEBUG_WEAPON_TRACELINES
				DrawDebugLine(GetWorld(), FollowCamera->GetComponentLocation(), HitLocation, FColor::Blue, false, 20.0f, 0, 0.6f);
#endif
				// Find the rotation from the gun muzzle to the impact point in the crosshairs.
				FRotator RotDirection = UKismetMathLibrary::FindLookAtRotation(MuzzleTransform.GetLocation(), HitLocation);

//				bool isHit = GetWorld()->LineTraceSingleByChannel(OutHit, MuzzleTransform.GetLocation(), MuzzleTransform.GetLocation() + (MuzzleTransform.GetRotation().Vector() * 1000.0f), ECC_GameTraceChannel1, CollisionParams);
#ifdef SHOW_DEBUG_WEAPON_TRACELINES
				DrawDebugLine(GetWorld(), MuzzleTransform.GetLocation(), HitLocation, FColor::Red, false, 20.0f, 0, 0.6f);
#endif
				EquippedWeapon->LaunchProjectileType(World, MuzzleTransform, RotDirection);
				Attack();
			}
		}
		else
		{
			Attack();
		}
	}
}

void AMain::ESCUp()
{
	bESCDown = false;
}

void AMain::DecrementHealth(float Amount)
{
	if (Health - Amount <= 0.0f)
	{
		Health -= Amount;
		Die();
	}
	else
	{
		Health -= Amount;
	}
}

void AMain::IncrementCoins(int32 Amount)
{
	Coins += Amount;
}

void AMain::IncrementHealth(float Amount)
{
	if (Health + Amount >= MaxHealth)
	{
		Health = MaxHealth;
	}
	else
	{
		Health += Amount;
	}
}

void AMain::Die()
{
	if (MovementStatus == EMovementStatus::EMS_Dead) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && GunCombatMontage)
	{
		AnimInstance->Montage_Play(GunCombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
	}
	SetMovementStatus(EMovementStatus::EMS_Dead);
}

void AMain::Jump()
{
	if (MovementStatus != EMovementStatus::EMS_Dead)
	{
		Super::Jump();
	}
}

void AMain::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}

void AMain::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
		
		/*if (bIsGunEquipped)
		{
			CameraBoom->SetRelativeLocation(FVector(0.0f, 50.0f, 50.0f));
			CameraZoomTarget = CameraBoom->TargetArmLength = 300.0f;
			//SetCameraZoomTarget(300.0f, CameraZoomOutSpeed);
		}
		else
		{
			CameraBoom->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
			CameraZoomTarget = CameraBoom->TargetArmLength = 500.0f;
			//SetCameraZoomTarget(500.0f, CameraZoomOutSpeed);
		}*/
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
		
		/*CameraZoomTarget = CameraBoom->TargetArmLength = 350.0f;
		//SetCameraZoomTarget(200.0f, 0.2f);*/
	}
}


void AMain::ShiftKeyDown()
{
	bShiftKeyDown = true;
}


void AMain::ShiftKeyUp()
{
	bShiftKeyDown = false;
}

void AMain::ShowPickupLocations()
{
	for (auto Location : PickupLocations)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.0f, 8, FLinearColor::Green, 10.0f, 0.5f);
	}
}

void AMain::SetEquippedWeapon(AWeapon* WeaponToSet)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}

	EquippedWeapon = WeaponToSet;

}

void AMain::Attack()
{
	//if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead)
	{
		bAttacking = true;
		//SetInterpToEnemy(true);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && GunCombatMontage)
		{
			int32 Section = FMath::RandRange(0, 1);
			switch (Section)
			{
			case 0:
				AnimInstance->Montage_Play(GunCombatMontage, 2.2f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), GunCombatMontage);
				break;
			case 1:
				AnimInstance->Montage_Play(GunCombatMontage, 1.8f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), GunCombatMontage);
				break;

			default:
				;
			}
		}
	}
}

void AMain::AttackEnd()
{
	bAttacking = false;
	SetInterpToEnemy(false);
	if (bLMBDown)
	{
		Attack();
	}
}

void AMain::PlaySwingSound()
{
	if (EquippedWeapon->SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}
}

void AMain::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

float AMain::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	if (Health - DamageAmount <= 0.0f)
	{
		Health -= DamageAmount;

		//:PH 
		//Die();
		Health = 0.0f;

		if (DamageCauser)
		{
			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
			if (Enemy)
			{
				Enemy->bHasValidTarget = false;
			}
		}
	}
	else
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}

void AMain::UpdateCombatTarget()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	if (OverlappingActors.Num() == 0)
	{
		if (MainPlayerController)
		{
			MainPlayerController->RemoveEnemyHealthBar();
		}
		return;
	}

	AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);
	if (ClosestEnemy)
	{
		FVector Location = GetActorLocation();
		float MinDistance = (ClosestEnemy->GetActorLocation() - Location).Size();

		for (auto Actor : OverlappingActors)
		{
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			if (Enemy)
			{
				float DistanceToActor = (Enemy->GetActorLocation() - Location).Size();
				if (DistanceToActor < MinDistance)
				{
					MinDistance = DistanceToActor;
					ClosestEnemy = Enemy;
				}
			}
		}
		if (MainPlayerController)
		{
			MainPlayerController->DisplayEnemyHealthBar();
		}
		SetCombatTarget(ClosestEnemy);
		bHasCombatTarget = true;
	}
}

void AMain::SwitchLevel(FName LevelName)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FString CurrentLevel = World->GetMapName();
		CurrentLevel.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

		FName CurrentLevelName(*CurrentLevel);
		if (CurrentLevelName != LevelName)
		{
			FString Level = LevelName.ToString();
			UE_LOG(LogTemp, Warning, TEXT("CurrentLevel: %s"), *CurrentLevel)
			UE_LOG(LogTemp, Warning, TEXT("LevelName: %s"), *Level)
			UGameplayStatics::OpenLevel(World, LevelName);
		}
	}
}

void AMain::SaveGame()
{
	UFirstSaveGame* SaveObject = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));

	SaveObject->CharacterStats.Health = Health;
	SaveObject->CharacterStats.MaxHealth = MaxHealth;
	SaveObject->CharacterStats.Stamina = Stamina;
	SaveObject->CharacterStats.MaxStamina = MaxStamina;
	SaveObject->CharacterStats.Coins = Coins;
	SaveObject->CharacterStats.Location = GetActorLocation();
	SaveObject->CharacterStats.Rotation = GetActorRotation();

	FString MapName = GetWorld()->GetMapName();
	UE_LOG(LogTemp, Warning, TEXT("MapName: %s"), *MapName)
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	UE_LOG(LogTemp, Warning, TEXT("MapName: %s"), *MapName)
	SaveObject->CharacterStats.LevelName = MapName;
	UE_LOG(LogTemp, Warning, TEXT("SaveObject->CharacterStats.LevelName: %s"), *SaveObject->CharacterStats.LevelName)
	if (EquippedWeapon)
	{
		SaveObject->CharacterStats.WeaponName = EquippedWeapon->Name;
		SaveObject->CharacterStats.bWeaponParticles = EquippedWeapon->bWeaponParticles;
	}

	UGameplayStatics::SaveGameToSlot(SaveObject, SaveObject->SaveSlotName, SaveObject->UserIndex);
}

void AMain::LoadGame(bool LoadPosition)
{
	UFirstSaveGame* Load = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	UFirstSaveGame* LoadObject = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(Load->SaveSlotName, Load->UserIndex));

	if (LoadObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("Level: %s"), *LoadObject->CharacterStats.LevelName)
		if (LoadObject->CharacterStats.LevelName != "")
		{
			FName Map(*LoadObject->CharacterStats.LevelName);
			SwitchLevel(Map);
		}

		Health = LoadObject->CharacterStats.Health;
		MaxHealth = LoadObject->CharacterStats.MaxHealth;

		Stamina = LoadObject->CharacterStats.Stamina;
		MaxStamina = LoadObject->CharacterStats.MaxStamina;

		Coins = LoadObject->CharacterStats.Coins;


		if (WeaponContainer)
		{
			AWeaponContainerActor* Container = GetWorld()->SpawnActor<AWeaponContainerActor>(WeaponContainer);
			if (Container)
			{
				FString WeaponName = LoadObject->CharacterStats.WeaponName;
				if (Container->WeaponMap.Num() > 0)
				{
					if (Container->WeaponMap.Contains(WeaponName))
					{
						AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(Container->WeaponMap[WeaponName]);
						if (Weapon)
						{
							Weapon->bWeaponParticles = LoadObject->CharacterStats.bWeaponParticles;
							Weapon->Equip(this);

						}
					}
				}

			}
		}
		if (LoadPosition)
		{
			SetActorLocation(LoadObject->CharacterStats.Location);
			SetActorRotation(LoadObject->CharacterStats.Rotation);
		}
	}
}

void AMain::LoadGameNoSwitch()
{
	UFirstSaveGame* Load = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	UFirstSaveGame* LoadObject = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(Load->SaveSlotName, Load->UserIndex));

	if (LoadObject)
	{
		Health = LoadObject->CharacterStats.Health;
		MaxHealth = LoadObject->CharacterStats.MaxHealth;

		Stamina = LoadObject->CharacterStats.Stamina;
		MaxStamina = LoadObject->CharacterStats.MaxStamina;

		Coins = LoadObject->CharacterStats.Coins;


		if (WeaponContainer)
		{
			AWeaponContainerActor* Container = GetWorld()->SpawnActor<AWeaponContainerActor>(WeaponContainer);
			if (Container)
			{
				FString WeaponName = LoadObject->CharacterStats.WeaponName;
				if (Container->WeaponMap.Num() > 0)
				{
					if (Container->WeaponMap.Contains(WeaponName))
					{
						AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(Container->WeaponMap[WeaponName]);
						if (Weapon)
						{
							Weapon->bWeaponParticles = LoadObject->CharacterStats.bWeaponParticles;
							Weapon->Equip(this);

						}
					}
				}

			}
		}
	}
}