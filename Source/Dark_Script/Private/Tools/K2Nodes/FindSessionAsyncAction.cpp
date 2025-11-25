#include "Tools/K2Nodes/FindSessionAsyncAction.h"
#include "Core/SubSystems/ProxyInstance.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
UFindSessionAsyncAction* UFindSessionAsyncAction::Create(UObject* WorldContextObject, const TArray<FSessionPropertyKeyPair>& Filters, const ESessionVisibility Visibility)
{
	UFindSessionAsyncAction* Node = NewObject<UFindSessionAsyncAction>();
	Node->WorldContextObject = WorldContextObject;
	Node->Filters = Filters;
	Node->SessionVisibility = Visibility;
	return Node;
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
void UFindSessionAsyncAction::Activate()
{
	Super::Activate();
	
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull);
	
	UProxyInstance* Net = World->GetGameInstance()->GetSubsystem<UProxyInstance>();
	Net->OnFindSessionsCompleted.AddUObject(this, &UFindSessionAsyncAction::OnFindSessionCompleted);
	Net->FindSessions(Filters, SessionVisibility);
}

void UFindSessionAsyncAction::OnFindSessionCompleted(const bool bSuccess, const TArray<FBlueprintSessionResult>& Results)
{
	if (bSuccess && Results.Num())
	{
		OnSuccess.Broadcast(Results);
		SetReadyToDestroy();
		return;
	}
	OnFailure.Broadcast(Results);
	SetReadyToDestroy();
}
