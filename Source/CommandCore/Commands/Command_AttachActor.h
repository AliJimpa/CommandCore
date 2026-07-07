#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Engine/CommandDebug.h"
#include "Command_AttachActor.generated.h"

/**
 * Attaches a target actor to another actor's component/socket.
 *
 * The "Attach To" actor can either be resolved automatically as the
 * InstigatorActor (bUseInstigatorAsAttachTarget = true), or picked
 * explicitly from the scene (bUseInstigatorAsAttachTarget = false).
 */
UCLASS(meta = (DisplayName = "Attach Actor"))
class COMMANDCORE_API UCommand_AttachActor : public UCommand
{
    GENERATED_BODY()

protected:
    /** Which actor should be attached (moved to become a child of the Attach-To actor). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
    TSoftObjectPtr<AActor> TargetActor;

    /** If true, InstigatorActor is used as the actor to attach TO, and AttachToActor below is ignored. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Setting")
    bool bUseInstigatorAsAttachTarget = false;

    /** Actor to attach TargetActor to. Only used if bUseInstigatorAsAttachTarget is false. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Setting", meta = (EditCondition = "!bUseInstigatorAsAttachTarget", EditConditionHides))
    TSoftObjectPtr<AActor> AttachToActor;

    /** Optional socket/bone name on the Attach-To actor's root component to attach to. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Setting")
    FName SocketName = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Setting")
    EAttachmentRule LocationRule = EAttachmentRule::SnapToTarget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Setting")
    EAttachmentRule RotationRule = EAttachmentRule::SnapToTarget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Setting")
    EAttachmentRule ScaleRule = EAttachmentRule::KeepWorld;

    /** If true, disables physics simulation on TargetActor's root component before attaching (recommended for most cases). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Setting")
    bool bDisablePhysicsBeforeAttach = true;

private:
    AActor *ResolveAttachToActor(AActor *InstigatorActor) const
    {
        if (bUseInstigatorAsAttachTarget)
        {
            return InstigatorActor;
        }

        return AttachToActor.LoadSynchronous();
    }

protected:
    virtual bool K2_CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const override
    {
        if (!bUseInstigatorAsAttachTarget && AttachToActor.IsNull())
        {
            return false;
        }

        return Super::K2_CanExecute_Implementation(OwnerActor, InstigatorActor);
    }
    virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
    {
        AActor *Target = TargetActor.LoadSynchronous();
        AActor *AttachTo = ResolveAttachToActor(InstigatorActor);

        if (!IsValid(Target))
        {
            Print("TargetActor could not be resolved.", true);
            return;
        }

        if (!IsValid(AttachTo))
        {
            Print("Attach-To actor could not be resolved.", true);
            return;
        }

        if (Target == AttachTo)
        {
            Print("Cannot attach an actor to itself.", true);
            return;
        }

        USceneComponent *AttachToRoot = AttachTo->GetRootComponent();
        if (!AttachToRoot)
        {
            Print(FString::Printf(TEXT("%s has no RootComponent to attach to."), *AttachTo->GetName()), true);
            return;
        }

        if (bDisablePhysicsBeforeAttach)
        {
            if (UPrimitiveComponent *TargetPrimitive = Cast<UPrimitiveComponent>(Target->GetRootComponent()))
            {
                TargetPrimitive->SetSimulatePhysics(false);
            }
        }

        const FAttachmentTransformRules AttachRules(LocationRule, RotationRule, ScaleRule, /*bWeldSimulatedBodies*/ true);
        Target->AttachToActor(AttachTo, AttachRules, SocketName);

        LOG("Attached %s to %s%s.", *Target->GetName(), *AttachTo->GetName(), SocketName != NAME_None ? *FString::Printf(TEXT(" (socket: %s)"), *SocketName.ToString()) : TEXT(""));

        Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
    }
};