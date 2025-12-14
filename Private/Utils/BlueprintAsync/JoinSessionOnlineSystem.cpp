#include "Utils/BlueprintAsync/JoinSessionOnlineSystem.h"

#include "Core/Systems/OnlineSystem.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
UJoinSessionOnlineSystem* UJoinSessionOnlineSystem::JoinSession(const UObject* WorldContextObject,
	const FSessionSearchResult& Session)
{
	UJoinSessionOnlineSystem* Node = NewObject<UJoinSessionOnlineSystem>();
	Node->WorldPtr = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	Node->Session = Session;
	Node->bIsByCode = false;
	return Node;
}

UJoinSessionOnlineSystem* UJoinSessionOnlineSystem::JoinSessionByCode(const UObject* WorldContextObject,
	const FString& SessionCode)
{
	UJoinSessionOnlineSystem* Node = NewObject<UJoinSessionOnlineSystem>();
	Node->WorldPtr = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	Node->SessionCode = SessionCode;
	Node->bIsByCode = true;
	return Node;
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
void UJoinSessionOnlineSystem::Activate()
{
	Super::Activate();
	if (!WorldPtr.IsValid())
	{
		OnJoinSessionComplete(false, ESessionJoinError::Unknown);
		return;
	}
	const UGameInstance* GameInstance = WorldPtr->GetGameInstance();
	if (!GameInstance)
	{
		OnJoinSessionComplete(false, ESessionJoinError::Unknown);
		return;
	}
	UOnlineSystem* OnlineSystem = GameInstance->GetSubsystem<UOnlineSystem>();
	if (!OnlineSystem)
	{
		OnJoinSessionComplete(false, ESessionJoinError::NetworkError);
		return;
	}
	
	OnlineSystem->OnSessionJoined.AddDynamic(this, &UJoinSessionOnlineSystem::OnJoinSessionComplete);
	if (bIsByCode)
	{
		OnlineSystem->JoinSessionByCode(SessionCode);
		return;
	}
	OnlineSystem->JoinSession(Session);
}

void UJoinSessionOnlineSystem::OnJoinSessionComplete(const bool bSuccess, const ESessionJoinError Error)
{
	if (bSuccess)
	{
		OnSuccess.Broadcast(Error);
	}
	else
	{
		OnFailure.Broadcast(Error);
	}
	OnSuccess.Clear();
	OnFailure.Clear();
	SetReadyToDestroy();
}
