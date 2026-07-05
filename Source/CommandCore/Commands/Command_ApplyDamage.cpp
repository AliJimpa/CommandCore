#include "Commands/Command_ApplyDamage.h"
#include "Kismet/GameplayStatics.h"

bool UCommand_ApplyDamage::CanExecute_Implementation(AActor* TriggerActor, AActor* OtherActor) const
{
	return OtherActor != nullptr;
}

void UCommand_ApplyDamage::Execute_Implementation(AActor* TriggerActor, AActor* OtherActor)
{
	if (!OtherActor)
	{
		return;
	}

	UGameplayStatics::ApplyDamage(OtherActor, DamageAmount, nullptr, TriggerActor, DamageType);
}
