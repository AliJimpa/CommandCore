#include "TriggerCommand.h"

void UTriggerCommand::Execute_Implementation(AActor* TriggerActor, AActor* OtherActor)
{
	// Base implementation intentionally does nothing - concrete commands override this.
}

bool UTriggerCommand::CanExecute_Implementation(AActor* TriggerActor, AActor* OtherActor) const
{
	return true;
}
