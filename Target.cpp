// Fill out your copyright notice in the Description page of Project Settings.

#include "Target.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Main.h"
#include "Projectile.h"


// Sets default values
ATarget::ATarget()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TargetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetMesh"));
	TargetMesh->SetupAttachment(GetRootComponent());

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	TriggerBox->SetBoxExtent(FVector(62.f, 62.f, 32.f));

	bOneShot = false;
	bHasTriggered = false;
	SwitchTime = 2.0f;

	TimeInSeconds = 1.0f;

	RotationDegree = 100.0f;
}

// Called when the game starts or when spawned
void ATarget::BeginPlay()
{
	Super::BeginPlay();

	InitialTargetRotation = TargetMesh->GetComponentRotation();

// 	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ATarget::OnOverlapBegin);
// 	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ATarget::OnOverlapEnd);
}

// Called every frame
void ATarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


/*
void ATarget::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AProjectile* Projectile = Cast<AProjectile>(OtherActor);
	if (Projectile)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Pew Pew Pew"))
		if (!bHasTriggered)
		{
			bHasTriggered = true;
		}
		TriggerActor();
	}
}*/

/*
void ATarget::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// If it's a one shot trigger, the leaving on the bound doesn't trigger anything.
	if (bOneShot)
		return;

	if (OtherActor)
	{
		// if it's the actor leaving the bound
		AProjectile* Projectile = Cast<AProjectile>(OtherActor);
		if (Projectile)
		{
			// After SwitchTime seconds call the Detrigger function on all the Triggerable actors (if there are any).

			GetWorldTimerManager().SetTimer(SwitchHandle, this, &ATarget::Detrigger, SwitchTime);
		}
	}
}*/

void ATarget::TriggerActor()
{
	if (bHasTriggered)
		return;

	Super::TriggerActor();

	/*if (OnHitSound)
	{
		UGameplayStatics::PlaySound2D(this, OnHitSound);
	}*/

	OnTargetTriggeredBP();

	GetWorldTimerManager().SetTimer(SwitchHandle, this, &ATarget::Detrigger, SwitchTime);

}

void ATarget::UpdateTargetLocation(float Rotation)
{
	FRotator NewRotation = InitialTargetRotation;

	Rotation *= RotationDegree;
	NewRotation.Pitch += Rotation;

	TargetMesh->SetWorldRotation(NewRotation);
}

void ATarget::DetriggerActor()
{
	Super::DetriggerActor();

	OnTargetDetriggeredBP();
}


void ATarget::Detrigger()
{
	if (!bOneShot)
	{
		DetriggerActor();
	}
	bHasTriggered = false;
}
