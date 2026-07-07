#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/CommandDebug.h"
#include "Command_NiagaraSystem.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

UENUM()
enum class ENiagaraSpawnMode : uint8
{
    AttachToActor UMETA(DisplayName = "Attach To Actor"),
    AtLocation UMETA(DisplayName = "At Location (World)")
};

/**
 * Spawns a Niagara System - either attached to a target actor's component/socket,
 * or as a standalone fire-and-forget effect at a world location.
 */
UCLASS(meta = (DisplayName = "Spawn Niagara System"))
class COMMANDCORE_API UCommand_NiagaraSystem : public UCommand
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
    TObjectPtr<UNiagaraSystem> NiagaraSystem;

    UPROPERTY(EditAnywhere, Category = "Command|Spawn")
    ENiagaraSpawnMode SpawnMode = ENiagaraSpawnMode::AttachToActor;

    /// Spawn to Location
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Spawn", meta = (EditCondition = "SpawnMode == ENiagaraSpawnMode::AtLocation", EditConditionHides))
    bool bLocationFromActor = false;
    /// Spawn to Actor
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Spawn", meta = (EditCondition = "SpawnMode == ENiagaraSpawnMode::AttachToActor", EditConditionHides))
    ECommandTargetActor TargetActor = ECommandTargetActor::InstigatorActor;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command", meta = (EditCondition = "TargetActor == ECommandTargetActor::OtherActor || bLocationFromActor && SpawnMode == ENiagaraSpawnMode::AtLocation", EditConditionHides))
    TObjectPtr<AActor> OtherActor;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Spawn", meta = (EditCondition = "SpawnMode == ENiagaraSpawnMode::AtLocation && bLocationFromActor", EditConditionHides))
    bool bDestroySourceActorAfterSpawn = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Spawn", meta = (EditCondition = "SpawnMode == ENiagaraSpawnMode::AttachToActor", EditConditionHides))
    FName AttachSocketName = NAME_None;

    /** Relative offset from the attach point (AttachToActor mode) or the exact world location (AtLocation mode). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Spawn", meta = (MakeEditWidget = "true"))
    FVector LocationOffset = FVector::ZeroVector;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Spawn")
    FRotator RotationOffset = FRotator::ZeroRotator;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Spawn")
    FVector Scale = FVector(1.f, 1.f, 1.f);

    /** If true (AttachToActor mode), the effect is destroyed automatically if the target actor is destroyed. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Spawn", meta = (EditCondition = "SpawnMode == ENiagaraSpawnMode::AttachToActor", EditConditionHides))
    bool bAutoDestroyOnActorDestroyed = true;

    /** Cached reference to the last spawned Niagara component. */
    UPROPERTY(Transient)
    TObjectPtr<UNiagaraComponent> SpawnedNiagaraComponent;

protected:
    FTransform ResolveSpawnTransform() const
    {
        FTransform BaseTransform = FTransform::Identity;
        const FTransform OffsetTransform(RotationOffset, LocationOffset, Scale);

        if (bLocationFromActor)
        {
            if (IsValid(OtherActor))
            {
                BaseTransform = OtherActor->GetActorTransform();
            }
        }

        return BaseTransform * OffsetTransform;
    }
    virtual AActor *K2_GetOtherActor_Implementation() const
    {
        return OtherActor;
    }
    virtual bool K2_CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const override
    {
        if (!NiagaraSystem)
        {
            return false;
        }

        AActor *Target = ResolveTargetActor(OwnerActor, InstigatorActor, TargetActor);
        if (SpawnMode == ENiagaraSpawnMode::AttachToActor)
        {
            if (!IsValid(Target) || !Target->GetRootComponent())
            {
                return false;
            }
        }
        else
        {
            if (bLocationFromActor && !IsValid(Target))
            {
                return false;
            }
        }

        return Super::K2_CanExecute_Implementation(OwnerActor, InstigatorActor);
    }
    virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
    {
        if (SpawnMode == ENiagaraSpawnMode::AttachToActor)
        {
            AActor *Target = ResolveTargetActor(OwnerActor, InstigatorActor, TargetActor);
            USceneComponent *AttachComponent = Target ? Target->GetRootComponent() : nullptr;

            if (!AttachComponent)
            {
                Print("Target actor has no RootComponent to attach the Niagara System to.", true);
                return;
            }

            SpawnedNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
                NiagaraSystem,
                AttachComponent,
                AttachSocketName,
                LocationOffset,
                RotationOffset,
                Scale,
                EAttachLocation::KeepRelativeOffset,
                bAutoDestroyOnActorDestroyed,
                ENCPoolMethod::None,
                /*bPreCullCheck*/ true);
        }
        else
        {
            UWorld *World = OwnerActor ? OwnerActor->GetWorld() : (InstigatorActor ? InstigatorActor->GetWorld() : nullptr);
            if (!World)
            {
                Print(TEXT("Could not resolve a valid World to spawn the Niagara System in."), true);
                return;
            }

            const FTransform ResolvedTransform = ResolveSpawnTransform();

            SpawnedNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                World,
                NiagaraSystem,
                ResolvedTransform.GetLocation(),
                ResolvedTransform.Rotator(),
                ResolvedTransform.GetScale3D(),
                /*bAutoDestroy*/ true,
                /*bAutoActivate*/ true,
                ENCPoolMethod::None,
                /*bPreCullCheck*/ true);

            if (bLocationFromActor && bDestroySourceActorAfterSpawn)
            {
                if (IsValid(OtherActor))
                {
                    OtherActor->Destroy();
                    LOG("Destroyed other actor %s after spawn.", *OtherActor->GetName());
                }
            }
        }

        if (!SpawnedNiagaraComponent)
        {
            Print(FString::Printf(TEXT("Failed to spawn Niagara System %s."), *NiagaraSystem->GetName()), true);
            return;
        }

        LOG("Spawned Niagara System %s.", *NiagaraSystem->GetName());

        Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
    }
};