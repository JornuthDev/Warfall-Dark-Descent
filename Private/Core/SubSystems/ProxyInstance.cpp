#include "Core/SubSystems/ProxyInstance.h"

#include "OnlineSubsystemUtils.h"
#include "Core/DarkGameState.h"
#include "Engine/World.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Tools/DarkLibrary.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
void UProxyInstance::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	if (IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld()))
	{
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface.Pin()->OnCreateSessionCompleteDelegates.AddUObject(this, &UProxyInstance::OnCreateSessionComplete);
			SessionInterface.Pin()->OnFindSessionsCompleteDelegates.AddUObject(this, &UProxyInstance::OnFindSessionsComplete);
			SessionInterface.Pin()->OnUpdateSessionCompleteDelegates.AddUObject(this, &UProxyInstance::OnUpdateSessionComplete);
			SessionInterface.Pin()->OnJoinSessionCompleteDelegates.AddUObject(this, &UProxyInstance::OnJoinSessionComplete);
			SessionInterface.Pin()->OnDestroySessionCompleteDelegates.AddUObject(this, &UProxyInstance::OnLeaveSessionComplete);
			SessionInterface.Pin()->OnEndSessionCompleteDelegates.AddUObject(this, &UProxyInstance::OnLeaveSessionComplete);
		}
	}
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
void UProxyInstance::CreateSession(const bool bInIsPublic, const int32 InMaxPlayers)
{
	MaxPlayers = InMaxPlayers;
	bIsPublic = bInIsPublic;
	if (bIsPublic)
	{
		NativeCreateSession();
		return;
	}
	NativeFindSession(EFindSessionsCauses::E_PendingCreation);
}

void UProxyInstance::FindSessions(const bool bInIsPublic)
{
	bIsPublic = bInIsPublic;
	NativeFindSession(EFindSessionsCauses::E_None);
}

void UProxyInstance::UpdateSession(const FOnlineSessionSettings& Settings, const bool bRefreshOnlineData) const
{
	NativeUpdateSession(Settings, bRefreshOnlineData);
}

void UProxyInstance::JoinSessionByCode(const FString Code)
{
	SessionCode = Code;
	bIsPublic = false;
	NativeFindSession(EFindSessionsCauses::E_JoinSession);
}

void UProxyInstance::LeaveSession() const
{
	NativeLeaveSession();
}

void UProxyInstance::DestroySession() const
{
	SessionInterface.Pin()->DestroySession(NAME_GameSession);
}

void UProxyInstance::NativeCreateSession(const FString Code) const
{
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bIsLANMatch = (Online::GetSubsystem(GetWorld())->GetSubsystemName() == "NULL");
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.NumPublicConnections = MaxPlayers;
	
	FOnlineSessionSetting PublicSetting;
	PublicSetting.Data = bIsPublic;
	PublicSetting.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineService;
	SessionSettings.Settings.Add(FName("IS_PUBLIC"), PublicSetting);
	
	FOnlineSessionSetting CodeSetting;
	CodeSetting.Data = Code;
	CodeSetting.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineService;
	SessionSettings.Settings.Add(FName("SESSION_CODE"), CodeSetting);
	
	SessionInterface.Pin()->CreateSession(0, NAME_GameSession, SessionSettings);
}


void UProxyInstance::NativeFindSession(const EFindSessionsCauses Causes)
{
	FindSessionsCauses = Causes;
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = (Online::GetSubsystem(GetWorld())->GetSubsystemName() == "NULL");
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set("SEARCH_PRESENCE", true, EOnlineComparisonOp::Equals);
	SessionSearch->QuerySettings.Set("IS_PUBLIC", bIsPublic, EOnlineComparisonOp::Equals);
	SessionInterface.Pin()->FindSessions(0, SessionSearch.ToSharedRef());
}

void UProxyInstance::NativeUpdateSession(FOnlineSessionSettings Settings, const bool bRefreshOnlineData) const
{
	SessionInterface.Pin()->UpdateSession(NAME_GameSession, Settings, bRefreshOnlineData);
}

void UProxyInstance::NativeLeaveSession(const FString KickReason) const
{
	if (IsServer())
	{
		if (ADarkGameState* GameState = Cast<ADarkGameState>(UGameplayStatics::GetGameState(GetWorld())))
		{
			for (const APlayerState* PState : GameState->PlayerArray)
			{
				if (!PState) continue;
				APlayerController* PController = PState->GetPlayerController();
				if (!PController) continue;
				if (PController == UGameplayStatics::GetPlayerController(GetWorld(), 0)) continue;
				UDarkLibrary::KickPlayer(GetWorld(), PController, "HostEndedSession");
			}
		}
		SessionInterface.Pin()->DestroySession(NAME_GameSession);
		return;
	}
	SessionInterface.Pin()->DestroySession(NAME_GameSession);
}

void UProxyInstance::OnCreateSessionComplete(FName SessionName, bool bSuccess)
{
	OnCreateSessionCompleted.Broadcast(bSuccess);
}

void UProxyInstance::OnFindSessionsComplete(bool bSuccess)
{
	if (bSuccess)
	{
		SessionResults = FilteredSessionResults();
		const TArray<FBlueprintSessionResult> ConvertResults = ConvertResultsToBlueprint();
		OnFindSessionsCompleted.Broadcast(bSuccess, ConvertResults);
		
		switch (FindSessionsCauses)
		{
			case EFindSessionsCauses::E_None:
				{
					return;
				}
			case EFindSessionsCauses::E_PendingCreation:
				{
					FindSessionsCauses = EFindSessionsCauses::E_None;
					FString Code = GenerateNewSessionCode();
					
					while (DoesCodeAlreadyExist(Code))
					{
						Code = GenerateNewSessionCode();
					}
					
					SessionCode = Code;
					NativeCreateSession(SessionCode);
					return;
				}
		case EFindSessionsCauses::E_JoinSession:
				{
					const FOnlineSessionSearchResult Session = GetSessionByCode(SessionCode);
					SessionInterface.Pin()->JoinSession(0, NAME_GameSession, Session);
					return;
				}
		}
	}
}

void UProxyInstance::OnUpdateSessionComplete(FName SessionName, bool bSuccess)
{
	OnUpdateSessionCompleted.Broadcast(bSuccess);
}

void UProxyInstance::OnJoinSessionComplete(const FName SessionName, const EOnJoinSessionCompleteResult::Type Result)
{
	const uint8 Code = static_cast<uint8>(Result);
	OnJoinSessionCompleted.Broadcast(SessionName, Code);
	if (APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FString JoinAddress = "";
		SessionInterface.Pin()->GetResolvedConnectString(SessionName, JoinAddress);
		if (JoinAddress != "")
		{
			PController->ClientTravel(JoinAddress, TRAVEL_Absolute);
		}
	}
}

void UProxyInstance::OnLeaveSessionComplete(FName SessionName, bool bSuccess)
{
	OnLeaveSessionCompleted.Broadcast(bSuccess);
}

FOnlineSessionSearchResult UProxyInstance::GetSessionByCode(const FString Code)
{
	if (SessionResults.Num())
	{
		for (auto Result : SessionResults)
		{
			if (FString OtherCode; Result.Session.SessionSettings.Get("SESSION_CODE", OtherCode))
			{
				if (OtherCode == Code)
				{
					return Result;
				}
			}
		}
	}
	return FOnlineSessionSearchResult();
}

FString UProxyInstance::GenerateNewSessionCode()
{
	return FGuid::NewGuid().ToString(EGuidFormats::Digits).Left(6).ToUpper();
}

bool UProxyInstance::DoesCodeAlreadyExist(const FString& Code) const
{
	for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults)
	{
		if (FString OtherCode; Result.Session.SessionSettings.Get("SESSION_CODE", OtherCode))
		{
			if (OtherCode == Code)
			{
				return true;
			}
		}
	}
	return false;
}

TArray<FBlueprintSessionResult> UProxyInstance::ConvertResultsToBlueprint()
{
	TArray<FBlueprintSessionResult> Results;
	if (SessionResults.Num())
	{
		for (const auto Result : SessionResults)
		{
			FBlueprintSessionResult NewResult;
			NewResult.OnlineResult = Result;
			Results.Add(NewResult);
		}
	}
	return Results;
}

TArray<FOnlineSessionSearchResult> UProxyInstance::FilteredSessionResults() const
{
	TArray<FOnlineSessionSearchResult> Results;
	
	for (FOnlineSessionSearchResult Result : SessionSearch->SearchResults)
	{
		bool bPublic = true;
		if (Result.Session.SessionSettings.Get("IS_PUBLIC", bPublic))
		{
			if (bPublic == bIsPublic)
			{
				Results.Add(Result);
			}
		}
	}
	return Results;
}
