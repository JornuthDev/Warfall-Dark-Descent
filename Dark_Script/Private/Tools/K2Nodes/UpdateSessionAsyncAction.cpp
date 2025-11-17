// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#include "Tools/K2Nodes/UpdateSessionAsyncAction.h"

#include "Core/SubSystems/ProxyInstance.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────


UUpdateSessionAsyncAction* UUpdateSessionAsyncAction::Create(UObject* WorldContextObject,
	const TArray<FSessionPropertyKeyPair> ExtraSettings, const int32 PublicConnections, const int32 PrivateConnections,
	const bool bUseLAN, const bool bAllowInvites, const bool bAllowJoinInProgress, const bool bRefreshOnlineData, const bool bIsDedicatedServer,
	const bool bShouldAdvertise, const bool bAllowJoinViaPresence, const bool bAllowJoinViaPresenceFriendsOnly)
{
	UUpdateSessionAsyncAction* Node = NewObject<UUpdateSessionAsyncAction>();
	Node->WorldContextObject = WorldContextObject;
	Node->ExtraSettings = ExtraSettings;
	Node->PublicConnections = PublicConnections;
	Node->PrivateConnections = PrivateConnections;
	Node->bUseLAN = bUseLAN;
	Node->bAllowInvites = bAllowInvites;
	Node->bAllowJoinInProgress = bAllowJoinInProgress;
	Node->bRefreshOnlineData = bRefreshOnlineData;
	Node->bIsDedicatedServer = bIsDedicatedServer;
	Node->bShouldAdvertise = bShouldAdvertise;
	Node->bAllowJoinViaPresence = bAllowJoinViaPresence;
	Node->bAllowJoinViaPresenceFriendsOnly = bAllowJoinViaPresenceFriendsOnly;
	return Node;
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
void UUpdateSessionAsyncAction::Activate()
{
	Super::Activate();
	
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull);
	
	UProxyInstance* Net = World->GetGameInstance()->GetSubsystem<UProxyInstance>();
	Net->OnUpdateSessionCompleted.AddUObject(this, &UUpdateSessionAsyncAction::OnUpdateSessionCompleted);
	
	FOnlineSessionSettings Settings;
	Settings.NumPublicConnections = PublicConnections;
	Settings.NumPrivateConnections = PrivateConnections;
	Settings.bIsLANMatch = bUseLAN;
	Settings.bAllowInvites = bAllowInvites;
	Settings.bAllowJoinInProgress = bAllowJoinInProgress;
	Settings.bIsDedicated = bIsDedicatedServer;
	Settings.bShouldAdvertise = bShouldAdvertise;
	Settings.bAllowJoinViaPresence = bAllowJoinViaPresence;
	Settings.bAllowJoinViaPresenceFriendsOnly = bAllowJoinViaPresenceFriendsOnly;
	
	Net->UpdateSession(PublicConnections, PrivateConnections, bUseLAN, bAllowInvites, 
		bAllowJoinInProgress, bIsDedicatedServer, bShouldAdvertise, bAllowJoinViaPresence, 
		bAllowJoinViaPresenceFriendsOnly, ExtraSettings, bRefreshOnlineData);
}

void UUpdateSessionAsyncAction::OnUpdateSessionCompleted(const bool bSuccess)
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
