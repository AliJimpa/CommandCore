#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Command_Delay.generated.h"

/**
 * Decorator command: runs an inner command after a delay.
 * Lets you compose e.g. "wait 2 seconds, then play a sound" out of existing commands
 * without writing a new command class for every combination.
 */
UCLASS(meta = (DisplayName = "Delay"))
class COMMANDCORE_API UCommand_Delay : public UCommand
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Delay", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
	float DelaySeconds = 1.f;
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Command|Delay", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCommand> InnerCommand;

protected:
	virtual bool K2_CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const override
	{
		return Super::K2_CanExecute_Implementation(OwnerActor, InstigatorActor) && InnerCommand != nullptr;
	}
	virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
	{
		UWorld *World = OwnerActor->GetWorld();
		if (!World)
		{
			Print("Try GetWorld form OwnerActor failed", true);
			return;
		}

		TWeakObjectPtr<UCommand> WeakInner(InnerCommand);
		TWeakObjectPtr<AActor> WeakTrigger(OwnerActor);
		TWeakObjectPtr<AActor> WeakOther(InstigatorActor);

		FTimerHandle TimerHandle;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([WeakInner, WeakTrigger, WeakOther]()
								 {
									 UCommand *Inner = WeakInner.Get();
									 if (!Inner)
									 {
										return;
									 }

									 AActor *OwnerActor = WeakTrigger.Get();
									 AActor *OtherActorPtr = WeakOther.Get();

									 Inner->Execute(OwnerActor, OtherActorPtr); });

		World->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, FMath::Max(DelaySeconds, 0.f), false);
		Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
	}
};
