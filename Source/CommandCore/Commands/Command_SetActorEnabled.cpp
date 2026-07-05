#include "Commands/Command_SetActorEnabled.h"

void UCommand_SetActorEnabled::Execute_Implementation(AActor* TriggerActor, AActor* OtherActor)
{
	AActor* Target = TargetActor.LoadSynchronous();
	if (!Target)
	{
		return;
	}

	Target->SetActorTickEnabled(bEnabled);
	Target->SetActorEnableCollision(bEnabled);
}
