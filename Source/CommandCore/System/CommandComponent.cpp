// Fill out your copyright notice in the Description page of Project Settings.

#include "System/CommandComponent.h"
#include "GameFramework/Actor.h"

void UCommandComponent::BeginPlay()
{
    Super::BeginPlay();

    if (bAutoExecuteOnBeginPlay)
    {
        ExecuteCommands(GetOwner());
    }
}

void UCommandComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (bAutoExecuteOnEndPlay)
    {
        ExecuteCommands(GetOwner());
    }

    Super::EndPlay(EndPlayReason);
}
