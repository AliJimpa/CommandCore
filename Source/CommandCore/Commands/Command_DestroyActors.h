#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Command_DestroyActors.generated.h"

/** Destroys a list of actors placed in the scene. */
UCLASS(meta = (DisplayName = "Destroy Actors"))
class COMMANDCORE_API UCommand_DestroyActors : public UCommand
{
    GENERATED_BODY()

protected:
    /** Actors placed in the level to destroy. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
    TArray<TSoftObjectPtr<AActor>> TargetActors;

    /** If true, OwnerActor is also destroyed. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Setting")
    bool bIncludeOwnerActor = false;

    /** If true, InstigatorActor is also destroyed. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Setting")
    bool bIncludeInstigatorActor = false;

private:
    TArray<AActor *> ResolveTargets(AActor *OwnerActor, AActor *InstigatorActor) const
    {
        TArray<AActor *> Result;
        Result.Reserve(TargetActors.Num() + 2);

        for (const TSoftObjectPtr<AActor> &SoftActor : TargetActors)
        {
            if (AActor *Actor = SoftActor.LoadSynchronous())
            {
                Result.AddUnique(Actor);
            }
        }

        if (bIncludeOwnerActor && OwnerActor)
        {
            Result.AddUnique(OwnerActor);
        }

        if (bIncludeInstigatorActor && InstigatorActor)
        {
            Result.AddUnique(InstigatorActor);
        }

        return Result;
    }

protected:
    virtual bool K2_CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const override
    {
        const bool bHasAnyTarget = TargetActors.Num() > 0 || bIncludeOwnerActor || bIncludeInstigatorActor;
        if (!bHasAnyTarget)
        {
            return false;
        }

        return Super::K2_CanExecute_Implementation(OwnerActor, InstigatorActor);
    }
    virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
    {
        TArray<AActor *> Targets = ResolveTargets(OwnerActor, InstigatorActor);

        int32 DestroyedCount = 0;
        for (AActor *Actor : Targets)
        {
            if (IsValid(Actor))
            {
                Actor->Destroy();
                DestroyedCount++;
            }
        }

        Print(FString::Printf(TEXT("Destroyed %d actor(s)."), DestroyedCount), false);

        Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
    }
};