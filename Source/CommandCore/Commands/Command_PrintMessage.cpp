#include "Commands/Command_PrintMessage.h"
#include "Engine/Engine.h"

void UCommand_PrintMessage::Execute_Implementation(AActor* TriggerActor, AActor* OtherActor)
{
	const FString Full = FString::Printf(TEXT("%s (Other: %s)"), *Message, OtherActor ? *OtherActor->GetName() : TEXT("None"));

	if (bPrintToScreen && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, TextColor.ToFColor(true), Full);
	}

	if (bPrintToLog)
	{
		UE_LOG(LogTemp, Log, TEXT("[Command] %s"), *Full);
	}
}
