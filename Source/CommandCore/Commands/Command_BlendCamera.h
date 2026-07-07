#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Camera/CameraActor.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/CommandDebug.h"
#include "Command_BlendCamera.generated.h"

/**
 * Blends the target player's view to a Camera Actor placed in the scene.
 *
 * Owning player is resolved either from the target actor (if it's a Pawn/Controller),
 * or falls back to the first local player controller.
 */
UCLASS(meta = (DisplayName = "Blend Camera"))
class COMMANDCORE_API UCommand_BlendCamera : public UCommand
{
    GENERATED_BODY()

protected:
    /** The Camera Actor placed in the scene to blend the view to. If left empty, blends back to the player's default view target (their Pawn). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
    TSoftObjectPtr<ACameraActor> CameraActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Blend", meta = (ClampMin = "0.0"))
    float BlendTime = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Blend")
    TEnumAsByte<EViewTargetBlendFunction> BlendFunction = VTBlend_Cubic;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Blend", meta = (ClampMin = "0.0"))
    float BlendExponent = 2.0f;

    /** If true, locks player input/outgoing camera during the blend (mirrors "Lock Outgoing" in Sequencer camera cuts). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Blend")
    bool bLockOutgoing = false;

    /** If true, falls back to the first local player controller if the resolved target actor has no PlayerController. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
    bool bFallbackToFirstPlayer = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Controller")
	bool bSetControllerRotationToCamera = false;

	/** If true, smoothly interpolates ControlRotation over BlendTime instead of snapping instantly. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Controller",
		meta = (EditCondition = "bSetControllerRotationToCamera", EditConditionHides))
	bool bSmoothRotation = true;

	/** Interpolation speed used when bSmoothRotation is true (higher = faster). Ignored if false. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Controller",
		meta = (EditCondition = "bSetControllerRotationToCamera && bSmoothRotation", EditConditionHides, ClampMin = "0.1"))
	float RotationInterpSpeed = 5.0f;

private:
    FTimerHandle RotationLerpTimerHandle;
    FRotator RotationLerpStart;
    FRotator RotationLerpTarget;
    float RotationLerpElapsed = 0.0f;
    TWeakObjectPtr<APlayerController> RotationLerpPC;

private:
    void TickRotationLerp()
    {
        APlayerController *PC = RotationLerpPC.Get();
        if (!PC)
        {
            return;
        }

        RotationLerpElapsed += PC->GetWorldTimerManager().GetTimerRate(RotationLerpTimerHandle);

        const float Alpha = FMath::Clamp(RotationLerpElapsed / BlendTime, 0.0f, 1.0f);
        const FRotator NewRotation = FMath::RInterpTo(PC->GetControlRotation(), RotationLerpTarget, PC->GetWorldTimerManager().GetTimerRate(RotationLerpTimerHandle), RotationInterpSpeed);

        PC->SetControlRotation(NewRotation);

        if (Alpha >= 1.0f)
        {
            PC->SetControlRotation(RotationLerpTarget);
            PC->GetWorldTimerManager().ClearTimer(RotationLerpTimerHandle);
        }
    }
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
    virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
    {
        APlayerController *PC = ResolvePlayerController(InstigatorActor);
        if (!PC)
        {
            Print(TEXT("Could not resolve a PlayerController to blend the camera for."), true);
            return;
        }

        AActor *NewViewTarget = nullptr;

        if (CameraActor.IsNull())
        {
            NewViewTarget = PC->GetPawn();
        }
        else
        {
            NewViewTarget = CameraActor.LoadSynchronous();
        }

        if (!NewViewTarget)
        {
            Print(TEXT("Could not resolve a valid view target (CameraActor unset and player has no Pawn)."), true);
            return;
        }

        PC->SetViewTargetWithBlend(NewViewTarget, BlendTime, BlendFunction.GetValue(), BlendExponent, bLockOutgoing);

        if (bSetControllerRotationToCamera)
        {
            if (bSmoothRotation)
            {
                // Reset/start a repeating lerp timer instead of snapping instantly.
                RotationLerpPC = PC;
                RotationLerpStart = PC->GetControlRotation();
                RotationLerpTarget = NewViewTarget->GetActorRotation();
                RotationLerpElapsed = 0.0f;

                PC->GetWorldTimerManager().ClearTimer(RotationLerpTimerHandle);
                PC->GetWorldTimerManager().SetTimer(RotationLerpTimerHandle, this, &UCommand_BlendCamera::TickRotationLerp, 0.016f, true);
            }
            else
            {
                PC->SetControlRotation(NewViewTarget->GetActorRotation());
            }
        }

        LOG("Blending %s's camera to %s over %.2fs.", *PC->GetName(), *NewViewTarget->GetName(), BlendTime);

        Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
    }
};