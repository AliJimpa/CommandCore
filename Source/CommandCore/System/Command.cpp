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
		if (bPrintLog)
			LOG_WARNING("[%s]: CanExecute retrun false, Execution skiped.", *GetName());
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
	switch (TargetActor)
	{
	case ECommandTargetActor::OwnerActor:
		return OwnerActor;
	case ECommandTargetActor::InstigatorActor:
		return InstigatorActor;
	case ECommandTargetActor::OtherActor:
		return K2_GetOtherActor();
	default:
		LOG_WARNING("Unhandle");
		return nullptr;
	}
}

void UCommand::Print(const FString &Message, bool IsError)
{
	if (GEngine && bPrintLog)
	{
		const FString MessageTEXT = FString::Printf(TEXT("[%s]: %s"), *GetName(), *Message);
		const FColor MessageColor = IsError ? FColor::Red : FColor::Yellow;
		const float MessageTime = IsError ? 15.0f : 8.0f;
		GEngine->AddOnScreenDebugMessage(-1, MessageTime, MessageColor, MessageTEXT);
	}
}
