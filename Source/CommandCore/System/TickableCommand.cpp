// Fill out your copyright notice in the Description page of Project Settings.


#include "TickableCommand.h"

void UTickableCommand::Tick(float DeltaTime)
{
	if (bIsTickable)
	{
		timer += DeltaTime;
		if(timer < Duration)
		{
			ReceiveTick(DeltaTime);
		}else{
			ReceiveFinishTick(DeltaTime);
			bIsTickable = false;
		}
		
	}

	UWorld* MyWorld = GetWorld();
	if (MyWorld)
	{
		FLatentActionManager& LatentActionManager = MyWorld->GetLatentActionManager();
		LatentActionManager.ProcessLatentActions(this, MyWorld->GetDeltaSeconds());
	}
}

bool UTickableCommand::IsTickable() const
{
	return bIsTickable;
}

bool UTickableCommand::IsTickableInEditor() const
{
	return false;
}

bool UTickableCommand::IsTickableWhenPaused() const
{
	return false;
}

TStatId UTickableCommand::GetStatId() const
{
	return TStatId();
}

UWorld* UTickableCommand::GetWorld() const
{
	return GetOuter()->GetWorld();
}

void UTickableCommand::AllowedToTick(bool setTickable)
{
	bIsTickable = setTickable;
	timer = 0.0f;

}

void UTickableCommand::K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor)
{
	Super::K2_Execute_Implementation(OwnerActor,InstigatorActor);
	AllowedToTick(true);
}
