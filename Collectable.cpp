// Fill out your copyright notice in the Description page of Project Settings.


#include "Collectable.h"

ACollectable::ACollectable()
{
	CollectableType = ECollectableType::ECT_YellowKey;

}

// Called when the game starts or when spawned
void ACollectable::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ACollectable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACollectable::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			const USkeletalMeshSocket* HeadSocket = Main->GetMesh()->GetSocketByName("HeadSocket");
			if (HeadSocket)
			{
			//	FRotator Orientation = FRotator(0.0f, 90.0f, 0.0f);
				HeadSocket->AttachActor(this, Main->GetMesh());
			//	Main->SetActorRelativeRotation(Orientation);
				bRotate = true;

				Main->Collectable = this;
				Main->Collectable->SetActorEnableCollision(false);

				PickUpParticle();

				IdleParticlesComponent->SetVisibility(false);
			}
		}
	}
}

void ACollectable::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	//UE_LOG(LogTemp, Warning, TEXT("Pickup::OnOverlapEnd()"));
}