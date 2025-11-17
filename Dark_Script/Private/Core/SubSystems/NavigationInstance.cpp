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
	if (!PC || PromptRoot || !WidgetClass) return nullptr;

	const bool bIsShowMouseCursor = PC->bShowMouseCursor;
	const bool bIsInputModeGameOnly = PC->GetCurrentInputModeDebugString().Contains("Game Only");
	
	PromptRoot = Cast<UPromptRoot>(UWidgetBlueprintLibrary::Create(WorldContextObject, WidgetClass, PC));
	if (PromptRoot)
	{
		if (!bIsShowMouseCursor) PC->SetShowMouseCursor(true);
		PromptRoot->PromptType = PromptType;
		PromptRoot->bIsShowMouseCursor = bIsShowMouseCursor;
		PromptRoot->bIsInputModeGameOnly = bIsInputModeGameOnly;
		PromptRoot->bCloseOnEsc = bCloseOnEsc;
		PromptRoot->LastFocus = GetFocusedWidget();
		PromptRoot->OnYes.AddDynamic(this, &UNavigationInstance::OnSuccessResponse);
		PromptRoot->OnNo.AddDynamic(this, &UNavigationInstance::OnFailureResponse);
		PromptRoot->OnEscape.BindUObject(this, &UNavigationInstance::OnCloseByEscape);
		
		if (!bCallBack)
		{
			PromptRoot->AddToViewport();
		}
	}
	return PromptRoot;
}

void UNavigationInstance::OnSuccessResponse(const FString& Value)
{
	PromptRoot = nullptr;
	PromptSuccessResponse.Broadcast(Value);
}

void UNavigationInstance::OnFailureResponse(const FString& Value)
{
	PromptRoot = nullptr;
	PromptFailureResponse.Broadcast(Value);
}

void UNavigationInstance::OnCloseByEscape()
{
	PromptRoot = nullptr;
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

