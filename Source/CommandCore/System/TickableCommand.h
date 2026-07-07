// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "TickableCommand.generated.h"

/**
 *
 */
UCLASS()
class COMMANDCORE_API UTickableCommand : public UCommand, public FTickableGameObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default")
	float Duration;

private:
	bool bIsTickable;
	float timer;

public:
	void Tick(float DeltaTime) override;
	bool IsTickable() const override;

	bool IsTickableInEditor() const override;
	bool IsTickableWhenPaused() const override;
	TStatId GetStatId() const override;

	UWorld *GetWorld() const override;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Tick"))
	void ReceiveTick(float DeltaSeconds);
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "FinishTick"))
	void ReceiveFinishTick(float DeltaSeconds);

	UFUNCTION(BlueprintCallable, Category = "Commands")
	void AllowedToTick(bool bIsTickable);

	virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override;
};
