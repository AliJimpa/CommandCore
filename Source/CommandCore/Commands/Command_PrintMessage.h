#pragma once

#include "CoreMinimal.h"
#include "System/TriggerCommand.h"
#include "Command_PrintMessage.generated.h"

/** Prints a debug message to screen and/or log. Handy for prototyping trigger logic. */
UCLASS(meta = (DisplayName = "Print Message"))
class COMMANDCORE_API UCommand_PrintMessage : public UTriggerCommand
{
	GENERATED_BODY()

public:
	virtual void Execute_Implementation(AActor* TriggerActor, AActor* OtherActor) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
	FString Message = TEXT("Trigger fired!");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
	FLinearColor TextColor = FLinearColor::Green;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
	float Duration = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
	bool bPrintToScreen = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
	bool bPrintToLog = true;
};
