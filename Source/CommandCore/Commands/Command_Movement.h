#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "GameFramework/MovementComponent.h"
#include "Engine/CommandDebug.h"
#include "Command_Movement.generated.h"

class UMovementComponent;

UENUM()
enum class ECommandTargetActor : uint8
{
    OwnerActor UMETA(DisplayName = "Owner Actor"),
    InstigatorActor UMETA(DisplayName = "Instigator Actor"),
    OtherActor UMETA(DisplayName = "Other Actor")
};

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
    UPROPERTY(EditAnywhere, Category = "Command|Movement")
    ECommandTargetActor TargetActor = ECommandTargetActor::OwnerActor;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Movement", meta = (EditCondition = "TargetActor == ECommandTargetActor::OtherActor", EditConditionHides, AllowPrivateAccess = "true"))
    TObjectPtr<AActor> OtherActor;

    /** True = Pause movement (deactivate + stop). False = Play/Resume movement. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Movement")
    bool bPauseMovement = false;



    /** Resolves TargetActor into the actual actor pointer for this execution. */
    AActor *ResolveTargetActor(AActor *OwnerActor, AActor *InstigatorActor) const
    {
        switch (TargetActor)
        {
        case ECommandTargetActor::OwnerActor:
            return OwnerActor;
        case ECommandTargetActor::InstigatorActor:
            return InstigatorActor;
        case ECommandTargetActor::OtherActor:
            return OtherActor;
        default:
            LOG_WARNING("Unhandle");
            return nullptr;
        }
    }

protected:
    virtual void StartMovement(UMovementComponent *MovementComp)
    {
        MovementComp->Activate();
    }
    virtual void StopMovement(UMovementComponent *MovementComp)
    {
        MovementComp->StopMovementImmediately();
        MovementComp->Deactivate();
    }

    virtual bool K2_CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const override
    {
        if (!Super::K2_CanExecute_Implementation(OwnerActor, InstigatorActor))
        {
            return false;
        }

        const AActor *Target = ResolveTargetActor(OwnerActor, InstigatorActor);
        return Target && Target->FindComponentByClass<UMovementComponent>() != nullptr;
    }
    virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
    {
        AActor *Target = ResolveTargetActor(OwnerActor, InstigatorActor);
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

        // Play / Pause
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