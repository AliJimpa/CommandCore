#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Engine/CommandDebug.h"
#include "Command_SpawnActor.generated.h"

UENUM(BlueprintType)
enum class ESpawnTransformMode : uint8
{
    Manual UMETA(DisplayName = "Manual"),
    FromActor UMETA(DisplayName = "From Actor")
};

/**
 * Spawns an actor of the given class, using either a manually specified
 * Transform, or a Transform copied from a reference actor placed in the scene.
 *
 * When using "From Actor" mode, optionally destroys that reference actor
 * right after the new actor is spawned (useful for spawn-point markers
 * that shouldn't persist in the level).
 */
UCLASS(meta = (DisplayName = "Spawn Actor"))
class COMMANDCORE_API UCommand_SpawnActor : public UCommand
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
    TSubclassOf<AActor> ActorClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Transform")
    ESpawnTransformMode TransformMode = ESpawnTransformMode::Manual;

    /** Used when TransformMode is Manual. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Transform",
              meta = (EditCondition = "TransformMode == ESpawnTransformMode::Manual", EditConditionHides, MakeEditWidget = "true"))
    FTransform SpawnTransform = FTransform::Identity;

    /** Used when TransformMode is FromActor - the spawned actor's Transform is copied from this actor. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Transform",
              meta = (EditCondition = "TransformMode == ESpawnTransformMode::FromActor", EditConditionHides))
    TSoftObjectPtr<AActor> TransformSourceActor;

    /** If true (FromActor mode only), destroys TransformSourceActor right after spawning. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Transform",
              meta = (EditCondition = "TransformMode == ESpawnTransformMode::FromActor", EditConditionHides))
    bool bDestroySourceActorAfterSpawn = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Spawn")
    ESpawnActorCollisionHandlingMethod CollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    /** If true, sets the spawned actor's Owner to OwnerActor (the actor holding this command). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Spawn")
    bool bSetOwnerToCommandOwner = false;

    /** Cached reference to the last actor spawned by this command instance. */
    UPROPERTY(Transient, BlueprintReadOnly, Category = "Command|Spawn")
    TObjectPtr<AActor> SpawnedActor;

private:
    FTransform ResolveSpawnTransform() const
    {
        if (TransformMode == ESpawnTransformMode::FromActor)
        {
            if (AActor *SourceActor = TransformSourceActor.LoadSynchronous())
            {
                return SourceActor->GetActorTransform();
            }
            return FTransform::Identity;
        }

        return SpawnTransform;
    }

protected:
    virtual bool K2_CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const override
    {
        if (!ActorClass)
        {
            return false;
        }

        if (TransformMode == ESpawnTransformMode::FromActor && TransformSourceActor.IsNull())
        {
            return false;
        }

        return Super::K2_CanExecute_Implementation(OwnerActor, InstigatorActor);
    }
    virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
    {
        UWorld *World = OwnerActor ? OwnerActor->GetWorld() : (InstigatorActor ? InstigatorActor->GetWorld() : nullptr);
        if (!World)
        {
            Print(TEXT("Could not resolve a valid World to spawn in."), true);
            return;
        }

        const FTransform SpawnAt = ResolveSpawnTransform();

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = CollisionHandlingMethod;

        if (bSetOwnerToCommandOwner)
        {
            SpawnParams.Owner = OwnerActor;
        }

        AActor *NewActor = World->SpawnActor<AActor>(ActorClass, SpawnAt, SpawnParams);

        if (!NewActor)
        {
            Print(FString::Printf(TEXT("Failed to spawn actor of class %s."), *ActorClass->GetName()), true);
            return;
        }

        SpawnedActor = NewActor;

        LOG("Spawned %s at %s.", *NewActor->GetName(), *SpawnAt.GetLocation().ToString());

        if (TransformMode == ESpawnTransformMode::FromActor && bDestroySourceActorAfterSpawn)
        {
            if (AActor *SourceActor = TransformSourceActor.LoadSynchronous())
            {
                if (IsValid(SourceActor))
                {
                    SourceActor->Destroy();
                    LOG("Destroyed source actor %s after spawn.", *SourceActor->GetName());
                }
            }
        }

        Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
    }
};