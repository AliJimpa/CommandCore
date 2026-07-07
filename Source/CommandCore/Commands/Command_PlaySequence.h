#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Engine/CommandDebug.h"
#include "LevelSequenceActor.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "Command_PlaySequence.generated.h"

UENUM()
enum class ESequenceSourceMode : uint8
{
	ReferenceActor UMETA(DisplayName = "Reference Sequence Actor"),
	SpawnFromAsset UMETA(DisplayName = "Spawn From Asset")
};

UENUM()
enum class ESequencePlayAction : uint8
{
	Play UMETA(DisplayName = "Play"),
	Pause UMETA(DisplayName = "Pause"),
	Stop UMETA(DisplayName = "Stop"),
	Reverse UMETA(DisplayName = "Play Reverse")
};

/**
 * Plays, pauses, stops, or reverses a Level Sequence via a placed
 * ALevelSequenceActor in the scene.
 *
 * Optionally overrides playback rate and can jump to a specific time
 * (in seconds) before starting playback.
 */
UCLASS(meta = (DisplayName = "Play Sequence"))
class COMMANDCORE_API UCommand_PlaySequence : public UCommand
{
	GENERATED_BODY()

protected:
	/** Whether to control a placed Sequence Actor, or spawn one from an asset at runtime. */
	UPROPERTY(EditAnywhere, Category = "Command")
	ESequenceSourceMode SourceMode = ESequenceSourceMode::ReferenceActor;

	/** The Level Sequence asset to spawn and play. Only used in "Spawn From Asset" mode. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command", meta = (EditCondition = "SourceMode == ESequenceSourceMode::SpawnFromAsset", EditConditionHides))
	TSoftObjectPtr<ULevelSequence> SequenceAsset;

	/** The Level Sequence Actor placed in the scene to control. Only used in "Reference Sequence Actor" mode. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command", meta = (EditCondition = "SourceMode == ESequenceSourceMode::ReferenceActor", EditConditionHides))
	TSoftObjectPtr<ALevelSequenceActor> SequenceActor;

	/** Playback settings used when spawning from an asset. Ignored in ReferenceActor mode (the placed actor keeps its own settings). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Setting", meta = (EditCondition = "SourceMode == ESequenceSourceMode::SpawnFromAsset", EditConditionHides))
	FMovieSceneSequencePlaybackSettings SpawnPlaybackSettings;

	UPROPERTY(EditAnywhere, Category = "Command|Playback")
	ESequencePlayAction PlayAction = ESequencePlayAction::Play;

	/** If true (SpawnFromAsset mode only), destroys the spawned sequence actor automatically when playback finishes. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Setting", meta = (EditCondition = "SourceMode == ESequenceSourceMode::SpawnFromAsset", EditConditionHides))
	bool bAutoDestroyOnFinish = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Playback", meta = (EditCondition = "PlayAction == ESequencePlayAction::Play", EditConditionHides))
	bool bRestartOnPlay = false;

	UPROPERTY(Transient)
	TObjectPtr<ULevelSequencePlayer> CachedPlayer;
	UPROPERTY(Transient)
	TWeakObjectPtr<ALevelSequenceActor> WeakSpawnedActor;

protected:
	UFUNCTION()
	void HandleSequenceFinished()
	{
		if (ALevelSequenceActor *Actor = WeakSpawnedActor.Get())
		{
			Actor->Destroy();
		}

		WeakSpawnedActor = nullptr;
		CachedPlayer = nullptr;
	}
	UFUNCTION(BlueprintNativeEvent, Category = "Command|Override")
	void ApplyPlaybackOverridesAndAction(ULevelSequencePlayer *Player);

	ULevelSequencePlayer *ResolvePlayer(AActor *OwnerActor, AActor *InstigatorActor)
	{
		if (SourceMode == ESequenceSourceMode::ReferenceActor)
		{
			ALevelSequenceActor *SeqActor = SequenceActor.LoadSynchronous();
			if (!SeqActor)
			{
				Print("SequenceActor could not be resolved.", true);
				return nullptr;
			}

			ULevelSequencePlayer *Player = SeqActor->GetSequencePlayer();
			if (!Player)
			{
				Print("SequenceActor has no valid SequencePlayer.", true);
				return nullptr;
			}

			return Player;
		}
		else // SpawnFromAsset
		{
			// Reuse an already-spawned player if this command has already run once and it's still valid.
			if (IsValid(CachedPlayer))
			{
				return CachedPlayer;
			}

			ULevelSequence *Sequence = SequenceAsset.LoadSynchronous();
			if (!Sequence)
			{
				Print("SequenceAsset could not be resolved.", true);
				return nullptr;
			}

			UWorld *World = OwnerActor ? OwnerActor->GetWorld() : (InstigatorActor ? InstigatorActor->GetWorld() : nullptr);
			if (!World)
			{
				Print("Could not resolve a valid World to spawn the sequence in.", true);
				return nullptr;
			}

			ALevelSequenceActor *SpawnedActor = nullptr;
			ULevelSequencePlayer *Player = ULevelSequencePlayer::CreateLevelSequencePlayer(World, Sequence, SpawnPlaybackSettings, SpawnedActor);

			if (!Player)
			{
				Print("Failed to create a LevelSequencePlayer from the given asset.", true);
				return nullptr;
			}

			if (bAutoDestroyOnFinish && SpawnedActor)
			{
				WeakSpawnedActor = SpawnedActor;
				Player->OnFinished.AddDynamic(this, &UCommand_PlaySequence::HandleSequenceFinished);
			}

			CachedPlayer = Player;
			return Player;
		}
	}
	virtual void ApplyPlaybackOverridesAndAction_Implementation(ULevelSequencePlayer *Player)
	{
		switch (PlayAction)
		{
		case ESequencePlayAction::Play:
			if (bRestartOnPlay)
			{
				Player->SetPlaybackPosition(FMovieSceneSequencePlaybackParams(FFrameTime(0), EUpdatePositionMethod::Jump));
			}
			Player->Play();
			break;

		case ESequencePlayAction::Pause:
			Player->Pause();
			break;

		case ESequencePlayAction::Stop:
			Player->Stop();
			break;

		case ESequencePlayAction::Reverse:
			Player->PlayReverse();
			break;
		}
	}
	virtual bool K2_CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const override
	{
		if (SourceMode == ESequenceSourceMode::ReferenceActor)
		{
			if (SequenceActor.IsNull())
			{
				return false;
			}
		}
		else // SpawnFromAsset
		{
			if (SequenceAsset.IsNull())
			{
				return false;
			}
		}

		return Super::K2_CanExecute_Implementation(OwnerActor, InstigatorActor);
	}
	virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
	{
		ULevelSequencePlayer *Player = ResolvePlayer(OwnerActor, InstigatorActor);
		if (!Player)
		{
			return;
		}

		ApplyPlaybackOverridesAndAction(Player);

		LOG("Sequence action: %s", *UEnum::GetValueAsString(PlayAction));

		Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
	}
};