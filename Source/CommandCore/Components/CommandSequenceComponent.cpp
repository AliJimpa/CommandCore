#include "CommandSequenceComponent.h"
#include "System/Command.h"
#include "Algo/Sort.h"

UCommandSequenceComponent::UCommandSequenceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UCommandSequenceComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);
}

void UCommandSequenceComponent::ExecuteCommands(AActor* InstigatorActor)
{
	if (bIsRunning && !bRestartIfAlreadyRunning)
	{
		return;
	}

	if (Commands.Num() == 0)
	{
		return;
	}

	// Sort by Timeline ascending so we can walk through them in order.
	Algo::Sort(Commands, [](const FCommandTrack& A, const FCommandTrack& B)
	{
		return A.Timeline < B.Timeline;
	});

	CachedInstigatorActor = InstigatorActor;
	NextCommandIndex = 0;
	ElapsedTime = 0.0f;
	bIsRunning = true;

	SetComponentTickEnabled(true);

	OnCommandsExecuted.Broadcast(InstigatorActor);
}

void UCommandSequenceComponent::StopSequence()
{
	bIsRunning = false;
	SetComponentTickEnabled(false);
}

void UCommandSequenceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsRunning)
	{
		return;
	}

	ElapsedTime += DeltaTime;

	AActor* OwnerActor = GetOwner();

	while (NextCommandIndex < Commands.Num() && Commands[NextCommandIndex].Timeline <= ElapsedTime)
	{
		if (UCommand* Command = Commands[NextCommandIndex].Command)
		{
			Command->Execute(OwnerActor, CachedInstigatorActor);
		}

		NextCommandIndex++;
	}

	if (NextCommandIndex >= Commands.Num())
	{
		bIsRunning = false;
		SetComponentTickEnabled(false);
		OnCommandSequenceFinished.Broadcast(CachedInstigatorActor);
	}
}

#if WITH_EDITOR
void UCommandSequenceComponent::Construction()
{
	if (AActor* OwnerActor = GetOwner())
	{
		ExecuteCommands(OwnerActor);
	}
}
#endif