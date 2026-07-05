#include "Command.h"

void UCommand::Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor)
{
	// Base implementation intentionally does nothing - concrete commands override this.
}

bool UCommand::CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const
{
	return true;
}
