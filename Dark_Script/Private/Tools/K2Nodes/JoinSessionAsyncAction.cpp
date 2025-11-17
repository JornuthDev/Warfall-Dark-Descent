#include "Tools/K2Nodes/JoinSessionAsyncAction.h"
#include "Core/SubSystems/ProxyInstance.h"
#include "Tools/Types/ProxyTypes.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
UJoinSessionAsyncAction* UJoinSessionAsyncAction::Create(UObject* WorldContextObject, const FString Code)
{
	UJoinSessionAsyncAction* Node = NewObject<UJoinSessionAsyncAction>();
	Node->WorldContextObject = WorldContextObject;
	Node->SessionCode = Code;
	return Node;
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
void UJoinSessionAsyncAction::Activate()
{
	Super::Activate();
	
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull);
	
	UProxyInstance* Net = World->GetGameInstance()->GetSubsystem<UProxyInstance>();
	Net->OnJoinSessionCompleted.AddUObject(this, &UJoinSessionAsyncAction::OnJoinSessionCompleted);
	Net->JoinSessionByCode(SessionCode);
}

void UJoinSessionAsyncAction::OnJoinSessionCompleted(FName SessionName,  const EOnJoinSessionCompleteResult::Type Result)
{
	const EOnJoinSessionResult BPResult = FSessionResultHelper(Result).GetResult();
	
	if (BPResult == EOnJoinSessionResult::E_Success)
	{
		OnSuccess.Broadcast(BPResult);
		SetReadyToDestroy();
		return;
	}
	OnFailure.Broadcast(BPResult);
	SetReadyToDestroy();
}
