// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EProjectileType :uint8
{
	EPT_Bullet		UMETA(DisplayName = "Bullet"),
	EPT_Grenade		UMETA(DisplayName = "Grenade"),
	EPT_Shell		UMETA(DisplayName = "Shell"),	// shotgun spray
	EPT_Arrow		UMETA(DisplayName = "Arrow"),	
	EPT_Bubble		UMETA(DisplayName = "Bubble"),
	EPT_Water		UMETA(DisplayName = "Water"),	// idk lol
	EPT_Flame		UMETA(DisplayName = "Flame")
};

UENUM(BlueprintType)
enum class EWeaponState :uint8
{
	EWS_Pickup		UMETA(DisplayName = "Pickup"),
	EWS_Equipped	UMETA(DisplayName = "Equipped"),

	EWS_MAX			UMETA(DisplayName = "DefaultMax")
};

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API AWeapon : public AItem
{
	GENERATED_BODY()
public:
	
	AWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bIsProjectile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FString Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
	EWeaponState WeaponState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
	bool bWeaponParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
	class USoundCue* OnEquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
	USoundCue* SwingSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkeletalMeshRight") // right hand weapon mesh
	class USkeletalMeshComponent* SkeletalMeshRight;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkeletalMeshLeft") // left hand weapon mesh
	class USkeletalMeshComponent* SkeletalMeshLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Combat") // right hand weapon collision
	class UBoxComponent* CombatCollisionRight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Combat") // left hand weapon collision
	class UBoxComponent* CombatCollisionLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	float Damage;

	// Projectile class to spawn.
	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<class AProjectile>ProjectileClass;

	// Projectile types
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	EProjectileType ProjectileType;
	void LaunchProjectileType(UWorld* World, FTransform& MuzzleTransform, FRotator& RotDirection);

	// Fire Rate
	int ReloadCount;
	UPROPERTY(EditAnywhere, Category = "Projectile")
	int ReloadTicks;

protected:

	virtual void BeginPlay() override;

public:
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	void Equip(class AMain* Char);

	FORCEINLINE void SetWeaponState(EWeaponState State) { WeaponState = State; }
	FORCEINLINE EWeaponState GetWeaponState() { return WeaponState; }

	UFUNCTION()
	void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void ActivateCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AController* WeaponInstigator;

	FORCEINLINE void SetInstigator(AController* Inst) { WeaponInstigator = Inst; }

};
