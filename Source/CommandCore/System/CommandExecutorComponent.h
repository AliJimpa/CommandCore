#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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
UCLASS(NotBlueprintable, BlueprintType, ClassGroup = (CommandCore), meta = (BlueprintSpawnableComponent, DisplayName = "Command Executor", Tooltip = "Holds an order list of commands and executes them in order."))
class COMMANDCORE_API UCommandExecutorComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UCommandExecutorComponent();

protected:
    /** Ordered list of commands to run when ExecuteCommands() is called. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Command")
    TArray<TObjectPtr<UCommand>> Commands;

public:
    /**
     * Executes all commands in order.
     *
     * @param InstigatorActor   The actor responsible for triggering this execution
     *                          (e.g. the actor that overlapped a trigger, or the
     *                          player that interacted with this actor). The owning
     *                          actor of this component is automatically passed as
     *                          each command's OwnerActor.
     */
    UFUNCTION(BlueprintCallable, Category = "CommandExecutor", meta = (ToolTip = "Executes all commands in this component, in order, passing this component's owner as OwnerActor and the given actor as InstigatorActor."))
    void ExecuteCommands(AActor *InstigatorActor);
};