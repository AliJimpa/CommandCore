#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Engine/CommandDebug.h"
#include "Components/PrimitiveComponent.h"
#include "Command_SetPhysicsProperties.generated.h"

class UPrimitiveComponent;

/**
 * Sets common physics properties on a list of actors placed in the scene.
 *
 * Applies to every UPrimitiveComponent found on each target actor.
 * Each property has its own override toggle - only checked properties are
 * applied, everything else is left untouched.
 */
UCLASS(meta = (DisplayName = "Set Physics Properties"))
class COMMANDCORE_API UCommand_SetPhysicsProperties : public UCommand
{
    GENERATED_BODY()

protected:
    /** Actors placed in the level to apply these physics settings to. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command")
    TArray<TSoftObjectPtr<AActor>> TargetActors;

    // --- Simulate Physics ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Physics", meta = (InlineEditConditionToggle))
    bool bOverrideSimulatePhysics = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Physics", meta = (EditCondition = "bOverrideSimulatePhysics"))
    bool bSimulatePhysics = true;

    // --- Enable Gravity ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Physics", meta = (InlineEditConditionToggle))
    bool bOverrideEnableGravity = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Physics", meta = (EditCondition = "bOverrideEnableGravity"))
    bool bEnableGravity = true;

    // --- Mass ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Physics", meta = (InlineEditConditionToggle))
    bool bOverrideMass = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Physics", meta = (EditCondition = "bOverrideMass", ClampMin = "0.001"))
    float MassInKg = 100.0f;

    // --- Linear Damping ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Physics", meta = (InlineEditConditionToggle))
    bool bOverrideLinearDamping = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Physics", meta = (EditCondition = "bOverrideLinearDamping", ClampMin = "0.0"))
    float LinearDamping = 0.01f;

    // --- Angular Damping ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Physics", meta = (InlineEditConditionToggle))
    bool bOverrideAngularDamping = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Physics", meta = (EditCondition = "bOverrideAngularDamping", ClampMin = "0.0"))
    float AngularDamping = 0.0f;

    // --- Linear Velocity ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Physics", meta = (InlineEditConditionToggle))
    bool bOverrideLinearVelocity = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Physics", meta = (EditCondition = "bOverrideLinearVelocity", MakeEditWidget = "true"))
    FVector LinearVelocity = FVector::ZeroVector;

    // --- Angular Velocity (degrees/sec) ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Physics", meta = (InlineEditConditionToggle))
    bool bOverrideAngularVelocity = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Physics", meta = (EditCondition = "bOverrideAngularVelocity"))
    FVector AngularVelocityDegrees = FVector::ZeroVector;

    // --- Enable Physics Constraints (lock movement/rotation on specific axes) ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Physics", meta = (InlineEditConditionToggle))
    bool bOverrideConstraints = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Physics", meta = (EditCondition = "bOverrideConstraints"))
    bool bLockXRotation = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Physics", meta = (EditCondition = "bOverrideConstraints"))
    bool bLockYRotation = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Physics", meta = (EditCondition = "bOverrideConstraints"))
    bool bLockZRotation = false;

private:
    void ApplyToComponent(UPrimitiveComponent *Component) const
    {
        if (!Component)
        {
            return;
        }

        // Simulate Physics applied first - most other properties below require it to be enabled to take effect.
        if (bOverrideSimulatePhysics)
        {
            Component->SetSimulatePhysics(bSimulatePhysics);
        }

        if (bOverrideEnableGravity)
        {
            Component->SetEnableGravity(bEnableGravity);
        }

        if (bOverrideMass)
        {
            Component->SetMassOverrideInKg(NAME_None, MassInKg, true);
        }

        if (bOverrideLinearDamping)
        {
            Component->SetLinearDamping(LinearDamping);
        }

        if (bOverrideAngularDamping)
        {
            Component->SetAngularDamping(AngularDamping);
        }

        if (bOverrideLinearVelocity)
        {
            Component->SetPhysicsLinearVelocity(LinearVelocity);
        }

        if (bOverrideAngularVelocity)
        {
            Component->SetPhysicsAngularVelocityInDegrees(AngularVelocityDegrees);
        }

        if (bOverrideConstraints)
        {
            Component->SetConstraintMode(EDOFMode::SixDOF);
            Component->BodyInstance.bLockXRotation = bLockXRotation;
            Component->BodyInstance.bLockYRotation = bLockYRotation;
            Component->BodyInstance.bLockZRotation = bLockZRotation;
            //Component->BodyInstance.UpdateDOFMode();
        }
    }
    void ApplyToActor(AActor *Actor) const
    {
        if (!Actor)
        {
            return;
        }

        TArray<UPrimitiveComponent *> PrimitiveComponents;
        Actor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);

        for (UPrimitiveComponent *Component : PrimitiveComponents)
        {
            ApplyToComponent(Component);
        }
    }

protected:
    virtual bool K2_CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const override
    {
        if (TargetActors.Num() == 0)
        {
            return false;
        }

        const bool bHasAnyOverride = bOverrideSimulatePhysics || bOverrideEnableGravity || bOverrideMass || bOverrideLinearDamping || bOverrideAngularDamping || bOverrideLinearVelocity || bOverrideAngularVelocity || bOverrideConstraints;

        if (!bHasAnyOverride)
        {
            return false;
        }

        return Super::K2_CanExecute_Implementation(OwnerActor, InstigatorActor);
    }
    virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
    {
        int32 AppliedCount = 0;

        for (const TSoftObjectPtr<AActor> &SoftActor : TargetActors)
        {
            if (AActor *Actor = SoftActor.LoadSynchronous())
            {
                ApplyToActor(Actor);
                AppliedCount++;
            }
        }

        LOG("Applied physics property overrides to %d actor(s).", AppliedCount);

        Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
    }
};