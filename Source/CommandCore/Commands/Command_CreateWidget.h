#pragma once

#include "CoreMinimal.h"
#include "System/Command.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/CommandDebug.h"
#include "Command_CreateWidget.generated.h"

/**
 * Creates a Widget of the given class and adds it to the player's viewport.
 *
 * Owning player is resolved either from InstigatorActor (if it's a Pawn/Controller)
 * or falls back to the game's first local player controller.
 */
UCLASS(meta = (DisplayName = "Create Widget"))
class COMMANDCORE_API UCommand_CreateWidget : public UCommand
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command")
    TSubclassOf<UUserWidget> WidgetClass;

    /** Draw order on the viewport - higher values render on top of lower ones. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Widget")
    int32 ZOrder = 0;

    /** If true, uses InstigatorActor to find the owning player (works if it's a Pawn or Controller). If false, uses the first local player controller. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Widget")
    bool bUseInstigatorAsOwningPlayer = true;

    /** If true, sets input mode to UI Only and shows the mouse cursor after adding the widget. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Widget")
    bool bSetInputModeUIOnly = false;

    /** Cached reference to the last widget created by this command instance. */
    UPROPERTY(Transient, BlueprintReadOnly, Category = "Command|Widget")
    TObjectPtr<UUserWidget> CreatedWidget;

private:
    APlayerController *ResolveOwningPlayerController(AActor *OwnerActor, AActor *InstigatorActor) const
    {
        if (bUseInstigatorAsOwningPlayer)
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
        }

        UWorld *World = OwnerActor ? OwnerActor->GetWorld() : nullptr;
        return World ? UGameplayStatics::GetPlayerController(World, 0) : nullptr;
    }

protected:
    virtual bool K2_CanExecute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) const override
    {
        if (!WidgetClass)
        {
            return false;
        }

        return Super::K2_CanExecute_Implementation(OwnerActor, InstigatorActor);
    }
    virtual void K2_Execute_Implementation(AActor *OwnerActor, AActor *InstigatorActor) override
    {
        APlayerController *OwningPlayer = ResolveOwningPlayerController(OwnerActor, InstigatorActor);
        if (!OwningPlayer)
        {
            Print(TEXT("Could not resolve an owning PlayerController for the widget."), true);
            return;
        }

        UUserWidget *NewWidget = CreateWidget<UUserWidget>(OwningPlayer, WidgetClass);
        if (!NewWidget)
        {
            Print(FString::Printf(TEXT("Failed to create widget of class %s."), *WidgetClass->GetName()), true);
            return;
        }

        NewWidget->AddToViewport(ZOrder);
        CreatedWidget = NewWidget;

        if (bSetInputModeUIOnly)
        {
            FInputModeUIOnly InputMode;
            InputMode.SetWidgetToFocus(NewWidget->TakeWidget());
            OwningPlayer->SetInputMode(InputMode);
            OwningPlayer->SetShowMouseCursor(true);
        }

        LOG("Created widget %s and added to viewport.", *WidgetClass->GetName());

        Super::K2_Execute_Implementation(OwnerActor, InstigatorActor);
    }
};