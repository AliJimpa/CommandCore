#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Command_Destroy.generated.h"

UCLASS(meta = (DisplayName = "Delay"))
class COMMANDCORE_API UCommand_Destroy : public UCommand
{
    GENERATED_BODY()

protected:
    virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
    {

    }



};