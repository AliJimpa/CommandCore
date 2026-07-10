#pragma once

#include "CoreMinimal.h"
#include "System/CommandComponent.h"
#include "CommandExecutorComponent.generated.h"

class UCommand;

/**
 * Holds an ordered list of Commands and executes them in sequence.
 *
 * Attach this to any actor, configure the Commands array in the Details panel
 * (each entry picks a concrete UCommand_* class and exposes its own properties
 * inline), then call ExecuteCommands() from anywhere - Blueprint, C++, input
 * events, triggers, etc. - to run the whole sequence.
 */
UCLASS(NotBlueprintable, BlueprintType, ClassGroup = (CommandCore), meta = (BlueprintSpawnableComponent, DisplayName = "Command Executor"))
class COMMANDCORE_API UCommandExecutorComponent : public UCommandComponent
{
    GENERATED_BODY()
public:
    UCommandExecutorComponent();

protected:
    /** Ordered list of commands to run when ExecuteCommands() is called. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Command")
    TArray<TObjectPtr<UCommand>> Commands;

protected:
    virtual void ExecuteCommands(AActor *InstigatorActor) override;
#if WITH_EDITOR
    virtual void Construction() override;
#endif
};