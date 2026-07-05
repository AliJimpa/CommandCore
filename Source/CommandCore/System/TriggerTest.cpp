// Fill out your copyright notice in the Description page of Project Settings.

#include "System/TriggerTest.h"
#include "TriggerTest.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"

namespace
{
    static const FColor TriggerBaseColor(100, 255, 100, 255);
    static const FName TriggerCollisionProfileName(TEXT("Trigger"));
}

ATriggerTest::ATriggerTest(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UBoxComponent>(TEXT("CollisionComp")))
{
    UBoxComponent *BoxCollisionComponent = CastChecked<UBoxComponent>(GetCollisionComponent());

    BoxCollisionComponent->ShapeColor = TriggerBaseColor;
    BoxCollisionComponent->InitBoxExtent(FVector(40.0f, 40.0f, 40.0f));
    BoxCollisionComponent->SetCollisionProfileName(TriggerCollisionProfileName);
#if WITH_EDITORONLY_DATA
    if (UBillboardComponent *TriggerSpriteComponent = GetSpriteComponent())
    {
        TriggerSpriteComponent->SetupAttachment(BoxCollisionComponent);
    }

    if (GetSpriteComponent())
    {
        // Structure to hold one-time initialization
        struct FConstructorStatics
        {
            ConstructorHelpers::FObjectFinderOptional<UTexture2D> TriggerTextureObject;
            FName ID_Triggers;
            FText NAME_Triggers;
            FConstructorStatics()
                : TriggerTextureObject(TEXT("/Engine/EditorResources/S_TriggerBox")), ID_Triggers(TEXT("Triggers")), NAME_Triggers(NSLOCTEXT("SpriteCategory", "Triggers", "Triggers"))
            {
            }
        };
        static FConstructorStatics ConstructorStatics;

        GetSpriteComponent()->Sprite = ConstructorStatics.TriggerTextureObject.Get();
    }
#endif
}

#if WITH_EDITOR
void ATriggerTest::EditorApplyScale(const FVector &DeltaScale, const FVector *PivotLocation, bool bAltDown, bool bShiftDown, bool bCtrlDown)
{
    const FVector ModifiedScale = DeltaScale * (AActor::bUsePercentageBasedScaling ? 500.0f : 5.0f);

    UBoxComponent *BoxComponent = CastChecked<UBoxComponent>(GetRootComponent());
    if (bCtrlDown)
    {
        // CTRL+Scaling modifies trigger collision height.  This is for convenience, so that height
        // can be changed without having to use the non-uniform scaling widget (which is
        // inaccessable with spacebar widget cycling).
        FVector Extent = BoxComponent->GetUnscaledBoxExtent() + FVector(0, 0, ModifiedScale.X);
        Extent.Z = FMath::Max<FVector::FReal>(0, Extent.Z);
        BoxComponent->SetBoxExtent(Extent);
    }
    else
    {
        FVector Extent = BoxComponent->GetUnscaledBoxExtent() + FVector(ModifiedScale.X, ModifiedScale.Y, ModifiedScale.Z);
        Extent.X = FMath::Max<FVector::FReal>(0, Extent.X);
        Extent.Y = FMath::Max<FVector::FReal>(0, Extent.Y);
        Extent.Z = FMath::Max<FVector::FReal>(0, Extent.Z);
        BoxComponent->SetBoxExtent(Extent);
    }
}
#endif