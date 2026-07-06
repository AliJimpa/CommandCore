#pragma once

#include "CoreMinimal.h"
#include "Command_Movement.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Command_RotatingMovement.generated.h"

/**
 * Plays/Pauses a URotatingMovementComponent and optionally overrides
 * its RotationRate, PivotTranslation, and RotationInLocalSpace settings.
 */
UCLASS(meta = (DisplayName = "Rotating Movement"))
class COMMANDCORE_API UCommand_RotatingMovement : public UCommand_Movement
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, Category = "Command|Rotating", meta = (InlineEditConditionToggle, AllowPrivateAccess = "true"))
    bool bOverrideRotationRate = false;
    UPROPERTY(EditAnywhere, Category = "Command|Rotating", meta = (EditCondition = "bOverrideRotationRate", AllowPrivateAccess = "true"))
    FRotator RotationRate = FRotator(0.0f, 180.0f, 0.0f);

    UPROPERTY(EditAnywhere, Category = "Command|Rotating", meta = (InlineEditConditionToggle, AllowPrivateAccess = "true"))
    bool bOverridePivotTranslation = false;
    UPROPERTY(EditAnywhere, Category = "Command|Rotating", meta = (EditCondition = "bOverridePivotTranslation", AllowPrivateAccess = "true"))
    FVector PivotTranslation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, Category = "Command|Rotating", meta = (InlineEditConditionToggle, AllowPrivateAccess = "true"))
    bool bOverrideRotationInLocalSpace = false;
    UPROPERTY(EditAnywhere, Category = "Command|Rotating", meta = (EditCondition = "bOverrideRotationInLocalSpace", AllowPrivateAccess = "true"))
    bool bRotationInLocalSpace = true;

protected:
    virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
    {
        AActor *Target = ResolveTargetActor(OwnerActor, InstigatorActor);
        if (!Target)
        {
            Print(TEXT("Target actor is invalid."), true);
            return;
        }

        URotatingMovementComponent *RotatingComp = Target->FindComponentByClass<URotatingMovementComponent>();
        if (!RotatingComp)
        {
            Print(TEXT("MovementComp is not a URotatingMovementComponent."), true);
            return;
        }

        if (bOverrideRotationRate)
        {
            RotatingComp->RotationRate = RotationRate;
        }

        if (bOverridePivotTranslation)
        {
            RotatingComp->PivotTranslation = PivotTranslation;
        }

        if (bOverrideRotationInLocalSpace)
        {
            RotatingComp->bRotationInLocalSpace = bRotationInLocalSpace;
        }

        Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
    }
};