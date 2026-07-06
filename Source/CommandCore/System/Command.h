#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Command.generated.h"

UENUM()
enum class ECommandTargetActor : uint8
{
	OwnerActor UMETA(DisplayName = "Owner Actor"),
	InstigatorActor UMETA(DisplayName = "Instigator Actor"),
	OtherActor UMETA(DisplayName = "Other Actor")
};

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
	/** Called by the owning actor/component's OnConstruction to forward to this command. */
	void Construction(AActor *OwnerActor);
	/**
	 * Executes this command's logic.
	 *
	 * This is the core entry point of the Command Design Pattern implementation.
	 * Each command defines its own behavior inside this function, allowing it to be
	 * triggered from anywhere (Blueprints, C++, triggers, events, etc.) in a decoupled,
	 * reusable way.
	 *
	 * @param OwnerActor        The actor that owns/holds this command (e.g. the actor
	 *                          whose CommandListComponent contains this command).
	 * @param InstigatorActor   The actor that caused/triggered this command's execution
	 *                          (e.g. the actor that overlapped a trigger box or
	 *                          interacted with the owner).
	 */
	void Execute(AActor *OwnerActor, AActor *InstigatorActor);

private:
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Command")
	bool bPrintLog = false;

protected:
	UFUNCTION(BlueprintCallable, Category = "Command", meta = (ToolTip = "Retrun Target Actor Based On SelectedMode", AllowPrivateAccess = "true"))
	AActor *ResolveTargetActor(AActor *OwnerActor, AActor *InstigatorActor, const ECommandTargetActor &Mode) const;
	UFUNCTION(BlueprintCallable, Category = "Command", meta = (ToolTip = "This function will add a debug message to the onscreen message list.", AllowPrivateAccess = "true"))
	void Print(const FString &Message, bool IsError);

	UFUNCTION(BlueprintNativeEvent, Category = "Command", meta = (DisplayName = "Execute", ToolTip = "Executes this command. OwnerActor is the actor that holds this command, InstigatorActor is the actor that triggered its execution.", OwnerActorToolTip = "The actor that owns this command", InstigatorActorToolTip = "The actor that triggered this command's execution", AllowPrivateAccess = "true"))
	void K2_Execute(AActor *OwnerActor, AActor *InstigatorActor);
	virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor);
	/** Optional guard, checked before Execute is called. Return false to skip this command. */
	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Command", meta = (DisplayName = "CanExecute", ToolTip = "Called befor Execute event called. Return false to skip this command.", AllowPrivateAccess = "true"))
	bool K2_CanExecute(AActor *OwnerActor, AActor *InstigatorActor) const;
	virtual bool K2_CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const;
	/** Called in the editor whenever the owning actor's OnConstruction runs */
	UFUNCTION(BlueprintNativeEvent, Category = "Command", meta = (DisplayName = "On Construction"))
	void K2_OnConstruction(AActor *OwnerActor);
	virtual void K2_OnConstruction_Implementation(AActor *OwnerActor);
	UFUNCTION(BlueprintNativeEvent, Category = "Command", meta = (DisplayName = "GetOtherActor"))
	void K2_GetOtherActor();
	virtual AActor *K2_GetOtherActor_Implementation() { return nullptr; }
};
