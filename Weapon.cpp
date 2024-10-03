// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Main.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"
#include "Engine/SkeletalMeshSocket.h"


AWeapon::AWeapon()
{

	bIsProjectile = false;

	SkeletalMeshRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshRight")); // Right hand weapon mesh
	SkeletalMeshRight->SetupAttachment(GetRootComponent());
	SkeletalMeshLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshLeft")); // Left hand weapon mesh
	SkeletalMeshLeft->SetupAttachment(GetRootComponent());

	CombatCollisionRight = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollisionRight")); // Right hand weapon collision
	CombatCollisionRight->SetupAttachment(GetRootComponent());
	CombatCollisionRight->SetVisibility(false);

	CombatCollisionLeft = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollisionLeft")); // Left hand weapon collision
	CombatCollisionLeft->SetupAttachment(GetRootComponent());
	CombatCollisionLeft->SetVisibility(false);

	bWeaponParticles = false;

	WeaponState = EWeaponState::EWS_Pickup;

	ProjectileType = EProjectileType::EPT_Bullet;

	Damage = 25.f;
	ReloadCount = 0;
	ReloadTicks = 60; // How often the bullet is shot
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	CombatCollisionRight->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapBegin); // Right hand
	CombatCollisionRight->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapEnd);

	CombatCollisionRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollisionRight->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollisionRight->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollisionRight->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	CombatCollisionRight->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);

	if ( SkeletalMeshLeft->MeshObject )
	{
		CombatCollisionLeft->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapBegin); // Left hand
		CombatCollisionLeft->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapEnd);

		CombatCollisionLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CombatCollisionLeft->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		CombatCollisionLeft->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CombatCollisionLeft->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		CombatCollisionLeft->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);
	}
	
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ReloadCount != 0)
	{
		ReloadCount--;
	}
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if ((WeaponState == EWeaponState::EWS_Pickup) && OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			Main->SetActiveOverlappingItem(this);
		}
	}
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			Main->SetActiveOverlappingItem(nullptr);
		}
	}
}

void AWeapon::Equip(AMain* Char)
{
	if (Char)
	{
		SetInstigator(Char->GetController());

		SkeletalMeshRight->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		SkeletalMeshRight->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		SkeletalMeshRight->SetSimulatePhysics(false);

		const USkeletalMeshSocket* RightHandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket");
		if (RightHandSocket)
		{
			RightHandSocket->AttachActor(this, Char->GetMesh());
			bRotate = false;

			Char->SetEquippedWeapon(this);
			Char->SetActiveOverlappingItem(nullptr);

			// turn collision with weapon / projectile off
			CollisionVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		if (OnEquipSound) UGameplayStatics::PlaySound2D(this, OnEquipSound);
		if (!bWeaponParticles)
		{
			IdleParticlesComponent->Deactivate();
		}
		else
		{
			IdleParticlesComponent->Activate();
		}
	}
}


void AWeapon::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
	{
 		if (OtherComp->IsSimulatingPhysics())
 		{
			UE_LOG(LogTemp, Warning, TEXT("Hit physics object %s"), *OtherActor->GetName());

			TArray<UStaticMeshComponent*> Components;
			OtherActor->GetComponents<UStaticMeshComponent>(Components);
			UStaticMeshComponent* ObjectHitComp = Components[0];
 			if ( ObjectHitComp )
			{
				ObjectHitComp->AddForce(OverlappedComponent->GetForwardVector() * 100000 * ObjectHitComp->GetMass());
			}
		}

		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			if (Enemy->HitParticles)
			{
				const USkeletalMeshSocket* WeaponSocket = SkeletalMeshRight->GetSocketByName("WeaponSocket");
				if (WeaponSocket)
				{
					FVector SocketLocation = WeaponSocket->GetSocketLocation(SkeletalMeshRight);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, SocketLocation, FRotator(0.f), false);
				}
			}
			if (Enemy->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
			}
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Enemy, Damage, WeaponInstigator, this, DamageTypeClass);
			}
		}
	}
}


void AWeapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AWeapon::ActivateCollision()
{
	CombatCollisionRight->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}


void AWeapon::DeactivateCollision()
{
	CombatCollisionRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::LaunchProjectileType(UWorld* World, FTransform& MuzzleTransform, FRotator& RotDirection)
{
	// A structure that's stored with with whatever is spawned.
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = nullptr;				// data about who spawned it, which you wanna attach.
	SpawnParams.Instigator = GetInstigator();	// who spawned it.

	AProjectile* Projectile;

	switch (ProjectileType)
	{
	case EProjectileType::EPT_Bullet:
		// Spawn the projectile at the gun muzzle.
		Projectile = World->SpawnActor<AProjectile>(ProjectileClass, MuzzleTransform, SpawnParams);
		if (Projectile)
		{
			if (Projectile->OnProjectileSounds.Num())
			{
				int index = 0;
				if (Projectile->OnProjectileSounds.Num() > 1)
				{
					index = FMath::RandRange(0, Projectile->OnProjectileSounds.Num()-1);
				}
				UGameplayStatics::PlaySound2D(this, Projectile->OnProjectileSounds[index]);
			}
			// Tell the projectile what type it is.
			Projectile->ProjectileType = ProjectileType;
			// Set the projectile's initial trajectory towards the crosshairs.
			Projectile->FireInDirection(RotDirection.Vector());
		}
		break;

	case EProjectileType::EPT_Grenade:
		Projectile = World->SpawnActor<AProjectile>(ProjectileClass, MuzzleTransform, SpawnParams);
		if (Projectile)
		{
			Projectile->ProjectileType = ProjectileType;

			Projectile->ProjectileMovementComponent->bShouldBounce = true;
			Projectile->ProjectileMovementComponent->Bounciness = 0.5f;
			Projectile->ProjectileMovementComponent->ProjectileGravityScale = 4.0f;

			// Set the projectile's initial trajectory towards the crosshairs.
			Projectile->FireInDirection(RotDirection.Vector());
		}
		break;

/*
	case EProjectileType::EPT_Shell:
			// spawn multiple projectiles per click - using range? from an array?
		break;

	case EProjectileType::EPT_Arrow:
		break;
*/

	case EProjectileType::EPT_Bubble:

		Projectile = World->SpawnActor<AProjectile>(ProjectileClass, MuzzleTransform, SpawnParams);
		if (Projectile)
		{
			// Set the projectile's bounce.
			Projectile->ProjectileMovementComponent->bShouldBounce = true;
			Projectile->ProjectileMovementComponent->Bounciness = 0.5f;

			// random scaled bubbles eksdee elemeyo

			// Set the projectile's initial trajectory towards the crosshairs in a cone shape.
			Projectile->ScatterFireInDirection(RotDirection.Vector());
		}
		break;
/*
	case EProjectileType::EPT_Water:
		break;

	case EProjectileType::EPT_Flame:
		break;

	default:
		;

*/
	}

}