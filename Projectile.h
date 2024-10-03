// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
//#include "Paper2D/PaperSpriteComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "PulsingLight.h"
#include "Target.h"
#include "Weapon.h"
#include "Projectile.generated.h"

UCLASS()
class FIRSTPROJECT_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	// Sphere collision component.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Projectile")
	class USphereComponent* CollisionComponent;

	// Collision component for aoe/splash damage
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Projectile")
	class USphereComponent* SplashCollision;

	// Projectile mesh component.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Projectile")
	class UStaticMeshComponent* ProjectileMesh;

	// Projectile movement component.
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile | Movement")
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile | Movement")
	UMaterialInstanceDynamic* ProjectileMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile | Particles")
	class UParticleSystemComponent* IdleParticlesComponent;

/*
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile | Sprite")
	class UPaperSpriteComponent* ProjectileSprite;
*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile | Particles")
	class UParticleSystem* OverlapParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile | Sound")
	class USoundCue* OnHitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile | Sound")
	TArray<USoundCue*>OnProjectileSounds;

	// Pulsing Light to spawn.
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class APulsingLight>PulsingLight;

	float TravelTime;

	EProjectileType ProjectileType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile | Movement")
	float LifeSpan;

	// Function that is called when the projectile hits something.
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	// Function that initializes the projectile's velocity in the shoot direction.
	void FireInDirection(const FVector& ShootDirection);

	void ScatterFireInDirection(const FVector& ShootDirection);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
