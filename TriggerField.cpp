// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerField.h"

// Sets default values
ATriggerField::ATriggerField()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Scene root component
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	// Collision box
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	TriggerBox->SetupAttachment(GetRootComponent());

	// Trigger variables
	bDontUntrigger = false;
	bOneShot = false;
	SwitchTime = 2.0f;
	bHasTriggered = false;
	bHasUnTriggered = false;
	CharacterOverlap = nullptr;
	bCanTrigger = false;
	bDisableTrigger = false;

	// Enums
	TriggerType = EOverlapTriggerTypes::ETT_Overlap;
}

// Called when the game starts or when spawned
void ATriggerField::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ATriggerField::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ATriggerField::OnOverlapEnd);
}

// Called every frame
void ATriggerField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDisableTrigger)
		return;
	if (TriggerType == EOverlapTriggerTypes::ETT_Enter || TriggerType == EOverlapTriggerTypes::ETT_EnterExit)
	{
		return;
	}
	// Triggers based on type
	if (CharacterOverlap)
	{
		switch (TriggerType)
		{
		case EOverlapTriggerTypes::ETT_Overlap:
			if (CharacterOverlap)
			{
				OnTrigger(TriggeredActor);
			}
			break;
		case EOverlapTriggerTypes::ETT_OverlapExit:
			if (CharacterOverlap)
			{
				OnTrigger(TriggeredActor);
			}
			break;
		case EOverlapTriggerTypes::ETT_Button:
			if (CharacterOverlap->IsKeyPressed)
			{
				OnTrigger(TriggeredActor);
			}
			break;
		case EOverlapTriggerTypes::ETT_ButtonToggle:
			if (CharacterOverlap->IsKeyPressed)
			{
				if (++CharacterOverlap->KeyPressCount >= 2)
				{
					CharacterOverlap->KeyPressCount = 0;
					OffTrigger();
				}
				else
				{
					OnTrigger(TriggeredActor);
				}
				CharacterOverlap->IsKeyPressed = 0;
			}
			break;
		case EOverlapTriggerTypes::ETT_Object:
			if (CharacterOverlap->Collectable)
			{
				if (CharacterOverlap->Collectable->CollectableType == ObjectType)
				{
					OnTrigger(TriggeredActor);
				}
			}
			break;
			/*
				case EOverlapTriggerTypes::ETT_PainBound:
					if (/ *pain bound check* /)
					{
						OnTrigger(TriggeredActor);
					}
					break;
				case EOverlapTriggerTypes::ETT_DeathBound:
					if (/ *death bound check* /)
					{
						OnTrigger(TriggeredActor);
					}
					break;
				case EOverlapTriggerTypes::ETT_KnockBack:
					if (/ *knock back check* /)
					{
						OnTrigger(TriggeredActor);
					}*/
		default:
			;
		}
	}
	else
	{
		// Nothing is in the trigger, and the trigger volume was previously triggered
		if (bHasTriggered)
		{
			// we want to untrigger 
			if (!bDontUntrigger)
			{
				// and we haven't previously untriggered
				if (!bHasUnTriggered)
				{
					if (TriggerType == EOverlapTriggerTypes::ETT_Overlap || TriggerType == EOverlapTriggerTypes::ETT_ButtonToggle)
					{
						return;
					}
					if (SwitchTime !=  0.0f)
					{
						// After SwitchTime seconds call the Detrigger function on all the Triggerable actors (if there are any).
						GetWorldTimerManager().SetTimer(SwitchHandle, this, &ATriggerField::OffTrigger, SwitchTime);
						bHasUnTriggered = true;
					}
					else
					{
						OffTrigger();
					}
				}
			}
			else
			{
				// If we don't want to untrigger ever, disable the triggering mechanism
				bDisableTrigger = true;
			}
		}
	}
}

// Triggerable event
void ATriggerField::OnTrigger(AActor* OtherActor)
{
	// Functionality for when a trigger begins
	if (OtherActor)
	{
		bCanTrigger = true;
		if (CharacterOverlap && bCanTrigger)
		{
			if (!bHasTriggered)
			{
				bHasTriggered = true;
				for (ATriggerable* ActorToTrigger : ActorsToTrigger)
				{
					ActorToTrigger->TriggerActor();
				}
			}
		}
	}
}

void ATriggerField::OffTrigger()
{
	// Functionality for when a trigger ends
	for (ATriggerable* ActorToDetrigger : ActorsToTrigger)
	{
		ActorToDetrigger->DetriggerActor();
	}
	bHasTriggered = false;
	bHasUnTriggered = false;

}

// Overlap begin/end functionality (calls once, once you enter it, never calls again)
void ATriggerField::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// If it's the player overlapping
	AMain* Main = Cast<AMain>(OtherActor);
	if (Main)
	{
		CharacterOverlap = Main;
		TriggeredActor = OtherActor;

		if (TriggerType == EOverlapTriggerTypes::ETT_Enter || TriggerType == EOverlapTriggerTypes::ETT_EnterExit)
		{
			OnTrigger(TriggeredActor);
		}
	}
}
// Calls once, once you exit it.
void ATriggerField::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CharacterOverlap = nullptr;

	if (TriggerType == EOverlapTriggerTypes::ETT_EnterExit)
	{
		OffTrigger();
	}
}

// Trigger / Detrigger actor
void ATriggerField::TriggerActor()
{
	bHasTriggered = true;
}
void ATriggerField::DetriggerActor()
{
	bHasTriggered = false;
}
