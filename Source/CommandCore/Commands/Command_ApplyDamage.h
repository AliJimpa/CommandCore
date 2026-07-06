#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Kismet/GameplayStatics.h"
#include "Command_ApplyDamage.generated.h"

class UDamageType;

/** Applies damage to whichever actor overlapped the trigger - e.g. a lava, spikes, or fire volume. */
UCLASS(meta = (DisplayName = "Apply Damage"))
class COMMANDCORE_API UCommand_ApplyDamage : public UCommand
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Damage", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
	float DamageAmount = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command|Damage", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UDamageType> DamageType;

protected:
	virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
	{
		AController *EventInstigator = nullptr;
		if (APawn *OwnerPawn = Cast<APawn>(OwnerActor))
		{
			EventInstigator = OwnerPawn->GetController();
		}
		UGameplayStatics::ApplyDamage(InstigatorActor, DamageAmount, EventInstigator, OwnerActor, DamageType);

		Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
	}
};
