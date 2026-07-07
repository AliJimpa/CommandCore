#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Command_ToggleVisibility.generated.h"

/** Shows or hides a target actor, optionally toggling its collision along with it. */
UCLASS(meta = (DisplayName = "Toggle Visibility"))
class COMMANDCORE_API UCommand_ToggleVisibility : public UCommand
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command")
	TArray<TSoftObjectPtr<AActor>> TargetActors;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command")
	bool bAlsoToggleCollision = true;

protected:
	virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
	{
		for (const TSoftObjectPtr<AActor> &SoftActor : TargetActors)
		{
			if (AActor *Actor = SoftActor.LoadSynchronous())
			{
				const bool result = !Actor->IsHidden();
				Actor->SetActorHiddenInGame(result);
				if (bAlsoToggleCollision)
				{
					Actor->SetActorEnableCollision(result);
				}
			}
		}
	}
};
