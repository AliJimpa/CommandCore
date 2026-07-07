#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/CommandDebug.h"
#include "Command_PlaySound.generated.h"

class USoundBase;
class UAudioComponent;

UENUM(BlueprintType)
enum class ESoundPlayMode : uint8
{
	Sound2D UMETA(DisplayName = "2D Sound"),
	AtLocation UMETA(DisplayName = "At Location (3D)")
};

/**
 * Plays a sound - either as a fire-and-forget 2D sound, or in 3D at a
 * location sourced from the OwnerActor, InstigatorActor, or a custom
 * world-space point set via the editor gizmo.
 *
 * Optionally spawns the sound as a UAudioComponent instead of a fire-and-forget
 * call, giving you a persistent handle to stop/fade/adjust it later.
 */
UCLASS(meta = (DisplayName = "Play Sound"))
class COMMANDCORE_API UCommand_PlaySound : public UCommand
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command")
	TObjectPtr<USoundBase> Sound;

	/** Stage 1: 2D sound or 3D sound at a location. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command")
	ESoundPlayMode PlayMode = ESoundPlayMode::Sound2D;

	/** Stage 2 (only for AtLocation): where the 3D location comes from. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command", meta = (EditCondition = "PlayMode == ESoundPlayMode::AtLocation", EditConditionHides))
	ECommandTargetActor LocationSource = ECommandTargetActor::OwnerActor;

	/** Custom world-space location, editable via the 3D gizmo in the level viewport. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command", meta = (EditCondition = "PlayMode == ESoundPlayMode::AtLocation && LocationSource == ECommandTargetActor::OtherActor", EditConditionHides, MakeEditWidget = "true"))
	TObjectPtr<AActor> CustomLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Setting", meta = (ClampMin = "0.0"))
	float VolumeMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Setting", meta = (ClampMin = "0.0"))
	float PitchMultiplier = 1.f;

	/** If true, spawns a UAudioComponent instead of a fire-and-forget PlaySound call. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Setting")
	bool bUseAudioComponent = false;

	/** Only for AtLocation + AudioComponent: attaches the spawned component to the source actor instead of a static world location. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Setting",
			  meta = (EditCondition = "bUseAudioComponent && PlayMode == ESoundPlayMode::AtLocation && LocationSource != ESoundLocationSource::CustomLocation", EditConditionHides))
	bool bAttachToActor = false;

	/** Cached handle when bUseAudioComponent is true - useful if a subclass or later command needs to stop/fade it. */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Command|Setting")
	TObjectPtr<UAudioComponent> SpawnedAudioComponent;

private:
	virtual AActor *K2_GetOtherActor_Implementation() const override
	{
		return CustomLocation;
	}

protected:
	virtual bool K2_CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const override
	{
		if (Sound == nullptr)
			return false;
		return Super::K2_CanExecute_Implementation(OwnerActor, InstigatorActor);
	}
	virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
	{
		if (PlayMode == ESoundPlayMode::Sound2D)
		{
			if (bUseAudioComponent)
			{
				SpawnedAudioComponent = UGameplayStatics::SpawnSound2D(OwnerActor, Sound, VolumeMultiplier, PitchMultiplier);
			}
			else
			{
				UGameplayStatics::PlaySound2D(OwnerActor, Sound, VolumeMultiplier, PitchMultiplier);
			}
		}
		else // AtLocation
		{
			AActor *TargetActor = ResolveTargetActor(OwnerActor, InstigatorActor, LocationSource);
			if (TargetActor == nullptr)
			{
				Print("TargetActor is not valid!", true);
				return;
			}

			const FVector Location = TargetActor->GetActorLocation();

			if (bUseAudioComponent)
			{
				if (bAttachToActor)
				{
					if (USceneComponent *RootComp = TargetActor->GetRootComponent())
					{
						SpawnedAudioComponent = UGameplayStatics::SpawnSoundAttached(
							Sound, RootComp, NAME_None, FVector::ZeroVector, EAttachLocation::KeepRelativeOffset,
							/*bStopWhenAttachedToDestroyed*/ false, VolumeMultiplier, PitchMultiplier);
					}
				}
				else
				{
					SpawnedAudioComponent = UGameplayStatics::SpawnSoundAtLocation(
						OwnerActor, Sound, Location, FRotator::ZeroRotator, VolumeMultiplier, PitchMultiplier);
				}
			}
			else
			{
				UGameplayStatics::PlaySoundAtLocation(OwnerActor, Sound, Location, VolumeMultiplier, PitchMultiplier);
			}
		}

		LOG("Played sound %s (%s).", *Sound->GetName(), PlayMode == ESoundPlayMode::Sound2D ? TEXT("2D") : TEXT("At Location"));
		Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
	}
};