#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Command_ToggleActorVisibility.generated.h"

/** Shows or hides a target actor, optionally toggling its collision along with it. */
UCLASS(meta = (DisplayName = "Toggle Actor Visibility"))
class COMMANDCORE_API UCommand_ToggleActorVisibility : public UCommand
{
	GENERATED_BODY()

public:
	virtual void Execute_Implementation(AActor* TriggerActor, AActor* OtherActor) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
	TSoftObjectPtr<AActor> TargetActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
	bool bNewVisibility = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
	bool bAlsoToggleCollision = true;
};
