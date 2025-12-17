#include "Utils/BlueprintAsync/FindSessionOnlineSystem.h"

#include "Core/Systems/OnlineSystem.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
UFindSessionOnlineSystem* UFindSessionOnlineSystem::Create(const UObject* WorldContextObject,
	const TArray<EClassArchetype>& SearchForArchetypes, const ESessionType SessionType, const int32 MinimumPlayers,
	const int32 MinLevelRequired)
{
	UFindSessionOnlineSystem* Node = NewObject<UFindSessionOnlineSystem>();
	Node->WorldPtr = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	Node->SessionType = SessionType;
	Node->SearchForArchetypes = SearchForArchetypes;
	Node->MinimumPlayers = MinimumPlayers;
	Node->MinLevelRequired = MinLevelRequired;
	return Node;
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
void UFindSessionOnlineSystem::Activate()
{
	Super::Activate();
	
	if (!WorldPtr.IsValid())
	{
		OnFindSessionComplete(TArray<FSessionSearchResult>(), false, ESessionJoinError::Unknown);
		return;
	}
	const UGameInstance* GameInstance = WorldPtr->GetGameInstance();
	if (!GameInstance)
	{
		OnFindSessionComplete(TArray<FSessionSearchResult>(), false, ESessionJoinError::Unknown);
		return;
	}
	UOnlineSystem* OnlineSystem = GameInstance->GetSubsystem<UOnlineSystem>();
	if (!OnlineSystem)
	{
		OnFindSessionComplete(TArray<FSessionSearchResult>(), false, ESessionJoinError::NetworkError);
		return;
	}
	
	FSessionFilters Filters;
	Filters.SessionType = SessionType;
	Filters.Archetypes = SearchForArchetypes;
	Filters.MinLevelRequired = MinLevelRequired;
	Filters.MinPlayers = MinimumPlayers;
	
	OnlineSystem->OnSessionSearchComplete.AddDynamic(this, &UFindSessionOnlineSystem::OnFindSessionComplete);
	OnlineSystem->FindSession(Filters);
}

void UFindSessionOnlineSystem::OnFindSessionComplete(const TArray<FSessionSearchResult>& Results, bool bWasSuccessful,
	ESessionJoinError ErrorReason)
{
	if (bWasSuccessful)
	{
		OnSuccess.Broadcast(Results, ErrorReason);
	}
	else
	{
		OnFailure.Broadcast(Results, ErrorReason);
	}
	OnSuccess.Clear();
	OnFailure.Clear();
	SetReadyToDestroy();
}

