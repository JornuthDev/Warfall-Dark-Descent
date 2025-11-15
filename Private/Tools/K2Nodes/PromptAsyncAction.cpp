#include "Tools/K2Nodes/PromptAsyncAction.h"

#include "Core/SubSystems/NavigationInstance.h"
#include "User Interfaces/PromptRoot.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
UPromptAsyncAction* UPromptAsyncAction::CreatePrompt(UObject* WorldContextObject, TSubclassOf<UPromptRoot> WidgetClass, EPromptType PromptType, bool bCloseOnEsc)
{
	UPromptAsyncAction* Node = NewObject<UPromptAsyncAction>();
	Node->StoredClass = WidgetClass;
	Node->StoredType = PromptType;
	Node->WorldContextObject = WorldContextObject;
	Node->bCloseOnEsc = bCloseOnEsc;
	return Node;
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
void UPromptAsyncAction::Activate()
{
	Super::Activate();

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull);
	NavInstance = World->GetGameInstance()->GetSubsystem<UNavigationInstance>();
	if (!NavInstance.Get())
	{
		OnEscape();
		return;
	}
	
	NavInstance->PromptSuccessResponse.AddDynamic(this, &UPromptAsyncAction::Internal_OnSuccess);
	NavInstance->PromptFailureResponse.AddDynamic(this, &UPromptAsyncAction::Internal_OnFailure);
	NavInstance->PromptCloseByEsc.BindUObject(this, &UPromptAsyncAction::OnEscape);
	
	UPromptRoot* Prompt = NavInstance->OpenPrompt(WorldContextObject.Get(), StoredClass, StoredType, bCloseOnEsc, true);
	if (!Prompt)
	{
		OnEscape();
		return;
	}
	
	PromptRoot = Prompt;
	PromptRoot->AddToViewport();
}

void UPromptAsyncAction::Internal_OnSuccess(const FString& Value)
{
	OnSuccess.Broadcast(Value);
	SetReadyToDestroy();
}

void UPromptAsyncAction::Internal_OnFailure(const FString& Value)
{
	OnFailure.Broadcast(Value);
	SetReadyToDestroy();
}

void UPromptAsyncAction::OnEscape()
{
	if (NavInstance.IsValid())
	{
		NavInstance->PromptSuccessResponse.RemoveDynamic(this, &UPromptAsyncAction::Internal_OnSuccess);
		NavInstance->PromptFailureResponse.RemoveDynamic(this, &UPromptAsyncAction::Internal_OnFailure);
		NavInstance->PromptCloseByEsc.Unbind();
	}
	if (OnSuccess.IsBound())
	{
		OnSuccess.Clear();
	}
	if (OnFailure.IsBound())
	{
		OnFailure.Clear();
	}
	SetReadyToDestroy();
}
