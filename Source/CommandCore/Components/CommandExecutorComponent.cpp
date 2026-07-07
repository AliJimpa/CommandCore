#include "CommandExecutorComponent.h"
#include "System/Command.h"

UCommandExecutorComponent::UCommandExecutorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCommandExecutorComponent::ExecuteCommands(AActor *InstigatorActor)
{
	AActor *OwnerActor = GetOwner();

	if (!OwnerActor)
	{
		return;
	}

	for (UCommand *Command : Commands)
	{
		if (Command)
		{
			Command->Execute(OwnerActor, InstigatorActor);
		}
	}

	OnCommandsExecuted.Broadcast(InstigatorActor);
}

#if WITH_EDITOR
void UCommandExecutorComponent::Construction()
{
	AActor *OwnerActor = GetOwner();
	for (UCommand *Command : Commands)
	{
		if (Command)
		{
			Command->Construction(OwnerActor);
		}
	}
}
#endif
