#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/CommandDebug.h"
#include "Command_CallEvent.generated.h"

UENUM(BlueprintType)
enum class EEventTargetMode : uint8
{
    SpecificActor UMETA(DisplayName = "Owner/Instigator/Other Actor"),
    FindByClass UMETA(DisplayName = "Find Actor(s) By Class In Scene")
};

/**
 * Calls a named Event/Function on a target actor (or all actors of a given
 * class found in the scene) via reflection.
 *
 * The target Blueprint/C++ class must implement a matching UFUNCTION
 * (e.g. a Custom Event in Blueprint, or a UFUNCTION() in C++) with the
 * exact EventName and no required parameters, or the call will silently fail.
 */
UCLASS(meta = (DisplayName = "Call Event"))
class COMMANDCORE_API UCommand_CallEvent : public UCommand
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
    EEventTargetMode TargetMode = EEventTargetMode::SpecificActor;

    /** Which actor to call the event on. Only used when TargetMode is SpecificActor. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command",
              meta = (EditCondition = "TargetMode == EEventTargetMode::SpecificActor", EditConditionHides))
    ECommandTargetActor TargetActor = ECommandTargetActor::InstigatorActor;

    /** Only used when TargetActor is "Other Actor". */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command",
              meta = (EditCondition = "TargetMode == EEventTargetMode::SpecificActor && TargetActor == ECommandTargetActor::OtherActor", EditConditionHides))
    TSoftObjectPtr<AActor> OtherActor;

    /** Actor class to search for in the scene. Only used when TargetMode is FindByClass. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command",
              meta = (EditCondition = "TargetMode == EEventTargetMode::FindByClass", EditConditionHides))
    TSubclassOf<AActor> ActorClass;

    /** Name of the Event/Function to call (must exist as a UFUNCTION with no parameters on the target). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Event")
    FName EventName;

    /** If true, calls the event with a single bool parameter (BoolParamValue). The target function must have exactly one bool input parameter. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Event")
    bool bPassBoolParameter = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Event",
              meta = (EditCondition = "bPassBoolParameter"))
    bool BoolParamValue = false;

private:
    void CallEventOnActor(AActor *Target) const
    {
        if (!Target)
        {
            return;
        }

        UFunction *Function = Target->FindFunction(EventName);
        if (!Function)
        {
            LOG_ERROR("%s has no Event/Function named '%s'.", *Target->GetName(), *EventName.ToString());
            return;
        }

        if (bPassBoolParameter)
        {
            // Verify the function actually expects exactly one bool parameter before calling.
            FProperty *FirstParam = nullptr;
            int32 ParamCount = 0;

            for (TFieldIterator<FProperty> It(Function); It && (It->PropertyFlags & CPF_Parm); ++It)
            {
                if (!(It->PropertyFlags & CPF_ReturnParm))
                {
                    FirstParam = *It;
                    ParamCount++;
                }
            }

            FBoolProperty *BoolParam = CastField<FBoolProperty>(FirstParam);
            if (ParamCount != 1 || !BoolParam)
            {
                LOG_ERROR("Event '%s' on %s does not take exactly one bool parameter.", *EventName.ToString(), *Target->GetName());
                return;
            }

            // Allocate a buffer matching the function's parameter layout and set the bool value into it.
            uint8 *Params = (uint8 *)FMemory_Alloca(Function->ParmsSize);
            FMemory::Memzero(Params, Function->ParmsSize);
            BoolParam->SetPropertyValue_InContainer(Params, BoolParamValue);

            Target->ProcessEvent(Function, Params);
        }
        else
        {
            Target->ProcessEvent(Function, nullptr);
        }
    }

protected:
    virtual AActor *K2_GetOtherActor_Implementation() const
    {
        return OtherActor.LoadSynchronous();
    }
    virtual bool K2_CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const override
    {
        if (EventName == NAME_None)
        {
            return false;
        }

        if (TargetMode == EEventTargetMode::SpecificActor)
        {
            if (TargetActor == ECommandTargetActor::OtherActor && OtherActor.IsNull())
            {
                return false;
            }
        }
        else // FindByClass
        {
            if (!ActorClass)
            {
                return false;
            }
        }

        return Super::K2_CanExecute_Implementation(OwnerActor, InstigatorActor);
    }
    virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
    {
        if (TargetMode == EEventTargetMode::SpecificActor)
        {
            AActor *Target = ResolveTargetActor(OwnerActor, InstigatorActor, TargetActor);
            if (!IsValid(Target))
            {
                Print(TEXT("Target actor could not be resolved."), true);
                return;
            }

            CallEventOnActor(Target);
            Print(FString::Printf(TEXT("Called event '%s' on %s."), *EventName.ToString(), *Target->GetName()), false);
        }
        else // FindByClass
        {
            UWorld *World = OwnerActor ? OwnerActor->GetWorld() : (InstigatorActor ? InstigatorActor->GetWorld() : nullptr);
            if (!World)
            {
                Print(TEXT("Could not resolve a valid World to search in."), true);
                return;
            }

            TArray<AActor *> FoundActors;
            UGameplayStatics::GetAllActorsOfClass(World, ActorClass, FoundActors);

            for (AActor *Target : FoundActors)
            {
                CallEventOnActor(Target);
            }

            LOG("Called event '%s' on %d actor(s) of class %s.", *EventName.ToString(), FoundActors.Num(), *ActorClass->GetName());
        }

        Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
    }
};