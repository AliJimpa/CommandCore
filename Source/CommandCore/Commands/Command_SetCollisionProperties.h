#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Engine/CommandDebug.h"
#include "Components/PrimitiveComponent.h"
#include "Command_SetCollisionProperties.generated.h"


/**
 * Sets common collision properties on a list of actors placed in the scene.
 *
 * Applies to every UPrimitiveComponent found on each target actor (e.g. a
 * character's capsule + mesh, or a static mesh actor's single component).
 * Each property has its own override toggle - only checked properties are
 * applied, everything else is left untouched.
 */
UCLASS(meta = (DisplayName = "Set Collision Properties"))
class COMMANDCORE_API UCommand_SetCollisionProperties : public UCommand
{
    GENERATED_BODY()

protected:
    /** Actors placed in the level to apply these collision settings to. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command")
    TArray<TSoftObjectPtr<AActor>> TargetActors;

    // --- Collision Enabled ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Collision", meta = (InlineEditConditionToggle))
    bool bOverrideCollisionEnabled = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Collision", meta = (EditCondition = "bOverrideCollisionEnabled"))
    TEnumAsByte<ECollisionEnabled::Type> CollisionEnabled = ECollisionEnabled::QueryAndPhysics;

    // --- Collision Profile Name ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Collision", meta = (InlineEditConditionToggle))
    bool bOverrideCollisionProfile = false;

    /** e.g. "BlockAll", "OverlapAll", "Pawn", "Trigger", "NoCollision", or any custom profile defined in project settings. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Collision", meta = (EditCondition = "bOverrideCollisionProfile"))
    FName CollisionProfileName = NAME_None;

    // --- Generate Overlap Events ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Collision", meta = (InlineEditConditionToggle))
    bool bOverrideGenerateOverlapEvents = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Collision", meta = (EditCondition = "bOverrideGenerateOverlapEvents"))
    bool bGenerateOverlapEvents = true;

    // --- Simulate Physics ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Collision", meta = (InlineEditConditionToggle))
    bool bOverrideSimulatePhysics = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Collision", meta = (EditCondition = "bOverrideSimulatePhysics"))
    bool bSimulatePhysics = false;

    // --- Collision Response To All Channels ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Collision", meta = (InlineEditConditionToggle))
    bool bOverrideCollisionResponseToAll = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Collision", meta = (EditCondition = "bOverrideCollisionResponseToAll"))
    TEnumAsByte<ECollisionResponse> CollisionResponseToAll = ECR_Block;

    // --- Notify Rigid Body Collision (hit events) ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Collision", meta = (InlineEditConditionToggle))
    bool bOverrideNotifyRigidBodyCollision = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Collision", meta = (EditCondition = "bOverrideNotifyRigidBodyCollision"))
    bool bNotifyRigidBodyCollision = false;

private:
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
    void ApplyToComponent(UPrimitiveComponent *Component) const
    {
        if (!Component)
        {
            return;
        }

        // Collision Profile applied first - it can reset individual response channels,
        // so subsequent individual overrides below take precedence over the profile if both are set.
        if (bOverrideCollisionProfile && CollisionProfileName != NAME_None)
        {
            Component->SetCollisionProfileName(CollisionProfileName);
        }

        if (bOverrideCollisionEnabled)
        {
            Component->SetCollisionEnabled(CollisionEnabled);
        }

        if (bOverrideCollisionResponseToAll)
        {
            Component->SetCollisionResponseToAllChannels(CollisionResponseToAll);
        }

        if (bOverrideGenerateOverlapEvents)
        {
            Component->SetGenerateOverlapEvents(bGenerateOverlapEvents);
        }

        if (bOverrideSimulatePhysics)
        {
            Component->SetSimulatePhysics(bSimulatePhysics);
        }

        if (bOverrideNotifyRigidBodyCollision)
        {
            Component->SetNotifyRigidBodyCollision(bNotifyRigidBodyCollision);
        }
    }

protected:
    virtual bool K2_CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const override
    {
        if (TargetActors.Num() == 0)
        {
            return false;
        }

        const bool bHasAnyOverride = bOverrideCollisionEnabled || bOverrideCollisionProfile || bOverrideGenerateOverlapEvents || bOverrideSimulatePhysics || bOverrideCollisionResponseToAll || bOverrideNotifyRigidBodyCollision;

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

        LOG("Applied collision property overrides to %d actor(s).", AppliedCount);

        Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
    }
};