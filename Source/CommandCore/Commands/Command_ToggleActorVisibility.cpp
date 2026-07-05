#include "Commands/Command_ToggleActorVisibility.h"

void UCommand_ToggleActorVisibility::Execute_Implementation(AActor* TriggerActor, AActor* OtherActor)
{
	AActor* Target = TargetActor.LoadSynchronous();
	if (!Target)
	{
		return;
	}

	Target->SetActorHiddenInGame(!bNewVisibility);

	if (bAlsoToggleCollision)
	{
		Target->SetActorEnableCollision(bNewVisibility);
	}
}
