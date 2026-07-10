// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/CommandComponent.h"
#include "CommandExecutorComponent.h"
#include "CommandSequenceComponent.generated.h"

class UCommand;

/** A single entry in a Command Sequence - a Command to run at a specific time offset. */
USTRUCT(BlueprintType)
struct FCommandTrack
{
	GENERATED_BODY()

	/** Time (in seconds, from sequence start) at which this command should execute. */
	UPROPERTY(EditAnywhere, Category = "Command Track", meta = (ClampMin = "0.0"))
	float Timeline = 0.0f;

	UPROPERTY(EditAnywhere, Instanced, Category = "Command Track")
	TObjectPtr<UCommand> Command;
};

/** Broadcast when the full sequence finishes running (all commands executed). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCommandSequenceFinished, AActor *, InstigatorActor);

/**
 * Holds an ordered list of Commands, each with a Timeline offset, and executes
 * them in sequence over time - like a lightweight timeline/cue sheet.
 *
 * Call ExecuteCommands() to start the sequence; commands fire automatically
 * as the internal timer reaches each entry's Timeline value, in ascending order.
 */
UCLASS(NotBlueprintable, BlueprintType, ClassGroup = (CommandCore), meta = (BlueprintSpawnableComponent, DisplayName = "Command Sequencer"))
class COMMANDCORE_API UCommandSequenceComponent : public UCommandComponent
{
	GENERATED_BODY()

public:
	UCommandSequenceComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void ExecuteCommands(AActor *InstigatorActor) override;
#if WITH_EDITOR
	virtual void Construction() override;
#endif

public:
	UPROPERTY(BlueprintAssignable,AdvancedDisplay, Category = "Events")
	FOnCommandSequenceFinished OnCommandSequenceFinished;

protected:
	/** Commands to run, each at its own Timeline offset. Sorted by Timeline automatically when the sequence starts. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command")
	TArray<FCommandTrack> Commands;

	/** If true, restarting the sequence while it's already running cancels the current run and starts over. If false, a call while already running is ignored. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Setting")
	bool bRestartIfAlreadyRunning = true;

private:
	UPROPERTY(Transient)
	TObjectPtr<AActor> CachedInstigatorActor;

	bool bIsRunning = false;
	int32 NextCommandIndex = 0;
	float ElapsedTime = 0.0f;

public:
	/** Stops the sequence immediately. Remaining (not-yet-reached) commands will not execute. */
	UFUNCTION(BlueprintCallable, Category = "CommandComponent|Sequencer")
	void StopSequence();

	/** Returns true if the sequence is currently running. */
	UFUNCTION(BlueprintPure, Category = "CommandComponent|Sequencer")
	bool IsRunning() const { return bIsRunning; }
};