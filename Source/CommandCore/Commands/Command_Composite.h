#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Command_Composite.generated.h"

/** Runs a list of other commands, in order. Lets you group several actions behind one array entry. */
UCLASS(meta = (DisplayName = "Composite (Run Multiple Commands)"))
class COMMANDCORE_API UCommand_Composite : public UCommand
{
	GENERATED_BODY()

public:
	virtual void Execute_Implementation(AActor* TriggerActor, AActor* OtherActor) override;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Command")
	TArray<TObjectPtr<UCommand>> Commands;
};
