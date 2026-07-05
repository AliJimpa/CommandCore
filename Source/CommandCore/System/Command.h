#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Command.generated.h"

/**
 * Abstract base "Command" object for the Trigger Command System.
 *
 * This is the Command interface in the classic Command design pattern:
 * concrete subclasses encapsulate a single action (open a door, play a sound,
 * apply damage, print a message, etc.) behind one common Execute() entry point.
 *
 * Because it derives from UObject and is marked EditInlineNew + Instanced,
 * designers can add any number of these to a Trigger's command arrays directly
 * in the Details panel - picking a concrete command class from a dropdown and
 * filling in its exposed properties - without writing any code. Concrete
 * commands can also be authored in Blueprint (right-click > create Blueprint
 * class based on any UCommand_* class).
 */
UCLASS(Abstract, EditInlineNew, DefaultToInstanced, BlueprintType, Blueprintable, CollapseCategories)
class COMMANDCORE_API UCommand : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Runs the command.
	 * @param TriggerActor  The trigger volume actor that fired this command.
	 * @param OtherActor    The actor that began/ended overlapping the trigger.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Trigger Command")
	void Execute(AActor* TriggerActor, AActor* OtherActor);
	virtual void Execute_Implementation(AActor* TriggerActor, AActor* OtherActor);

	/** Optional guard, checked before Execute is called. Return false to skip this command. */
	UFUNCTION(BlueprintNativeEvent, Category = "Trigger Command")
	bool CanExecute(AActor* TriggerActor, AActor* OtherActor) const;
	virtual bool CanExecute_Implementation(AActor* TriggerActor, AActor* OtherActor) const;
};
