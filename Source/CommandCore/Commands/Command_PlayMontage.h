#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Engine/CommandDebug.h"
#include "Command_PlayMontage.generated.h"

class UAnimMontage;
class USkeletalMeshComponent;

/**
 * Plays an Animation Montage on the target actor's SkeletalMeshComponent.
 *
 * Finds the first USkeletalMeshComponent on the target actor and plays the
 * Montage through its AnimInstance. Optionally starts from a specific section
 * and/or stops any currently playing montage first.
 */
UCLASS(meta = (DisplayName = "Play Montage"))
class COMMANDCORE_API UCommand_PlayMontage : public UCommand
{
	GENERATED_BODY()

protected:
	/** Which actor's SkeletalMeshComponent this command should play the montage on. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Montage")
	ECommandTargetActor TargetActor = ECommandTargetActor::InstigatorActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Montage", meta = (EditCondition = "TargetActor == ECommandTargetActor::OtherActor", EditConditionHides, AllowPrivateAccess = "true"))
	TObjectPtr<AActor> OtherActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Montage")
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Montage", meta = (ClampMin = "0.01"))
	float PlayRate = 1.0f;

	/** If set, starts the montage from this section instead of the beginning. Leave "None" to play from the start. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Montage")
	FName StartSectionName = NAME_None;

	/** If true, stops any currently playing montage on this AnimInstance before playing this one. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Montage")
	bool bStopAllMontagesFirst = true;

	/** Blend-out time used when stopping a previous montage (only relevant if bStopAllMontagesFirst is true). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Montage", meta = (EditCondition = "bStopAllMontagesFirst", ClampMin = "0.0"))
	float StopBlendOutTime = 0.25f;

private:
	USkeletalMeshComponent *ResolveSkeletalMeshComponent(AActor *OwnerActor, AActor *InstigatorActor) const
	{
		AActor *Target = ResolveTargetActor(OwnerActor, InstigatorActor, TargetActor);
		return Target ? Target->FindComponentByClass<USkeletalMeshComponent>() : nullptr;
	}

protected:
	virtual AActor *K2_GetOtherActor_Implementation() const override
	{
		return OtherActor;
	}
	virtual bool K2_CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const override
	{
		if (!Montage)
		{
			return false;
		}

		USkeletalMeshComponent *SkeletalMeshComp = ResolveSkeletalMeshComponent(OwnerActor, InstigatorActor);
		if (!SkeletalMeshComp || !SkeletalMeshComp->GetAnimInstance())
		{
			return false;
		}

		return Super::K2_CanExecute_Implementation(OwnerActor, InstigatorActor);
	}
	virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
	{
		USkeletalMeshComponent *SkeletalMeshComp = ResolveSkeletalMeshComponent(OwnerActor, InstigatorActor);
		if (!SkeletalMeshComp)
		{
			Print("No SkeletalMeshComponent found on target actor.", true);
			return;
		}

		UAnimInstance *AnimInstance = SkeletalMeshComp->GetAnimInstance();
		if (!AnimInstance)
		{
			Print("Target actor's SkeletalMeshComponent has no valid AnimInstance.", true);
			return;
		}

		if (bStopAllMontagesFirst)
		{
			AnimInstance->StopAllMontages(StopBlendOutTime);
		}

		const float Duration = AnimInstance->Montage_Play(Montage, PlayRate);

		if (Duration <= 0.f)
		{
			Print(FString::Printf(TEXT("Failed to play Montage %s."), *Montage->GetName()), true);
			return;
		}

		if (StartSectionName != NAME_None)
		{
			AnimInstance->Montage_JumpToSection(StartSectionName, Montage);
		}

		LOG("Playing Montage %s.", *Montage->GetName());

		Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
	}
};