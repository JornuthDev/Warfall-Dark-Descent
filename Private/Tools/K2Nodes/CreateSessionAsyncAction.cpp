#include "Tools/K2Nodes/CreateSessionAsyncAction.h"

#include "Core/SubSystems/ProxyInstance.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
UCreateSessionAsyncAction* UCreateSessionAsyncAction::Create(UObject* WorldContextObject, const bool bIsPublic, const int32 MaxPlayers)
{
	UCreateSessionAsyncAction* Node = NewObject<UCreateSessionAsyncAction>();
	Node->WorldContextObject = WorldContextObject;
	Node->bIsPublic = bIsPublic;
	Node->MaxPlayers = MaxPlayers;
	return Node;
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
void UCreateSessionAsyncAction::Activate()
{
	Super::Activate();
	
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull);
	
	UProxyInstance* Net = World->GetGameInstance()->GetSubsystem<UProxyInstance>();
	Net->OnCreateSessionCompleted.AddUObject(this, &UCreateSessionAsyncAction::OnCreateSessionCompleted);
	Net->CreateSession(bIsPublic, MaxPlayers);
}

void UCreateSessionAsyncAction::OnCreateSessionCompleted(const bool bSuccess)
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
