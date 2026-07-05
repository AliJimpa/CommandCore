#include "Command.h"

void UCommand::Execute_Implementation(AActor* TriggerActor, AActor* OtherActor)
{
	// Base implementation intentionally does nothing - concrete commands override this.
}

bool UCommand::CanExecute_Implementation(AActor* TriggerActor, AActor* OtherActor) const
{
	return true;
}
