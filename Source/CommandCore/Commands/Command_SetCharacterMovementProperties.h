#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Engine/CommandDebug.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Command_SetCharacterMovementProperties.generated.h"

/**
 * Checks if the target actor is a Character, then sets properties on its
 * CharacterMovementComponent - movement mode, speeds, jump settings, etc.
 *
 * Each property has its own override toggle - only checked properties are
 * applied, everything else is left untouched. Fails safely (logs and skips)
 * if the target actor is not a Character.
 */
UCLASS(meta = (DisplayName = "Set Character Movement"))
class COMMANDCORE_API UCommand_SetCharacterMovementProperties : public UCommand
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
    ECommandTargetActor TargetActor = ECommandTargetActor::InstigatorActor;

    /** Only used when TargetActor is "Other Actor". */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command", meta = (EditCondition = "TargetActor == ECommandTargetActor::OtherActor", EditConditionHides))
    TSoftObjectPtr<AActor> OtherActor;

    // --- Movement Mode ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Movement", meta = (InlineEditConditionToggle))
    bool bOverrideMovementMode = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Movement", meta = (EditCondition = "bOverrideMovementMode"))
    TEnumAsByte<EMovementMode> MovementMode = MOVE_Walking;

    // --- Max Walk Speed ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Movement", meta = (InlineEditConditionToggle))
    bool bOverrideMaxWalkSpeed = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Movement", meta = (EditCondition = "bOverrideMaxWalkSpeed", ClampMin = "0.0"))
    float MaxWalkSpeed = 600.0f;

    // --- Max Walk Speed Crouched ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Movement", meta = (InlineEditConditionToggle))
    bool bOverrideMaxWalkSpeedCrouched = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Movement", meta = (EditCondition = "bOverrideMaxWalkSpeedCrouched", ClampMin = "0.0"))
    float MaxWalkSpeedCrouched = 300.0f;

    // --- Max Fly Speed ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Movement", meta = (InlineEditConditionToggle))
    bool bOverrideMaxFlySpeed = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Movement", meta = (EditCondition = "bOverrideMaxFlySpeed", ClampMin = "0.0"))
    float MaxFlySpeed = 600.0f;

    // --- Max Swim Speed ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Movement", meta = (InlineEditConditionToggle))
    bool bOverrideMaxSwimSpeed = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Movement", meta = (EditCondition = "bOverrideMaxSwimSpeed", ClampMin = "0.0"))
    float MaxSwimSpeed = 300.0f;

    // --- Max Acceleration ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Movement", meta = (InlineEditConditionToggle))
    bool bOverrideMaxAcceleration = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Movement", meta = (EditCondition = "bOverrideMaxAcceleration", ClampMin = "0.0"))
    float MaxAcceleration = 2048.0f;

    // --- Gravity Scale ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Movement", meta = (InlineEditConditionToggle))
    bool bOverrideGravityScale = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Movement", meta = (EditCondition = "bOverrideGravityScale"))
    float GravityScale = 1.0f;

    // --- Jump Z Velocity ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Movement", meta = (InlineEditConditionToggle))
    bool bOverrideJumpZVelocity = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Movement", meta = (EditCondition = "bOverrideJumpZVelocity", ClampMin = "0.0"))
    float JumpZVelocity = 420.0f;

    // --- Air Control ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Movement", meta = (InlineEditConditionToggle))
    bool bOverrideAirControl = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Movement", meta = (EditCondition = "bOverrideAirControl", ClampMin = "0.0", ClampMax = "1.0"))
    float AirControl = 0.2f;

    // --- Can Crouch ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Movement", meta = (InlineEditConditionToggle))
    bool bOverrideCanCrouch = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Movement", meta = (EditCondition = "bOverrideCanCrouch"))
    bool bCanCrouch = true;

protected:
    virtual AActor *K2_GetOtherActor_Implementation() const
    {
        return OtherActor.LoadSynchronous();
    }
    virtual bool K2_CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const override
    {
        AActor *Target = ResolveTargetActor(OwnerActor, InstigatorActor, TargetActor);
        if (!Cast<ACharacter>(Target))
        {
            return false;
        }

        const bool bHasAnyOverride = bOverrideMovementMode || bOverrideMaxWalkSpeed || bOverrideMaxWalkSpeedCrouched || bOverrideMaxFlySpeed || bOverrideMaxSwimSpeed || bOverrideMaxAcceleration || bOverrideGravityScale || bOverrideJumpZVelocity || bOverrideAirControl || bOverrideCanCrouch;

        if (!bHasAnyOverride)
        {
            return false;
        }

        return Super::K2_CanExecute_Implementation(OwnerActor, InstigatorActor);
    }
    virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
    {
        AActor *Target = ResolveTargetActor(OwnerActor, InstigatorActor, TargetActor);

        ACharacter *Character = Cast<ACharacter>(Target);
        if (!Character)
        {
            Print(TEXT("Target actor is not a Character."), true);
            return;
        }

        UCharacterMovementComponent *MovementComp = Character->GetCharacterMovement();
        if (!MovementComp)
        {
            Print(FString::Printf(TEXT("%s has no CharacterMovementComponent."), *Character->GetName()), true);
            return;
        }

        if (bOverrideMaxWalkSpeed)
        {
            MovementComp->MaxWalkSpeed = MaxWalkSpeed;
        }

        if (bOverrideMaxWalkSpeedCrouched)
        {
            MovementComp->MaxWalkSpeedCrouched = MaxWalkSpeedCrouched;
        }

        if (bOverrideMaxFlySpeed)
        {
            MovementComp->MaxFlySpeed = MaxFlySpeed;
        }

        if (bOverrideMaxSwimSpeed)
        {
            MovementComp->MaxSwimSpeed = MaxSwimSpeed;
        }

        if (bOverrideMaxAcceleration)
        {
            MovementComp->MaxAcceleration = MaxAcceleration;
        }

        if (bOverrideGravityScale)
        {
            MovementComp->GravityScale = GravityScale;
        }

        if (bOverrideJumpZVelocity)
        {
            MovementComp->JumpZVelocity = JumpZVelocity;
        }

        if (bOverrideAirControl)
        {
            MovementComp->AirControl = AirControl;
        }

        if (bOverrideCanCrouch)
        {
            Character->GetCharacterMovement()->NavAgentProps.bCanCrouch = bCanCrouch;
        }

        // Movement Mode applied last - some modes (e.g. Flying) depend on speed values already being set.
        if (bOverrideMovementMode)
        {
            MovementComp->SetMovementMode(MovementMode.GetValue());
        }

        LOG("Applied CharacterMovement overrides to %s.", *Character->GetName());

        Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
    }
};