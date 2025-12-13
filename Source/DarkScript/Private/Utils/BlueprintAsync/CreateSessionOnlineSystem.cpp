#include "Utils/BlueprintAsync/CreateSessionOnlineSystem.h"

#include "Core/Systems/OnlineSystem.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
UCreateSessionOnlineSystem* UCreateSessionOnlineSystem::Create(const UObject* WorldContextObject,
	const TArray<EClassArchetype>& AllowedArchetypes, const ESessionType SessionType, const FString& SessionName,
	const int32 MinLevelRequired)
{
	UCreateSessionOnlineSystem* Node = NewObject<UCreateSessionOnlineSystem>();
	Node->WorldPtr = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	Node->SessionType = SessionType;
	Node->SessionName = SessionName;
	Node->AllowedArchetypes = AllowedArchetypes;
	Node->MinLevelRequired = MinLevelRequired;
	return Node;
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
void UCreateSessionOnlineSystem::Activate()
{
	Super::Activate();
	
	if (!WorldPtr.IsValid())
	{
		OnCreateSessionComplete(false);
		return;
	}
	const UGameInstance* GameInstance = WorldPtr->GetGameInstance();
	if (!GameInstance)
	{
		OnCreateSessionComplete(false);
		return;
	}
	UOnlineSystem* OnlineSystem = GameInstance->GetSubsystem<UOnlineSystem>();
	if (!OnlineSystem)
	{
		OnCreateSessionComplete(false);
		return;
	}
	FDarkSessionSettings Settings;
	Settings.SessionType = SessionType;
	Settings.SessionName = SessionName;
	Settings.AllowedArchetypes = AllowedArchetypes;
	Settings.MinLevelRequired = MinLevelRequired;
	
	OnlineSystem->OnSessionCreated.AddDynamic(this, &UCreateSessionOnlineSystem::OnCreateSessionComplete);
	OnlineSystem->CreateSession(Settings);
}

void UCreateSessionOnlineSystem::OnCreateSessionComplete(const bool bSuccess)
{
	if (bSuccess)
	{
		OnSuccess.Broadcast();
	}
	else
	{
		OnFailure.Broadcast();
	}
	OnSuccess.Clear();;
	OnFailure.Clear();
	SetReadyToDestroy();
}
