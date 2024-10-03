// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"
#include "NewEnemy.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Use sphere as a simple collision representation.
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	// Set the sphere's collision profile name to "Projectile"
	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	// Set the sphere's collision radius.
	CollisionComponent->InitSphereRadius(10.0f);
	// Set the root component to be the collision component.
	SetRootComponent(CollisionComponent);

	// Collision sphere for aoe/splash damage.  Disable until needed!
	SplashCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SplashCollision"));
	SplashCollision->InitSphereRadius(250.0f);
	SplashCollision->SetupAttachment(GetRootComponent());
	SplashCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Particle component
	IdleParticlesComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("IdleParticlesComponent"));
	IdleParticlesComponent->SetupAttachment(GetRootComponent());

	// Projectile mesh
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	ProjectileMesh->SetupAttachment(GetRootComponent());

	// Use this component to drive this projectile's movement.
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(RootComponent);
	ProjectileMovementComponent->InitialSpeed = 3000.0f;
	ProjectileMovementComponent->MaxSpeed = 3000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.3f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

/*
	ProjectileSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("ProjectileSprite"));
	ProjectileSprite->SetupAttachment(GetRootComponent());
*/

	PulsingLight = nullptr;
	TravelTime = 0.0f;
	LifeSpan = 10.0f;

	// Event called when component hits something.
	CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Projectile deletion
	TravelTime += DeltaTime;
	if (TravelTime > LifeSpan - 1) // -1 because UE starts counting from 0 instead of 1
	{
		if (ProjectileType == EProjectileType::EPT_Grenade)
		{
			// Sound
			if (OnHitSound)
			{
				UGameplayStatics::PlaySound2D(this, OnHitSound);
			}
			// Particles
			if (OverlapParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.f), true);
			}
			// Light effect
			if ( PulsingLight)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = nullptr;				// data about who spawned it, which you wanna attach.
				SpawnParams.Instigator = GetInstigator();	// who spawned it.
				GetWorld()->SpawnActor<APulsingLight>(PulsingLight, GetActorLocation(), FRotator(0.0f,0.0f,0.0f), SpawnParams);
			}

			// AOE / Splash Damage
 			float SphereRadius = SplashCollision->GetScaledSphereRadius();
//			DrawDebugSphere(GetWorld(), GetActorLocation(), SphereRadius, 50, FColor::Green, false, 2.0f);

			// check if the collision radius overlaps anything
			TArray<AActor*> OverlappedActors;
			SplashCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			SplashCollision->GetOverlappingActors(OverlappedActors, TSubclassOf<AActor>());

			// loop through the overlapped actors gathered
			for (auto& Hit : OverlappedActors)
			{
// 				if (GEngine)
// 				{
// 					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("%s"), *Hit->GetName()));
// 				}

				// and if the actor is running physics, then apply a radial impulse to it...
				UPrimitiveComponent* Component = Hit->GetComponentByClass<UPrimitiveComponent>();
				if (Component && Component->IsSimulatingPhysics())
				{
					// ERadialImpulseFalloff::RIF_Linear for the impulse to get linearly weaker as it gets further from origin.
					Component->AddRadialImpulse(GetActorLocation(), SphereRadius, 2000.f, ERadialImpulseFalloff::RIF_Linear, true);
				}
			}

		}
		Destroy();
	}
}

// Function that initializes the projectile's velocity in the shoot direction.
void AProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

// For cone fire
void AProjectile::ScatterFireInDirection(const FVector& ShootDirection)
{
	FVector Direction = FMath::VRandCone(ShootDirection, FMath::DegreesToRadians(22.5f));
	ProjectileMovementComponent->Velocity = Direction * ProjectileMovementComponent->InitialSpeed;
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{	
	ATarget* Target = Cast<ATarget>(OtherActor);
	if (Target)
	{
		if ( ProjectileType != EProjectileType::EPT_Grenade )
			Target->TriggerActor();
	}
	else if (OtherActor != this && OtherComponent->IsSimulatingPhysics())
	{
		OtherComponent->AddImpulseAtLocation(ProjectileMovementComponent->Velocity * 10.0f, Hit.ImpactPoint);
	}
	
	ANewEnemy* NewEnemy = Cast<ANewEnemy>(OtherActor);
	if (NewEnemy)
	{
		if (ProjectileType != EProjectileType::EPT_Grenade)
		NewEnemy->EnemyHitParticle();
		NewEnemy->EnemyDeathSound();
		NewEnemy->Destroy();
	}

	if ( ProjectileType != EProjectileType::EPT_Grenade )
	{
		if (OnHitSound)
		{
			UGameplayStatics::PlaySound2D(this, OnHitSound);
		}
		if (OverlapParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.f), true);
		}

		Destroy();
	}

}
