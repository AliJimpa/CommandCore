#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "Components/BoxComponent.h"
#include "CommandTriggerBox.generated.h"

class UCommand;

/**
 * A specialized Trigger Box that executes Commands when an actor begins
 * or ends overlap with it, with optional Tag and Actor-based filtering.
 *
 * Filtering rules (all checks must pass for OtherActor to be processed):
 *  1. If RequiredTags is non-empty, OtherActor must have at least one of them.
 *  2. If IncludeActors is non-empty, OtherActor must be in that list
 *     (acts as a whitelist - if set, ONLY these actors can trigger).
 *  3. If OtherActor is in ExcludeActors, it is always rejected, regardless
 *     of the above (blacklist takes priority).
 */
UCLASS(NotBlueprintable, BlueprintType, ClassGroup = (CommandCore), meta = (BlueprintSpawnableComponent, DisplayName = "Command Trigger Box"))
class COMMANDCORE_API ACommandTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

public:
	ACommandTriggerBox();

protected:
	virtual void OnConstruction(const FTransform &Transform) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** Returns true if OtherActor passes all filter rules. */
	bool PassesFilter(AActor *OtherActor) const;
	void RunCommands(const TArray<TObjectPtr<UCommand>> &Commands, AActor *OtherActor);

protected:
	/** Commands run, in order, when something begins overlapping the volume. */
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Trigger")
	TArray<TObjectPtr<UCommand>> OnBeginOverlapCommands;

	/** Commands run, in order, when something stops overlapping the volume. */
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Trigger")
	TArray<TObjectPtr<UCommand>> OnEndOverlapCommands;

	/** If non-empty, OtherActor must have at least one of these tags to pass. */
	UPROPERTY(EditAnywhere, Category = "Trigger|Filter")
	TArray<FName> RequiredTags;

	/** Whitelist: if non-empty, ONLY these actors are allowed to trigger commands. */
	UPROPERTY(EditAnywhere, Category = "Trigger|Filter")
	TArray<TSoftObjectPtr<AActor>> IncludeActors;

	/** Blacklist: these actors are always rejected, even if they pass other filters. */
	UPROPERTY(EditAnywhere, Category = "Trigger|Filter")
	TArray<TSoftObjectPtr<AActor>> ExcludeActors;

	/** If true, the Begin-overlap commands only ever fire once. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trigger|Settings")
	bool bTriggerOnce = false;

	/** Set true after the trigger has fired once, when bTriggerOnce is set. */
	UPROPERTY(BlueprintReadOnly, Category = "Trigger|Settings")
	bool bHasTriggered = false;

private:
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	void OnTriggerEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);
};