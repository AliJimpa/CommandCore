#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/CommandDebug.h"
#include "Command_ModifyBlackboard.generated.h"

UENUM()
enum class EBlackboardValueType : uint8
{
    Bool UMETA(DisplayName = "Bool"),
    Int UMETA(DisplayName = "Int"),
    Float UMETA(DisplayName = "Float"),
    String UMETA(DisplayName = "String"),
    Name UMETA(DisplayName = "Name"),
    Vector UMETA(DisplayName = "Vector"),
    Object UMETA(DisplayName = "Object (Actor)")
};

/** Single Blackboard key/value entry to set. */
USTRUCT(BlueprintType)
struct FBlackboardEntryCommand
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
    FName KeyName = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
    EBlackboardValueType ValueType = EBlackboardValueType::Bool;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta = (EditCondition = "ValueType == EBlackboardValueType::Bool", EditConditionHides))
    bool BoolValue = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta = (EditCondition = "ValueType == EBlackboardValueType::Int", EditConditionHides))
    int32 IntValue = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta = (EditCondition = "ValueType == EBlackboardValueType::Float", EditConditionHides))
    float FloatValue = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta = (EditCondition = "ValueType == EBlackboardValueType::String", EditConditionHides))
    FString StringValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta = (EditCondition = "ValueType == EBlackboardValueType::Name", EditConditionHides))
    FName NameValue = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta = (EditCondition = "ValueType == EBlackboardValueType::Vector", EditConditionHides, MakeEditWidget = "true"))
    FVector VectorValue = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta = (EditCondition = "ValueType == EBlackboardValueType::Object", EditConditionHides))
    TSoftObjectPtr<AActor> ObjectValue;
};

/**
 * Sets one or more Blackboard key values on the target actor's AI Controller.
 *
 * Resolves the BlackboardComponent from the target actor's AIController
 * (works on Pawns possessed by an AIController, or directly on an AIController actor).
 */
UCLASS(meta = (DisplayName = "Modify Blackboard"))
class COMMANDCORE_API UCommand_ModifyBlackboard : public UCommand
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
    ECommandTargetActor TargetActor = ECommandTargetActor::InstigatorActor;

    /** Only used when TargetActor is "Other Actor". */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command",
              meta = (EditCondition = "TargetActor == ECommandTargetActor::OtherActor", EditConditionHides))
    TSoftObjectPtr<AActor> OtherActor;

    /** Blackboard key/value pairs to set. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Blackboard")
    TArray<FBlackboardEntryCommand> Entries;

private:
    UBlackboardComponent *ResolveBlackboard(AActor *Target) const
    {
        if (!Target)
        {
            return nullptr;
        }

        AAIController *AIController = nullptr;

        if (APawn *Pawn = Cast<APawn>(Target))
        {
            AIController = Cast<AAIController>(Pawn->GetController());
        }
        else if (AAIController *DirectController = Cast<AAIController>(Target))
        {
            AIController = DirectController;
        }

        return AIController ? AIController->GetBlackboardComponent() : nullptr;
    }
    void ApplyEntry(UBlackboardComponent *Blackboard, const FBlackboardEntryCommand &Entry) const
    {
        if (Entry.KeyName == NAME_None)
        {
            return;
        }

        switch (Entry.ValueType)
        {
        case EBlackboardValueType::Bool:
            Blackboard->SetValueAsBool(Entry.KeyName, Entry.BoolValue);
            break;

        case EBlackboardValueType::Int:
            Blackboard->SetValueAsInt(Entry.KeyName, Entry.IntValue);
            break;

        case EBlackboardValueType::Float:
            Blackboard->SetValueAsFloat(Entry.KeyName, Entry.FloatValue);
            break;

        case EBlackboardValueType::String:
            Blackboard->SetValueAsString(Entry.KeyName, Entry.StringValue);
            break;

        case EBlackboardValueType::Name:
            Blackboard->SetValueAsName(Entry.KeyName, Entry.NameValue);
            break;

        case EBlackboardValueType::Vector:
            Blackboard->SetValueAsVector(Entry.KeyName, Entry.VectorValue);
            break;

        case EBlackboardValueType::Object:
            Blackboard->SetValueAsObject(Entry.KeyName, Entry.ObjectValue.LoadSynchronous());
            break;
        }
    }

protected:
    virtual AActor *K2_GetOtherActor_Implementation() const
    {
        return OtherActor.LoadSynchronous();
    }
    virtual bool K2_CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const override
    {
        if (Entries.Num() == 0)
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

        UBlackboardComponent *Blackboard = ResolveBlackboard(Target);
        if (!Blackboard)
        {
            Print(FString::Printf(TEXT("%s has no accessible BlackboardComponent (needs an AIController)."), *Target->GetName()), true);
            return;
        }

        for (const FBlackboardEntryCommand &Entry : Entries)
        {
            ApplyEntry(Blackboard, Entry);
        }

        LOG("Applied %d Blackboard entrie(s) on %s.", Entries.Num(), *Target->GetName());

        Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
    }
};