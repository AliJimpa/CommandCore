#include "CommandTrigger.h"
#include "Components/BoxComponent.h"
#include "TriggerCommand.h"

// ACommandTrigger::ACommandTrigger()
// {
// 	PrimaryActorTick.bCanEverTick = false;

// 	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
// 	SetRootComponent(TriggerVolume);

// 	TriggerVolume->InitBoxExtent(FVector(100.f, 100.f, 100.f));
// 	TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
// 	TriggerVolume->SetGenerateOverlapEvents(true);
// }

void ACommandTrigger::BeginPlay()
{
	Super::BeginPlay();

	GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &ACommandTrigger::HandleBeginOverlap);
	GetCollisionComponent()->OnComponentEndOverlap.AddDynamic(this, &ACommandTrigger::HandleEndOverlap);
}

bool ACommandTrigger::PassesFilter(AActor *OtherActor) const
{
	if (!OtherActor)
	{
		return false;
	}

	if (RequiredActorTag != NAME_None && !OtherActor->ActorHasTag(RequiredActorTag))
	{
		return false;
	}

	if (ActorClassFilter.Num() > 0)
	{
		bool bMatchesAny = false;
		for (const TSubclassOf<AActor> &Class : ActorClassFilter)
		{
			if (Class && OtherActor->IsA(Class))
			{
				bMatchesAny = true;
				break;
			}
		}
		if (!bMatchesAny)
		{
			return false;
		}
	}

	return true;
}

void ACommandTrigger::RunCommands(const TArray<TObjectPtr<UTriggerCommand>> &Commands, AActor *OtherActor)
{
	for (UTriggerCommand *Command : Commands)
	{
		if (Command && Command->CanExecute(this, OtherActor))
		{
			Command->Execute(this, OtherActor);
		}
	}
}

void ACommandTrigger::HandleBeginOverlap(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (bTriggerOnce && bHasTriggered)
	{
		return;
	}

	if (!PassesFilter(OtherActor))
	{
		return;
	}

	bHasTriggered = true;
	RunCommands(OnBeginOverlapCommands, OtherActor);
}

void ACommandTrigger::HandleEndOverlap(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
	if (!PassesFilter(OtherActor))
	{
		return;
	}

	RunCommands(OnEndOverlapCommands, OtherActor);
}

void ACommandTrigger::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetCollisionComponent()->OnComponentBeginOverlap.RemoveDynamic(this, &ACommandTrigger::HandleBeginOverlap);
	GetCollisionComponent()->OnComponentEndOverlap.RemoveDynamic(this, &ACommandTrigger::HandleEndOverlap);

	Super::EndPlay(EndPlayReason);
}