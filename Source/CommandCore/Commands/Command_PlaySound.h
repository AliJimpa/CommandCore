#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Command_PlaySound.generated.h"

class USoundBase;

/** Plays a sound - either at the trigger's location in 3D, or as a fire-and-forget 2D sound. */
UCLASS(meta = (DisplayName = "Play Sound"))
class COMMANDCORE_API UCommand_PlaySound : public UCommand
{
	GENERATED_BODY()

public:
	virtual void Execute_Implementation(AActor* TriggerActor, AActor* OtherActor) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
	TObjectPtr<USoundBase> Sound;

	/** If true, plays as a 3D sound at the trigger's location. If false, plays as a 2D sound. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
	bool bPlayAtLocation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command", meta = (ClampMin = "0.0"))
	float VolumeMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command", meta = (ClampMin = "0.0"))
	float PitchMultiplier = 1.f;
};
