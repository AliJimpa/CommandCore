#include "Command.h"
#include "Engine/CommandDebug.h"

void UCommand::Construction(AActor *OwnerActor)
{
	K2_OnConstruction(OwnerActor);
}

void UCommand::K2_OnConstruction_Implementation(AActor *OwnerActor)
{
}

void UCommand::Execute(AActor *OwnerActor, AActor *InstigatorActor)
{
	if (K2_CanExecute(OwnerActor, InstigatorActor))
	{
		K2_Execute(OwnerActor, InstigatorActor);
	}
	else
	{
		Print(TEXT("CanExecute retrun false, Execution skiped."), false);
	}
}

void UCommand::K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor)
{
	if (bPrintLog)
		LOG("[%s]: Executed.", *GetName());
}

bool UCommand::K2_CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const
{
	return OwnerActor != nullptr && InstigatorActor != nullptr;
}

AActor *UCommand::ResolveTargetActor(AActor *OwnerActor, AActor *InstigatorActor, const ECommandTargetActor &Mode) const
{
	switch (Mode)
	{
	case ECommandTargetActor::OwnerActor:
		return OwnerActor;
	case ECommandTargetActor::InstigatorActor:
		return InstigatorActor;
	case ECommandTargetActor::OtherActor:
		return K2_GetOtherActor();
	}
	if (bPrintLog)
		LOG_ERROR("Unhandle TargetActor State for Resolving.");
	return nullptr;
}

void UCommand::Print(const FString &Message, bool IsError)
{
	if (bPrintLog)
	{
		const FString MessageTEXT = FString::Printf(TEXT("[%s]: %s"), *GetName(), *Message);
		if (GEngine)
		{
			const FColor MessageColor = IsError ? FColor::Red : FColor::Yellow;
			const float MessageTime = IsError ? 15.0f : 8.0f;
			GEngine->AddOnScreenDebugMessage(-1, MessageTime, MessageColor, MessageTEXT);
		}
		if (IsError)
		{
			LOG_ERROR("%s", *MessageTEXT);
		}
		else
		{
			LOG_WARNING("%s", *MessageTEXT);
		}
	}
}
