#include "User Interfaces/WindowRoot.h"

#include "Components/GuiController.h"
#include "Core/DarkPlayerController.h"
#include "Microsoft/AllowMicrosoftPlatformTypes.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
UWindowRoot::UWindowRoot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
// Default: no-op. Override in C++/Blueprint if needed.
void UWindowRoot::OnBeforeActivated_Implementation()
{
}
void UWindowRoot::OnActivated_Implementation()
{
}
void UWindowRoot::OnBeforeDeactivated_Implementation()
{
}

void UWindowRoot::DisableModule() const
{
	if (ModuleKey.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("[WindowRoot] DisableSelf() aborted: ModuleKey is None."));
		return;
	}
	const ADarkPlayerController* PC = Cast<ADarkPlayerController>(GetOwningPlayer());
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WindowRoot] DisableSelf() aborted: wrong Player Controller class used."));
		return;
	}
	if (UGuiController* GC = PC->GuiController)
	{
		if (!GC->EnableModule(ModuleKey, false))
		{
			UE_LOG(LogTemp, Warning, TEXT("[WindowRoot] DisableSelf() failed: GuiControl refused Module '%s'."), *ModuleKey.ToString());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[WindowRoot] DisableSelf() aborted: GuiControl component not found on PC."));
	}
}

FReply UWindowRoot::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	
	if (bCloseOnEsc && InKeyEvent.GetKey() == EKeys::Escape)
	{
		DisableModule();
		return FReply::Unhandled();
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
