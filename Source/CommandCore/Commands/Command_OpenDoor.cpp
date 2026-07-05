#include "Commands/Command_OpenDoor.h"
#include "TimerManager.h"
#include "Engine/World.h"

void UCommand_OpenDoor::Execute_Implementation(AActor* TriggerActor, AActor* OtherActor)
{
	AActor* Door = DoorActor.LoadSynchronous();
	if (!Door || !TriggerActor)
	{
		return;
	}

	UWorld* World = TriggerActor->GetWorld();
	if (!World)
	{
		return;
	}

	if (OpenDuration <= 0.f)
	{
		Door->SetActorRotation(Door->GetActorRotation() + OpenRotationOffset);
		return;
	}

	const FRotator StartRotation = Door->GetActorRotation();
	const FRotator TargetRotation = StartRotation + OpenRotationOffset;
	const float Duration = OpenDuration;

	// Small self-contained "tween": a repeating timer that lerps the rotation
	// each tick and clears itself once finished. Captured state lives in shared
	// pointers so the lambda stays valid for the lifetime of the timer.
	TSharedPtr<float> Elapsed = MakeShared<float>(0.f);
	TSharedPtr<FTimerHandle> TimerHandle = MakeShared<FTimerHandle>();
	TWeakObjectPtr<AActor> WeakDoor(Door);
	TWeakObjectPtr<UWorld> WeakWorld(World);

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([WeakDoor, WeakWorld, StartRotation, TargetRotation, Elapsed, TimerHandle, Duration]()
	{
		UWorld* WorldPtr = WeakWorld.Get();
		AActor* DoorPtr = WeakDoor.Get();

		if (!WorldPtr || !DoorPtr)
		{
			if (WorldPtr && TimerHandle.IsValid())
			{
				WorldPtr->GetTimerManager().ClearTimer(*TimerHandle);
			}
			return;
		}

		*Elapsed += WorldPtr->GetDeltaSeconds();
		const float Alpha = FMath::Clamp(*Elapsed / Duration, 0.f, 1.f);
		DoorPtr->SetActorRotation(FMath::Lerp(StartRotation, TargetRotation, Alpha));

		if (Alpha >= 1.f)
		{
			WorldPtr->GetTimerManager().ClearTimer(*TimerHandle);
		}
	});

	World->GetTimerManager().SetTimer(*TimerHandle, TimerDelegate, 0.016f, true);
}
