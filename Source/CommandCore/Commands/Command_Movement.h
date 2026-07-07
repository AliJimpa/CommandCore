#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "GameFramework/MovementComponent.h"
#include "Engine/CommandDebug.h"
#include "Command_Movement.generated.h"

class UMovementComponent;

/**
 * Plays or pauses an actor's UMovementComponent, with optional overrides
 * for common movement settings (e.g. MaxSpeed, GravityScale).
 *
 * Each override property has its own enable checkbox - only checked
 * properties are applied, everything else is left untouched.
 */
UCLASS(meta = (DisplayName = "Movement"))
class COMMANDCORE_API UCommand_Movement : public UCommand
{
    GENERATED_BODY()

private:
    /** Which actor's MovementComponent this command should affect. */
    UPROPERTY(EditAnywhere, Category = "Command")
    ECommandTargetActor TargetActor = ECommandTargetActor::OwnerActor;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command", meta = (EditCondition = "TargetActor == ECommandTargetActor::OtherActor", EditConditionHides, AllowPrivateAccess = "true"))
    TObjectPtr<AActor> OtherActor;

    /** True = Pause movement (deactivate + stop). False = Play/Resume movement. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command")
    bool bPauseMovement = false;

protected:
    AActor *GetTargetActor(AActor *OwnerActor, AActor *InstigatorActor) const
    {
        return ResolveTargetActor(OwnerActor, InstigatorActor, TargetActor);
    }

    virtual void StartMovement(UMovementComponent *MovementComp)
    {
        MovementComp->Activate();
    }
    virtual void StopMovement(UMovementComponent *MovementComp)
    {
        MovementComp->StopMovementImmediately();
        MovementComp->Deactivate();
    }

    virtual AActor *K2_GetOtherActor_Implementation() const override
    {
        return OtherActor;
    }
    virtual bool K2_CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const override
    {
        if (!Super::K2_CanExecute_Implementation(OwnerActor, InstigatorActor))
        {
            return false;
        }

        const AActor *Target = GetTargetActor(OwnerActor,InstigatorActor);
        return Target && Target->FindComponentByClass<UMovementComponent>() != nullptr;
    }
    virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
    {
        AActor *Target = GetTargetActor(OwnerActor,InstigatorActor);
        if (!Target)
        {
            Print(TEXT("Target actor is invalid."), true);
            return;
        }

        UMovementComponent *MovementComp = Target->FindComponentByClass<UMovementComponent>();
        if (!MovementComp)
        {
            Print(TEXT("has no MovementComponent."), true);
            return;
        }

        if (bPauseMovement)
        {
            StopMovement(MovementComp);
        }
        else
        {
            StartMovement(MovementComp);
        }
        LOG("Movement %s on %s.", bPauseMovement ? TEXT("Paused") : TEXT("Resumed"), *Target->GetName());
        Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
    }
};