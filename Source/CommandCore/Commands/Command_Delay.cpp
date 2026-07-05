#include "Commands/Command_Delay.h"
#include "TimerManager.h"
#include "Engine/World.h"

void UCommand_Delay::Execute_Implementation(AActor* TriggerActor, AActor* OtherActor)
{
	if (!InnerCommand || !TriggerActor)
	{
		return;
	}

	UWorld* World = TriggerActor->GetWorld();
	if (!World)
	{
		return;
	}

	TWeakObjectPtr<UTriggerCommand> WeakInner(InnerCommand);
	TWeakObjectPtr<AActor> WeakTrigger(TriggerActor);
	TWeakObjectPtr<AActor> WeakOther(OtherActor);

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([WeakInner, WeakTrigger, WeakOther]()
	{
		UTriggerCommand* Inner = WeakInner.Get();
		if (!Inner)
		{
			return;
		}

		AActor* TriggerActorPtr = WeakTrigger.Get();
		AActor* OtherActorPtr = WeakOther.Get();

		if (Inner->CanExecute(TriggerActorPtr, OtherActorPtr))
		{
			Inner->Execute(TriggerActorPtr, OtherActorPtr);
		}
	});

	World->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, FMath::Max(DelaySeconds, 0.f), false);
}
