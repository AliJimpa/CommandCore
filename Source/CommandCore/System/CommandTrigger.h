#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "CommandTrigger.generated.h"

class UBoxComponent;
class UTriggerCommand;

/**
 * Trigger volume - the "Invoker" in Command-pattern terms.
 *
 * Holds two ordered lists of UTriggerCommand objects: one run on BeginOverlap,
 * one run on EndOverlap. The volume itself knows nothing about *what* the
 * commands do - it just calls CanExecute()/Execute() on each one in order.
 * That's what makes it easy to add new predefined commands later without
 * ever touching this class.
 */
UCLASS(Blueprintable, ClassGroup = (TriggerCommandSystem))
class COMMANDCORE_API ACommandTrigger : public ATriggerBox
{
	GENERATED_BODY()

public:
	// ACommandTrigger();

	// UFUNCTION(BlueprintCallable, Category = "Trigger")
	// FORCEINLINE UBoxComponent *GetTriggerVolume() const { return TriggerVolume; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// /** Collision volume driving the overlap events. */
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trigger")
	// TObjectPtr<UBoxComponent> TriggerVolume;

	/** Commands run, in order, when something begins overlapping the volume. */
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Trigger|Commands")
	TArray<TObjectPtr<UTriggerCommand>> OnBeginOverlapCommands;

	/** Commands run, in order, when something stops overlapping the volume. */
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Trigger|Commands")
	TArray<TObjectPtr<UTriggerCommand>> OnEndOverlapCommands;

	/** If set, only actors of one of these classes will fire the trigger. Empty = anyone. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trigger|Filter")
	TArray<TSubclassOf<AActor>> ActorClassFilter;

	/** If set (not "None"), the overlapping actor must have this Actor Tag. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trigger|Filter")
	FName RequiredActorTag = NAME_None;

	/** If true, the Begin-overlap commands only ever fire once. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trigger|Settings")
	bool bTriggerOnce = false;

	/** Set true after the trigger has fired once, when bTriggerOnce is set. */
	UPROPERTY(BlueprintReadOnly, Category = "Trigger|Settings")
	bool bHasTriggered = false;

	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	void HandleEndOverlap(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);

	bool PassesFilter(AActor *OtherActor) const;
	void RunCommands(const TArray<TObjectPtr<UTriggerCommand>> &Commands, AActor *OtherActor);
};
