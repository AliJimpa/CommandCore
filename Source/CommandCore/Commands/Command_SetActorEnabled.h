#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Command_SetActorEnabled.generated.h"

/** Enables/disables ticking and collision on a target actor - useful for gating hazards, doors, pickups, etc. */
UCLASS(meta = (DisplayName = "Set Actor Enabled"))
class COMMANDCORE_API UCommand_SetActorEnabled : public UCommand
{
	GENERATED_BODY()

public:
	virtual void Execute_Implementation(AActor* TriggerActor, AActor* OtherActor) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
	TSoftObjectPtr<AActor> TargetActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
	bool bEnabled = true;
};
