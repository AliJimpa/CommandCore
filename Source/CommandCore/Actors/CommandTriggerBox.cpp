#include "CommandTriggerBox.h"
#include "System/Command.h"

ACommandTriggerBox::ACommandTriggerBox()
{
	PrimaryActorTick.bCanEverTick = false;
	RequiredTags.Add(FName("Player"));
}

void ACommandTriggerBox::BeginPlay()
{
	Super::BeginPlay();

	GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &ACommandTriggerBox::OnTriggerBeginOverlap);
	GetCollisionComponent()->OnComponentEndOverlap.AddDynamic(this, &ACommandTriggerBox::OnTriggerEndOverlap);
}

void ACommandTriggerBox::OnConstruction(const FTransform &Transform)
{
	for (UCommand *Command : OnBeginOverlapCommands)
	{
		if (Command)
		{
			Command->Construction(this);
		}
	}
	for (UCommand *Command : OnEndOverlapCommands)
	{
		if (Command)
		{
			Command->Construction(this);
		}
	}
}

void ACommandTriggerBox::RunCommands(const TArray<TObjectPtr<UCommand>> &Commands, AActor *OtherActor)
{
	for (UCommand *Command : Commands)
	{
		if (Command)
		{
			Command->Execute(this, OtherActor);
		}
	}

	if(bTriggerOnce && bAutoDestroy)
	{
		Destroy();
	}
}

bool ACommandTriggerBox::PassesFilter(AActor *OtherActor) const
{
	if (!OtherActor)
	{
		return false;
	}

	if (ExcludeActorClasses.Num() > 0)
	{
		for (const TSubclassOf<AActor> &ExcludeClass : ExcludeActorClasses)
		{
			if (ExcludeClass && OtherActor->IsA(ExcludeClass))
			{
				return false;
			}
		}
	}

	if (IncludeActorClasses.Num() > 0)
	{
		bool bFoundInInclude = false;
		for (const TSubclassOf<AActor> &IncludeClass : IncludeActorClasses)
		{
			if (IncludeClass && OtherActor->IsA(IncludeClass))
			{
				bFoundInInclude = true;
				break;
			}
		}

		if (!bFoundInInclude)
		{
			return false;
		}
	}

	if (RequiredTags.Num() > 0)
	{
		bool bHasRequiredTag = false;
		for (const FName &Tag : RequiredTags)
		{
			if (OtherActor->ActorHasTag(Tag))
			{
				bHasRequiredTag = true;
				break;
			}
		}

		if (!bHasRequiredTag)
		{
			return false;
		}
	}

	return true;
}

void ACommandTriggerBox::OnTriggerBeginOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
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

void ACommandTriggerBox::OnTriggerEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
	if (!PassesFilter(OtherActor))
	{
		return;
	}

	RunCommands(OnEndOverlapCommands, OtherActor);
}

void ACommandTriggerBox::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetCollisionComponent()->OnComponentBeginOverlap.RemoveDynamic(this, &ACommandTriggerBox::OnTriggerBeginOverlap);
	GetCollisionComponent()->OnComponentEndOverlap.RemoveDynamic(this, &ACommandTriggerBox::OnTriggerEndOverlap);

	Super::EndPlay(EndPlayReason);
}
