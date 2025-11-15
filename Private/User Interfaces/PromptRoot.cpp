#include "User Interfaces/PromptRoot.h"


void UPromptRoot::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetUserFocus(GetOwningPlayer());
	SetKeyboardFocus();
	FInputModeUIOnly Mode;
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	Mode.SetWidgetToFocus(this->TakeWidget());
	GetOwningPlayer()->SetInputMode(Mode);
}

FReply UPromptRoot::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	
	if (bCloseOnEsc && InKeyEvent.GetKey() == EKeys::Escape)
	{
		bEndByClose = true;
		RemoveFromParent();
		return FReply::Unhandled();
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UPromptRoot::NativeDestruct()
{
	Super::NativeDestruct();
		
	APlayerController* PlayerController = GetOwningPlayer();
	if (!PlayerController) return;
	PlayerController->SetShowMouseCursor(bIsShowMouseCursor);
	if (bIsInputModeGameOnly) PlayerController->SetInputMode(FInputModeGameOnly());
	if (LastFocus)
	{
		LastFocus->SetUserFocus(PlayerController);
		LastFocus->SetKeyboardFocus();
	}
	
	if (bEndByClose)
	{
		if (OnEscape.IsBound())
		{
			OnEscape.Execute();
		}
		return;
	}
	
	if (bResponse)
	{
		OnYes.Broadcast(PromptValue);
		return;
	}
	OnNo.Broadcast(PromptValue);
}
