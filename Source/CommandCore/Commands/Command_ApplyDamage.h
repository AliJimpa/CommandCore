#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Command_ApplyDamage.generated.h"

class UDamageType;

/** Applies damage to whichever actor overlapped the trigger - e.g. a lava, spikes, or fire volume. */
UCLASS(meta = (DisplayName = "Apply Damage To Other Actor"))
class COMMANDCORE_API UCommand_ApplyDamage : public UCommand
{
	GENERATED_BODY()

public:
	virtual void Execute_Implementation(AActor* TriggerActor, AActor* OtherActor) override;
	virtual bool CanExecute_Implementation(AActor* TriggerActor, AActor* OtherActor) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command", meta = (ClampMin = "0.0"))
	float DamageAmount = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
	TSubclassOf<UDamageType> DamageType;
};
