#pragma once

#include "CoreMinimal.h"
#include "System/TriggerCommand.h"
#include "Command_Composite.generated.h"

/** Runs a list of other commands, in order. Lets you group several actions behind one array entry. */
UCLASS(meta = (DisplayName = "Composite (Run Multiple Commands)"))
class COMMANDCORE_API UCommand_Composite : public UTriggerCommand
{
	GENERATED_BODY()

public:
	virtual void Execute_Implementation(AActor* TriggerActor, AActor* OtherActor) override;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Command")
	TArray<TObjectPtr<UTriggerCommand>> Commands;
};
