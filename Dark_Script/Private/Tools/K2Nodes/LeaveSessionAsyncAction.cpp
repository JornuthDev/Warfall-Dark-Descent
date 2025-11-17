#include "Tools/K2Nodes/LeaveSessionAsyncAction.h"

#include "Core/SubSystems/ProxyInstance.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
ULeaveSessionAsyncAction* ULeaveSessionAsyncAction::Create(UObject* WorldContextObject)
{
	ULeaveSessionAsyncAction* Node = NewObject<ULeaveSessionAsyncAction>();
	Node->WorldContextObject = WorldContextObject;
	return Node;
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
void ULeaveSessionAsyncAction::Activate()
{
	Super::Activate();
	
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull);
	
	UProxyInstance* Net = World->GetGameInstance()->GetSubsystem<UProxyInstance>();
	Net->OnLeaveSessionCompleted.AddUObject(this, &ULeaveSessionAsyncAction::OnLeaveSessionCompleted);
	Net->LeaveSession();
}

void ULeaveSessionAsyncAction::OnLeaveSessionCompleted(const bool bSuccess)
{
	if (bSuccess)
	{
		OnSuccess.Broadcast();
		SetReadyToDestroy();
		return;
	}
	OnFailure.Broadcast();
	SetReadyToDestroy();
}
