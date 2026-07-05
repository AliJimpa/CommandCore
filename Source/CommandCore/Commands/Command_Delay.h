#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Command_Delay.generated.h"

/**
 * Decorator command: runs an inner command after a delay.
 * Lets you compose e.g. "wait 2 seconds, then play a sound" out of existing commands
 * without writing a new command class for every combination.
 */
UCLASS(meta = (DisplayName = "Delay"))
class COMMANDCORE_API UCommand_Delay : public UCommand
{
	GENERATED_BODY()

public:
	virtual void Execute_Implementation(AActor* TriggerActor, AActor* OtherActor) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command", meta = (ClampMin = "0.0"))
	float DelaySeconds = 1.f;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Command")
	TObjectPtr<UCommand> InnerCommand;
};
