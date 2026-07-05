// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBase.h"
#include "TriggerTest.generated.h"

/**
 *
 */
UCLASS()
class COMMANDCORE_API ATriggerTest : public ATriggerBase
{
	GENERATED_BODY()

public:
	ATriggerTest(const FObjectInitializer &ObjectInitializer);

#if WITH_EDITOR
	//~ Begin AActor Interface.
	virtual void EditorApplyScale(const FVector &DeltaScale, const FVector *PivotLocation, bool bAltDown, bool bShiftDown, bool bCtrlDown) override;
	//~ End AActor Interface.
#endif
};
