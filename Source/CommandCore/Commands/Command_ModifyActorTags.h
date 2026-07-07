#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Engine/CommandDebug.h"
#include "Command_ModifyActorTags.generated.h"

/** Adds and/or removes Tags on a target actor. */
UCLASS(meta = (DisplayName = "Modify Actor Tags"))
class COMMANDCORE_API UCommand_ModifyActorTags : public UCommand
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
    ECommandTargetActor TargetActor = ECommandTargetActor::InstigatorActor;

    /** Only used when TargetActor is "Other Actor". */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command",
              meta = (EditCondition = "TargetActor == ECommandTargetActor::OtherActor", EditConditionHides))
    TSoftObjectPtr<AActor> OtherActor;

    /** Tags to add to the target actor. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Tags")
    TArray<FName> TagsToAdd;

    /** Tags to remove from the target actor. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Tags")
    TArray<FName> TagsToRemove;

protected:
    virtual AActor *K2_GetOtherActor_Implementation() const
    {
        return OtherActor.LoadSynchronous();
    }
    virtual bool K2_CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const override
    {
        if (TagsToAdd.Num() == 0 && TagsToRemove.Num() == 0)
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
        AActor *Target = ResolveTargetActor(OwnerActor, InstigatorActor, TargetActor);
        if (!IsValid(Target))
        {
            Print(TEXT("Target actor could not be resolved."), true);
            return;
        }

        int32 AddedCount = 0;
        for (const FName &Tag : TagsToAdd)
        {
            if (!Target->Tags.Contains(Tag))
            {
                Target->Tags.Add(Tag);
                AddedCount++;
            }
        }

        int32 RemovedCount = 0;
        for (const FName &Tag : TagsToRemove)
        {
            if (Target->Tags.RemoveSingle(Tag) > 0)
            {
                RemovedCount++;
            }
        }

        LOG("%s: added %d tag(s), removed %d tag(s).", *Target->GetName(), AddedCount, RemovedCount);

        Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
    }
};