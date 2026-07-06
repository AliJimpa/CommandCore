#pragma once

#include "CoreMinimal.h"
#include "Command_Movement.h"
#include "Components/InterpToMovementComponent.h"
#include "Command_InterpMovement.generated.h"

/**
 * Plays/Pauses a UInterpToMovementComponent and optionally overrides its
 * ControlPoints, Duration, and Behaviour Type.
 *
 * Inherits TargetActor resolution and base Play/Pause logic from
 * UCommand_MovementControl, but uses InterpToMovementComponent's own
 * Pause()/Resume() to preserve interpolation progress instead of
 * fully deactivating the component.
 */
UCLASS(meta = (DisplayName = "Interp Movement"))
class COMMANDCORE_API UCommand_InterpMovement : public UCommand_Movement
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Interp", meta = (InlineEditConditionToggle, AllowPrivateAccess = "true"))
    bool bOverrideControlPoints = false;
    UPROPERTY(EditAnywhere, Category = "Command|Interp", meta = (EditCondition = "bOverrideControlPoints", AllowPrivateAccess = "true"))
    TArray<FInterpControlPoint> ControlPoints;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Interp", meta = (InlineEditConditionToggle, AllowPrivateAccess = "true"))
    bool bOverrideDuration = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Interp", meta = (EditCondition = "bOverrideDuration", ClampMin = "0.0", AllowPrivateAccess = "true"))
    float Duration = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Interp", meta = (InlineEditConditionToggle, AllowPrivateAccess = "true"))
    bool bOverrideBehaviourType = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Interp", meta = (EditCondition = "bOverrideBehaviourType", AllowPrivateAccess = "true"))
    EInterpToBehaviourType BehaviourType = EInterpToBehaviourType::OneShot;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, BlueprintReadOnly, Category = "Command|Movement", meta = (InlineEditConditionToggle, AllowPrivateAccess = "true"))
    bool bOverrideSpeedMultiplier = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, BlueprintReadOnly, Category = "Command|Movement", meta = (EditCondition = "bOverrideSpeedMultiplier", AllowPrivateAccess = "true"))
    float SpeedMultiplier = 600.0f;

    /** If true, restarts movement from the first ControlPoint after applying overrides. */
    UPROPERTY(EditAnywhere, Category = "Interp Movement")
    bool bRestartOnExecute = false;

protected:
    virtual void StartMovement(UMovementComponent *MovementComp) override
    {
        UInterpToMovementComponent *InterpComp = Cast<UInterpToMovementComponent>(MovementComp);
        if (!InterpComp)
        {
            Print("MovementComp is not a UInterpToMovementComponent.", true);
            return;
        }

        InterpComp->RestartMovement();
    }
    virtual void StopMovement(UMovementComponent *MovementComp) override
    {
        MovementComp->StopMovementImmediately();
    }

    virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
    {
        AActor *Target = GetTargetActor(OwnerActor, InstigatorActor);
        if (!Target)
        {
            Print("Target actor is invalid.", true);
            return;
        }

        UInterpToMovementComponent *InterpComp = Target->FindComponentByClass<UInterpToMovementComponent>();
        if (!InterpComp)
        {
            Print(FString::Printf(TEXT("%s has no UInterpToMovementComponent."), *Target->GetName()), true);
            return;
        }

        if (bOverrideControlPoints)
        {
            InterpComp->ControlPoints = ControlPoints;
        }

        if (bOverrideDuration)
        {
            InterpComp->Duration = Duration;
        }

        if (bOverrideBehaviourType)
        {
            InterpComp->BehaviourType = BehaviourType;
        }

        if (bRestartOnExecute)
        {
            InterpComp->RestartMovement();
        }

        if (bOverrideSpeedMultiplier)
        {
            InterpComp->SpeedMultiplier = SpeedMultiplier;
        }

        Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
    }
};