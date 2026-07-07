#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/CommandDebug.h"
#include "Command_PlayCameraShake.generated.h"

class UCameraShakeBase;
class APlayerController;

/**
 * Plays a Camera Shake on the target player's Player Camera Manager.
 *
 * Owning player is resolved either from the target actor (if it's a Pawn/Controller),
 * or falls back to the first local player controller.
 */
UCLASS(meta = (DisplayName = "Play Camera Shake"))
class COMMANDCORE_API UCommand_PlayCameraShake : public UCommand
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Shake")
    TSubclassOf<UCameraShakeBase> ShakeClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Shake", meta = (ClampMin = "0.0"))
    float Scale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Shake")
    ECameraShakePlaySpace PlaySpace = ECameraShakePlaySpace::CameraLocal;

    /** Only used when PlaySpace is UserDefined. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Shake",
              meta = (EditCondition = "PlaySpace == ECameraShakePlaySpace::UserDefined", EditConditionHides))
    FRotator UserPlaySpaceRotation = FRotator::ZeroRotator;

    /** If true, falls back to the first local player controller if the resolved target actor has no PlayerController. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Shake")
    bool bFallbackToFirstPlayer = true;

private:
    APlayerController *ResolvePlayerController(AActor *InstigatorActor) const
    {
        if (APlayerController *PC = Cast<APlayerController>(InstigatorActor))
        {
            return PC;
        }

        if (APawn *Pawn = Cast<APawn>(InstigatorActor))
        {
            if (APlayerController *PC = Cast<APlayerController>(Pawn->GetController()))
            {
                return PC;
            }
        }

        if (bFallbackToFirstPlayer)
        {
            UWorld *World = InstigatorActor->GetWorld();
            if (World)
            {
                return UGameplayStatics::GetPlayerController(World, 0);
            }
        }

        return nullptr;
    }

protected:
    virtual bool K2_CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const override
    {
        if (!ShakeClass)
        {
            return false;
        }

        return Super::K2_CanExecute_Implementation(OwnerActor, InstigatorActor);
    }
    virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
    {
        APlayerController *PC = ResolvePlayerController(InstigatorActor);
        if (!PC)
        {
            Print(TEXT("Could not resolve a PlayerController to play the Camera Shake on."), true);
            return;
        }

        APlayerCameraManager *CameraManager = PC->PlayerCameraManager;
        if (!CameraManager)
        {
            Print(TEXT("Resolved PlayerController has no PlayerCameraManager."), true);
            return;
        }

        CameraManager->StartCameraShake(ShakeClass, Scale, PlaySpace, UserPlaySpaceRotation);

        LOG("Played Camera Shake %s on %s.", *ShakeClass->GetName(), *PC->GetName());

        Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
    }
};