#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Engine/CommandDebug.h"
#include "Command_SetActorEnabled.generated.h"

/** Enables/disables ticking and collision on a target actor - useful for gating hazards, doors, pickups, etc. */
UCLASS(meta = (DisplayName = "Set Actor Enabled"))
class COMMANDCORE_API UCommand_SetActorEnabled : public UCommand
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Activation")
	bool bSetActorEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Activation")
	TArray<TSoftObjectPtr<AActor>> TargetActors;

	/** If true, OwnerActor is also included as a target. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Activation")
	bool bIncludeOwnerActor = false;
	/** If true, InstigatorActor is also included as a target. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Activation")
	bool bIncludeInstigatorActor = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Override", meta = (InlineEditConditionToggle))
	bool bOverrideVisibility = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Override", meta = (EditCondition = "bOverrideVisibility"))
	bool bSetVisible = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Override", meta = (InlineEditConditionToggle))
	bool bOverrideCollision = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Override", meta = (EditCondition = "bOverrideCollision"))
	bool bSetCollisionEnabled = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Override", meta = (InlineEditConditionToggle))
	bool bOverrideTick = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Override", meta = (EditCondition = "bOverrideTick"))
	bool bSetTickEnabled = true;

private:
	/** Resolves TargetActors + optional Owner/Instigator into one final list, skipping invalid/unloaded entries. */
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

		const TArray<AActor *> Targets = ResolveTargets(OwnerActor, InstigatorActor);

		if (Targets.Num() == 0)
		{
			Print("No valid target actors resolved.", false);
			return;
		}

		for (AActor *Actor : Targets)
		{
			if (bOverrideVisibility)
			{
				Actor->SetActorHiddenInGame(!bSetVisible);
			}
			else
			{
				Actor->SetActorHiddenInGame(!bSetActorEnabled);
			}

			if (bOverrideCollision)
			{
				Actor->SetActorEnableCollision(bSetCollisionEnabled);
			}
			else
			{
				Actor->SetActorEnableCollision(bSetActorEnabled);
			}

			if (bOverrideTick)
			{
				Actor->SetActorTickEnabled(bSetTickEnabled);
			}
			else
			{
				Actor->SetActorTickEnabled(bSetActorEnabled);
			}

			LOG("Applied state to %s actor.", *Actor->GetName());
		}

		LOG("Applied enabled-state overrides to %d actor(s).", Targets.Num());

		Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
	}
};
