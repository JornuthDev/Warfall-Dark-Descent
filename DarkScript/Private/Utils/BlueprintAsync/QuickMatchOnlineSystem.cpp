#include "Utils/BlueprintAsync/QuickMatchOnlineSystem.h"

#include "Core/Systems/OnlineSystem.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
UQuickMatchOnlineSystem* UQuickMatchOnlineSystem::Create(const UObject* WorldContextObject, const TArray<EClassArchetype>& SearchForArchetypes)
{
	UQuickMatchOnlineSystem* Node = NewObject<UQuickMatchOnlineSystem>();
	Node->WorldPtr = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	Node->SearchForArchetypes = SearchForArchetypes;
	return Node;
}

void UQuickMatchOnlineSystem::Activate()
{
	Super::Activate();
	
	if (!WorldPtr.IsValid())
	{
		OnCompleteQuickMatch(false);
		return;
	}
	const UGameInstance* GameInstance = WorldPtr->GetGameInstance();
	if (!GameInstance)
	{
		OnCompleteQuickMatch(false);
		return;
	}
	OnlineSystemPtr = GameInstance->GetSubsystem<UOnlineSystem>();
	if (!OnlineSystemPtr.IsValid())
	{
		OnCompleteQuickMatch(false);
		return;
	}
	FSessionFilters Filters;
	Filters.SessionType = ESessionType::QuickMatch;
	Filters.Archetypes = SearchForArchetypes;
	
	OnlineSystemPtr->OnSessionSearchComplete.AddDynamic(this, &UQuickMatchOnlineSystem::OnSessionSearchComplete);
	OnlineSystemPtr->FindSession(Filters);
}



// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
void UQuickMatchOnlineSystem::OnSessionSearchComplete(const TArray<FSessionSearchResult>& Results, const bool bSuccess,
	const ESessionJoinError Error)
{
	if (bSuccess)
	{
		OnlineSystemPtr->OnSessionJoined.AddDynamic(this, &UQuickMatchOnlineSystem::OnJoinSessionComplete);
	}
	else
	{
		OnlineSystemPtr->OnSessionCreated.AddDynamic(this, &UQuickMatchOnlineSystem::OnCreateSessionComplete);
	}
}

void UQuickMatchOnlineSystem::OnJoinSessionComplete(const bool bSuccess, const ESessionJoinError Error)
{
	if (bSuccess)
	{
		OnJoin.Broadcast();
	}
	OnCompleteQuickMatch(bSuccess);
}

void UQuickMatchOnlineSystem::OnCreateSessionComplete(const bool bSuccess)
{
	if (bSuccess)
	{
		OnCreate.Broadcast();
	}
	OnCompleteQuickMatch(bSuccess);
}

void UQuickMatchOnlineSystem::OnCompleteQuickMatch(const bool bSuccess)
{
	if (!bSuccess)
	{
		OnFailure.Broadcast();
	}
	OnJoin.Clear();
	OnCreate.Clear();
	OnFailure.Clear();
	SetReadyToDestroy();
}		

