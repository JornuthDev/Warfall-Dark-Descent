#include "Tools/K2Nodes/JoinSessionAsyncAction.h"
#include "Core/SubSystems/ProxyInstance.h"

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

void UJoinSessionAsyncAction::OnJoinSessionCompleted(FName SessionName,  uint8 Result)
{
	EOnJoinSessionResult BPResult = EOnJoinSessionResult::E_Success;
	if (Result == 0)
	{
		OnSuccess.Broadcast(BPResult);
		SetReadyToDestroy();
		return;
	}
	if (Result == 1)
	{
		BPResult = EOnJoinSessionResult::E_SessionFull;
	}
	else if (Result == 2 || Result == 3 || Result == 5)
	{
		BPResult = EOnJoinSessionResult::E_WrongCode;
	}
	else if (Result == 4)
	{
		BPResult = EOnJoinSessionResult::E_AlreadyInSession;
	}

	OnFailure.Broadcast(BPResult);
	SetReadyToDestroy();
}
