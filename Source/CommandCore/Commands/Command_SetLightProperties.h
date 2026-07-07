#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Engine/CommandDebug.h"
#include "Engine/Light.h"
#include "Components/LightComponent.h"
#include "Components/LocalLightComponent.h"
#include "Command_SetLightProperties.generated.h"

/**
 * Sets common properties on a list of Light Actors placed in the scene.
 *
 * Each property has its own override toggle - only checked properties are
 * applied, everything else is left untouched. Attenuation Radius only
 * applies to Point/Spot/Rect lights; it's ignored (with a log warning)
 * on Directional Lights.
 */
UCLASS(meta = (DisplayName = "Set Light Properties"))
class COMMANDCORE_API UCommand_SetLightProperties : public UCommand
{
    GENERATED_BODY()

protected:
    /** Light actors placed in the level to apply these settings to. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Command")
    TArray<TSoftObjectPtr<ALight>> TargetActors;

    // --- Intensity ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Light", meta = (InlineEditConditionToggle))
    bool bOverrideIntensity = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Light", meta = (EditCondition = "bOverrideIntensity", ClampMin = "0.0"))
    float Intensity = 1000.0f;

    // --- Light Color ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Light", meta = (InlineEditConditionToggle))
    bool bOverrideLightColor = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Light", meta = (EditCondition = "bOverrideLightColor"))
    FLinearColor LightColor = FLinearColor::White;

    // --- Cast Shadows ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Light", meta = (InlineEditConditionToggle))
    bool bOverrideCastShadows = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Light", meta = (EditCondition = "bOverrideCastShadows"))
    bool bCastShadows = true;

    // --- Visibility ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Light", meta = (InlineEditConditionToggle))
    bool bOverrideVisibility = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Light", meta = (EditCondition = "bOverrideVisibility"))
    bool bVisible = true;

    // --- Attenuation Radius (Point/Spot/Rect lights only) ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Light", meta = (InlineEditConditionToggle))
    bool bOverrideAttenuationRadius = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Light", meta = (EditCondition = "bOverrideAttenuationRadius", ClampMin = "0.0"))
    float AttenuationRadius = 1000.0f;

    // --- Temperature ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Light", meta = (InlineEditConditionToggle))
    bool bOverrideUseTemperature = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Light", meta = (EditCondition = "bOverrideUseTemperature"))
    bool bUseTemperature = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Light",
              meta = (EditCondition = "bOverrideUseTemperature && bUseTemperature", EditConditionHides, ClampMin = "1700.0", ClampMax = "22000.0"))
    float Temperature = 6500.0f;

private:
    void ApplyToLightActor(ALight *LightActor) const
    {
        if (!LightActor)
        {
            return;
        }

        ULightComponent *LightComp = LightActor->GetLightComponent();
        if (!LightComp)
        {
            LOG_ERROR("%s has no LightComponent.", *LightActor->GetName());
            return;
        }

        if (bOverrideIntensity)
        {
            LightComp->SetIntensity(Intensity);
        }

        if (bOverrideLightColor)
        {
            LightComp->SetLightColor(LightColor);
        }

        if (bOverrideCastShadows)
        {
            LightComp->SetCastShadows(bCastShadows);
        }

        if (bOverrideVisibility)
        {
            LightComp->SetVisibility(bVisible);
        }

        if (bOverrideUseTemperature)
        {
            LightComp->SetUseTemperature(bUseTemperature);

            if (bUseTemperature)
            {
                LightComp->SetTemperature(Temperature);
            }
        }

        if (bOverrideAttenuationRadius)
        {
            if (ULocalLightComponent *LocalLight = Cast<ULocalLightComponent>(LightComp))
            {
                LocalLight->SetAttenuationRadius(AttenuationRadius);
            }
            else
            {
                LOG_ERROR("AttenuationRadius override ignored on %s - Directional Lights have no attenuation radius.", *LightActor->GetName());
            }
        }
    }

protected:
    virtual bool K2_CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const override
    {
        if (TargetActors.Num() == 0)
        {
            return false;
        }

        const bool bHasAnyOverride = bOverrideIntensity || bOverrideLightColor || bOverrideCastShadows || bOverrideVisibility || bOverrideAttenuationRadius || bOverrideUseTemperature;

        if (!bHasAnyOverride)
        {
            return false;
        }

        return Super::K2_CanExecute_Implementation(OwnerActor, InstigatorActor);
    }
    virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
    {
        int32 AppliedCount = 0;

        for (const TSoftObjectPtr<ALight> &SoftLightActor : TargetActors)
        {
            if (ALight *LightActor = SoftLightActor.LoadSynchronous())
            {
                ApplyToLightActor(LightActor);
                AppliedCount++;
            }
        }

        LOG("Applied light property overrides to %d light actor(s).", AppliedCount);

        Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
    }
};