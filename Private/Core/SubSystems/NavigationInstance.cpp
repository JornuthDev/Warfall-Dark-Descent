// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppMemberFunctionMayBeConst
#include "Core/SubSystems/NavigationInstance.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "User Interfaces/NavigationRoot.h"
#include "Framework/Application/SlateApplication.h"
#include "Kismet/GameplayStatics.h"
#include "Slate/SObjectWidget.h"
#include "User Interfaces/PromptRoot.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
int32 UNavigationInstance::GetNavigationIndex() const
{
	if (NavigationRoot)
	{
		return NavigationRoot->NavigationIndex;
	}
	return -1;
}

void UNavigationInstance::SetNavigationRoot(UNavigationRoot* InNavRoot)
{
	if (NavigationRoot != InNavRoot)
	{
		NavigationRoot = InNavRoot;
	}
	NavigationRoot->OnNavigationRootInitialized();
}

auto UNavigationInstance::OpenPrompt(UObject* WorldContextObject, const TSubclassOf<UPromptRoot> WidgetClass, const EPromptType PromptType, const bool bCloseOnEsc, const bool bCallBack) -> UPromptRoot*
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	if (!PC) return nullptr;

	const bool bIsShowMouseCursor = PC->bShowMouseCursor;
	const bool bIsInputModeGameOnly = PC->GetCurrentInputModeDebugString().Contains("GameOnly");
	
	UPromptRoot* Prompt = Cast<UPromptRoot>(UWidgetBlueprintLibrary::Create(WorldContextObject, WidgetClass, PC));
	if (Prompt)
	{
		if (!bIsShowMouseCursor) PC->SetShowMouseCursor(true);
		Prompt->PromptType = PromptType;
		Prompt->bIsShowMouseCursor = bIsShowMouseCursor;
		Prompt->bIsInputModeGameOnly = bIsInputModeGameOnly;
		Prompt->bCloseOnEsc = bCloseOnEsc;
		Prompt->LastFocus = GetFocusedWidget();
		Prompt->OnYes.AddDynamic(this, &UNavigationInstance::OnSuccessResponse);
		Prompt->OnNo.AddDynamic(this, &UNavigationInstance::OnFailureResponse);
		Prompt->OnEscape.BindUObject(this, &UNavigationInstance::OnCloseByEscape);
		
		if (!bCallBack)
		{
			Prompt->AddToViewport();
		}
	}
	return Prompt;
}

void UNavigationInstance::OnSuccessResponse(const FString& Value)
{
	PromptSuccessResponse.Broadcast(Value);
}

void UNavigationInstance::OnFailureResponse(const FString& Value)
{
	PromptFailureResponse.Broadcast(Value);
}

void UNavigationInstance::OnCloseByEscape()
{
	if (PromptCloseByEsc.IsBound())
	{
		PromptCloseByEsc.Execute();
	}
}

UWidget* UNavigationInstance::GetFocusedWidget() const
{
	const TSharedPtr<SWidget> Focused = FSlateApplication::Get().GetKeyboardFocusedWidget();
	if (!Focused.IsValid())
		return nullptr;

	if (Focused->GetType() == "SObjectWidget")
	{
		const TSharedPtr<SObjectWidget> ObjectWidget = StaticCastSharedPtr<SObjectWidget>(Focused);
		return Cast<UWidget>(ObjectWidget->GetWidgetObject());
	}

	return nullptr;
}

