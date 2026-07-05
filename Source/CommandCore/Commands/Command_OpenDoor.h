#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Command_OpenDoor.generated.h"

/**
 * Rotates a target actor (typically a door) from its current rotation towards
 * a relative "open" rotation over a set duration.
 *
 * Note: for production-quality door motion you may prefer a Timeline component
 * or an animation on the door actor itself; this command is a simple, dependency-free
 * example of a command that performs a timed action rather than an instant one.
 */
UCLASS(meta = (DisplayName = "Open Door"))
class COMMANDCORE_API UCommand_OpenDoor : public UCommand
{
	GENERATED_BODY()

public:
	virtual void Execute_Implementation(AActor* TriggerActor, AActor* OtherActor) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
	TSoftObjectPtr<AActor> DoorActor;

	/** Rotation added to the door's current rotation to reach the "open" pose. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
	FRotator OpenRotationOffset = FRotator(0.f, 90.f, 0.f);

	/** How long the rotation takes. 0 = snap instantly. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command", meta = (ClampMin = "0.0"))
	float OpenDuration = 1.f;
};
