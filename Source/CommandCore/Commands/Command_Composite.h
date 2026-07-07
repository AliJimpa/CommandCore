#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Command_Composite.generated.h"

/** Runs a list of other commands, in order. Lets you group several actions behind one array entry. */
UCLASS(meta = (DisplayName = "Composite"))
class COMMANDCORE_API UCommand_Composite : public UCommand
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Command", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UCommand>> Commands;

protected:
	virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
	{
		for (UCommand *Command : Commands)
		{
			if (Command)
			{
				Command->Execute(OwnerActor, InstigatorActor);
			}
			else
			{
				Print("There is null command in Commands list", false);
			}
		}
		Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
	}
};
