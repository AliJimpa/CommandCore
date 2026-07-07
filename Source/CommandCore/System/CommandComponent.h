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

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCommandsExecuted OnCommandsExecuted;

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
	UFUNCTION(CallInEditor, Category = "Editor", meta = (DisplayName = "Construction"))
	virtual void Construction() {}
#endif
};
