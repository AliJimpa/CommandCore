#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Command_Print.generated.h"

/** Prints a debug message to screen and/or log. Handy for prototyping trigger logic. */
UCLASS(meta = (DisplayName = "Print"))
class COMMANDCORE_API UCommand_Print : public UCommand
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Print")
	FString Message = TEXT("Trigger fired!");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Print")
	FLinearColor TextColor = FLinearColor::Green;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Print")
	float Duration = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Print")
	bool bPrintToScreen = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Print")
	bool bPrintToLog = true;

public:
	virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
	{
		const FString Full = FString::Printf(TEXT("%s (Owner: %s)"), *Message, OwnerActor ? *OwnerActor->GetName() : TEXT("None"));

		if (bPrintToScreen && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, Duration, TextColor.ToFColor(true), Full);
		}

		if (bPrintToLog)
		{
			UE_LOG(LogTemp, Log, TEXT("[Command] %s"), *Full);
		}
	}
};
