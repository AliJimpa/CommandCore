#include "Commands/Command_Composite.h"

void UCommand_Composite::Execute_Implementation(AActor* TriggerActor, AActor* OtherActor)
{
	for (UTriggerCommand* Command : Commands)
	{
		if (Command && Command->CanExecute(TriggerActor, OtherActor))
		{
			Command->Execute(TriggerActor, OtherActor);
		}
	}
}
