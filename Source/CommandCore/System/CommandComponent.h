// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CommandComponent.generated.h"

/** Broadcast whenever ExecuteCommands() is called on this component. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCommandsExecuted, AActor *, InstigatorActor);

UCLASS(Abstract)
class COMMANDCORE_API UCommandComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UPROPERTY(BlueprintAssignable, AdvancedDisplay, Category = "Events")
	FOnCommandsExecuted OnCommandsExecuted;

protected:
	/**
	 * If true, ExecuteCommands() will be called automatically when the game starts,
	 * using the owning actor as both OwnerActor and InstigatorActor.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Setting")
	bool bAutoExecuteOnBeginPlay = false;
	/**
	 * If true, ExecuteCommands() will be called automatically when the component/actor
	 * ends play, using the owning actor as both OwnerActor and InstigatorActor.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Setting")
	bool bAutoExecuteOnEndPlay = false;

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
	UFUNCTION(BlueprintCallable, Category = "CommandComponent|Base", meta = (ToolTip = "Executes all commands in this component, in order, passing this component's owner as OwnerActor and the given actor as InstigatorActor."))
	virtual void ExecuteCommands(AActor *InstigatorActor) {}

private:
#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category = "Command", meta = (DisplayName = "Construction"))
	virtual void Construction() {}
#endif
};
