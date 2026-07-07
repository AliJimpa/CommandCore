#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Actors/CommandTriggerBox.h"
#include "Command_SetActiveTrigger.generated.h"

/** Prints a debug message to screen and/or log. Handy for prototyping trigger logic. */
UCLASS(meta = (DisplayName = "Set Active Trigger"))
class COMMANDCORE_API UCommand_SetActiveTrigger : public UCommand
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command")
    bool bActive = true;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command")
    TArray<TSoftObjectPtr<ACommandTriggerBox>> Triggers;

protected:
    virtual bool K2_CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const override
    {
        if (Triggers.Num() == 0)
        {
            return false;
        }

        return Super::K2_CanExecute_Implementation(OwnerActor, InstigatorActor);
    }
    virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
    {
        for (const TSoftObjectPtr<ACommandTriggerBox> &SoftActor : Triggers)
        {
            if (ACommandTriggerBox *Trigger = SoftActor.LoadSynchronous())
            {
                Trigger->SetActive(bActive);
            }
        }
        Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
    }
};