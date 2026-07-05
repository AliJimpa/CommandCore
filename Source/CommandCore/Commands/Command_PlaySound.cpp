#include "Commands/Command_PlaySound.h"
#include "Kismet/GameplayStatics.h"

void UCommand_PlaySound::Execute_Implementation(AActor* TriggerActor, AActor* OtherActor)
{
	if (!Sound || !TriggerActor)
	{
		return;
	}

	if (bPlayAtLocation)
	{
		UGameplayStatics::PlaySoundAtLocation(TriggerActor, Sound, TriggerActor->GetActorLocation(), VolumeMultiplier, PitchMultiplier);
	}
	else
	{
		UGameplayStatics::PlaySound2D(TriggerActor, Sound, VolumeMultiplier, PitchMultiplier);
	}
}
