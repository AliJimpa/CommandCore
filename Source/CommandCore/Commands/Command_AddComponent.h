#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Engine/CommandDebug.h"
#include "Command_AddComponent.generated.h"

UENUM(BlueprintType)
enum class ECommandComponentTargetActor : uint8
{
    OwnerActor UMETA(DisplayName = "Owner Actor"),
    InstigatorActor UMETA(DisplayName = "Instigator Actor"),
    OtherActor UMETA(DisplayName = "Other Actor")
};

/** Adds a new component of the given class to the target actor at runtime. */
UCLASS(meta = (DisplayName = "Add Component"))
class COMMANDCORE_API UCommand_AddComponent : public UCommand
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
    ECommandComponentTargetActor TargetActor = ECommandComponentTargetActor::InstigatorActor;

    /** Only used when TargetActor is "Other Actor". */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command",
              meta = (EditCondition = "TargetActor == ECommandComponentTargetActor::OtherActor", EditConditionHides))
    TSoftObjectPtr<AActor> OtherActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Component")
    TSubclassOf<UActorComponent> ComponentClass;

    /** Optional tag applied to the new component, useful for finding/removing it later. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Component")
    FName ComponentTag = NAME_None;

    /** If true, attaches the new component to the target actor's root component (only applies to SceneComponent subclasses). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Component")
    bool bAttachToRoot = true;

private:
    AActor *ResolveTargetActor(AActor *OwnerActor, AActor *InstigatorActor) const
    {
        switch (TargetActor)
        {
        case ECommandComponentTargetActor::OwnerActor:
            return OwnerActor;

        case ECommandComponentTargetActor::InstigatorActor:
            return InstigatorActor;

        case ECommandComponentTargetActor::OtherActor:
            return OtherActor.LoadSynchronous();

        default:
            return nullptr;
        }
    }

protected:
    virtual bool K2_CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const override
    {
        if (!ComponentClass)
        {
            return false;
        }

        if (TargetActor == ECommandComponentTargetActor::OtherActor && OtherActor.IsNull())
        {
            return false;
        }

        return Super::K2_CanExecute_Implementation(OwnerActor, InstigatorActor);
    }
    virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
    {
        AActor *Target = ResolveTargetActor(OwnerActor, InstigatorActor);
        if (!IsValid(Target))
        {
            Print(TEXT("Target actor could not be resolved."), true);
            return;
        }

        UActorComponent *NewComponent = NewObject<UActorComponent>(Target, ComponentClass);
        if (!NewComponent)
        {
            Print(FString::Printf(TEXT("Failed to create component of class %s."), *ComponentClass->GetName()), true);
            return;
        }

        if (ComponentTag != NAME_None)
        {
            NewComponent->ComponentTags.Add(ComponentTag);
        }

        if (bAttachToRoot)
        {
            if (USceneComponent *NewSceneComponent = Cast<USceneComponent>(NewComponent))
            {
                if (USceneComponent *RootComp = Target->GetRootComponent())
                {
                    NewSceneComponent->SetupAttachment(RootComp);
                }
            }
        }

        NewComponent->RegisterComponent();
        Target->AddInstanceComponent(NewComponent);

        LOG("Added %s to %s.", *ComponentClass->GetName(), *Target->GetName());

        Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
    }
};