#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Engine/CommandDebug.h"
#include "Command_DestroyComponent.generated.h"

/**
 * Destroys a specific component from a target actor.
 *
 * Searches the target actor for components of the given class. If more than
 * one component of that class exists, the Tag is used to pick the correct one.
 * If only one component of that class exists, the Tag is ignored (optional).
 */
UCLASS(meta = (DisplayName = "Destroy Component"))
class COMMANDCORE_API UCommand_DestroyComponent : public UCommand
{
    GENERATED_BODY()

protected:
    /** Which actor to search for the component on. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Destroy")
    ECommandTargetActor TargetActor = ECommandTargetActor::InstigatorActor;

    /** Only used when TargetActor is "Other Actor". */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Destroy", meta = (EditCondition = "TargetActor == ECommandTargetActor::OtherActor", EditConditionHides))
    TSoftObjectPtr<AActor> OtherActor;

    /** Component class to search for. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Destroy")
    TSubclassOf<UActorComponent> ComponentClass;

    /**
     * Tag used to disambiguate between multiple components of ComponentClass.
     * Only needed if the target actor has more than one component of this type;
     * ignored (and unnecessary) if there's only one match.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Destroy")
    FName ComponentTag = NAME_None;

private:
    virtual AActor *K2_GetOtherActor_Implementation() const
    {
        return OtherActor.LoadSynchronous();
    }
    UActorComponent *ResolveComponent(AActor *OwnerActor, AActor *InstigatorActor) const
    {
        AActor *Target = ResolveTargetActor(OwnerActor, InstigatorActor, TargetActor);
        if (!Target || !ComponentClass)
        {
            return nullptr;
        }

        TArray<UActorComponent *> FoundComponents;
        Target->GetComponents(ComponentClass, FoundComponents);

        if (FoundComponents.Num() == 0)
        {
            return nullptr;
        }

        // Only one match - tag is irrelevant, just return it.
        if (FoundComponents.Num() == 1)
        {
            return FoundComponents[0];
        }

        // Multiple matches - use tag to disambiguate.
        if (ComponentTag != NAME_None)
        {
            for (UActorComponent *Comp : FoundComponents)
            {
                if (Comp && Comp->ComponentHasTag(ComponentTag))
                {
                    return Comp;
                }
            }
        }

        // Multiple matches but no tag matched (or no tag set) - ambiguous, fail safely.
        return nullptr;
    }

protected:
    virtual bool K2_CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const override
    {
        if (!ComponentClass)
        {
            return false;
        }

        if (TargetActor == ECommandTargetActor::OtherActor && OtherActor.IsNull())
        {
            return false;
        }

        return Super::K2_CanExecute_Implementation(OwnerActor, InstigatorActor);
    }
    virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
    {
        UActorComponent *Component = ResolveComponent(OwnerActor, InstigatorActor);

        if (!Component)
        {
            Print(FString::Printf(TEXT("Could not resolve a unique %s component (check ComponentTag if multiple exist)."), *ComponentClass->GetName()), true);
            return;
        }

        const FString ComponentName = Component->GetName();
        Component->DestroyComponent();

        LOG("Destroyed component '%s'.", *ComponentName);

        Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
    }
};