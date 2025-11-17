#include "Core/SubSystems/ProxyInstance.h"

#include "OnlineSubsystemUtils.h"
#include "Core/Game/DarkGameState.h"
#include "Engine/World.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Tools/Paths.h"
#include "Tools/Libraries/ProxyLibrary.h"

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
// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
bool UProxyInstance::GetSessionSettings(int32& NumConnections, int32& NumPrivateConnections, bool& bIsLAN,
	bool& bAllowInvites, bool& bAllowJoinInProgress, bool& bIsAnticheatEnabled, int32& BuildUniqueID,
	TArray<FSessionPropertyKeyPair>& OutExtraSettings) const
{
	if (!IsOnline()) return false;

	if (SessionInterface.Pin())
	{
		if (FOnlineSessionSettings* Settings = SessionInterface.Pin()->GetSessionSettings(NAME_GameSession))
		{
			NumConnections = Settings->NumPublicConnections;
			NumPrivateConnections = Settings->NumPrivateConnections;
			bIsLAN = Settings->bIsLANMatch;
			bAllowInvites = Settings->bAllowInvites;
			bAllowJoinInProgress = Settings->bAllowJoinInProgress;
			bIsAnticheatEnabled = Settings->bAntiCheatProtected;
			BuildUniqueID = Settings->BuildUniqueId;
			
			FSessionPropertyKeyPair KeyPair;
			for (const auto& Elem : Settings->Settings)
			{
				KeyPair.Key = Elem.Key;
				KeyPair.Data = Elem.Value.Data;
				OutExtraSettings.Add(KeyPair);
			}
			return true;
		}
	}
	return false;
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
void UProxyInstance::CreateSession(const TArray<FSessionPropertyKeyPair>& InExtraSettings, const bool bInIsPublic, const int32 InMaxPlayers)
{
	MaxPlayers = InMaxPlayers;
	SessionVisibility = bInIsPublic ? ESessionVisibility::E_Public : ESessionVisibility::E_Private;
	ExtraSettings = InExtraSettings;
	if (bInIsPublic)
	{
		NativeCreateSession();
		return;
	}
	NativeFindSession(EFindSessionsCauses::E_PendingCreation);
}

void UProxyInstance::FindSessions(const TArray<FSessionPropertyKeyPair>& InFilters, const ESessionVisibility Visibility)
{
	SessionVisibility = Visibility;
	Filters = InFilters;
	NativeFindSession(EFindSessionsCauses::E_None);
}

void UProxyInstance::UpdateSession(const int32 PublicConnections, const int32 PrivateConnections, const bool bUseLAN, const bool bAllowInvites,
		const bool bAllowJoinInProgress, const bool bIsDedicatedServer, const bool bShouldAdvertise, const bool bAllowJoinViaPresence,
		const bool bAllowJoinViaPresenceFriendsOnly, const TArray<FSessionPropertyKeyPair>& InExtraSettings, const bool bRefreshOnlineData)
{
	FOnlineSessionSettings* Settings = SessionInterface.Pin()->GetSessionSettings(NAME_GameSession);
	FOnlineSessionSettings SessionSettings;
	if (Settings)
	{
		SessionSettings = *Settings;
	}
	SessionSettings.NumPublicConnections = PublicConnections;
	SessionSettings.NumPrivateConnections = PrivateConnections;
	SessionSettings.bIsLANMatch = bUseLAN;
	SessionSettings.bAllowInvites = bAllowInvites;
	SessionSettings.bAllowJoinInProgress = bAllowJoinInProgress;
	SessionSettings.bIsDedicated = bIsDedicatedServer;
	SessionSettings.bShouldAdvertise = bShouldAdvertise;
	SessionSettings.bAllowJoinViaPresence = bAllowJoinViaPresence;
	SessionSettings.bAllowJoinViaPresenceFriendsOnly = bAllowJoinViaPresenceFriendsOnly;
	
	FOnlineSessionSetting ExtraSetting;
	for (int32 i = 0; i < ExtraSettings.Num(); i++)
	{
		ExtraSetting.Data = ExtraSettings[i].Data;
		ExtraSetting.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineService;
		SessionSettings.Settings.Add(ExtraSettings[i].Key, ExtraSetting);
	}
	
	NativeUpdateSession(SessionSettings, bRefreshOnlineData);
}

void UProxyInstance::JoinSessionByCode(const FString Code)
{
	SessionCode = Code;
	SessionVisibility = ESessionVisibility::E_Private;
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

bool UProxyInstance::IsOnline() const
{
	if (SessionInterface.Pin())
	{
		const FNamedOnlineSession* CurrentSession = SessionInterface.Pin()->GetNamedSession(NAME_GameSession);
		return (CurrentSession != nullptr);
	}
	return false;
}

void UProxyInstance::OpenLobby(const bool bSuccess)
{
	if (bSuccess)
	{
		UGameplayStatics::OpenLevel(this, FName(LVL_LOBBY_PATH), true, "listen");
	}
}

void UProxyInstance::NativeCreateSession(const FString& Code) const
{
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bIsLANMatch = (Online::GetSubsystem(GetWorld())->GetSubsystemName() == "NULL");
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bAllowInvites = true;
	SessionSettings.NumPublicConnections = MaxPlayers;
	
	FString Visibility = ConvertVisibilityToParameter();
	
	FOnlineSessionSetting VisibilitySetting;
	VisibilitySetting.Data = Visibility;
	VisibilitySetting.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineService;
	SessionSettings.Settings.Add(FName("SEARCH_VISIBILITY"), VisibilitySetting);
	
	FOnlineSessionSetting CodeSetting;
	CodeSetting.Data = Code;
	CodeSetting.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineService;
	SessionSettings.Settings.Add(FName("SESSION_CODE"), CodeSetting);
	
	FOnlineSessionSetting ExtraSetting;
	for (int32 i = 0; i < ExtraSettings.Num(); i++)
	{
		ExtraSetting.Data = ExtraSettings[i].Data;
		ExtraSetting.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineService;
		SessionSettings.Settings.Add(ExtraSettings[i].Key, ExtraSetting);
	}
	
	SessionInterface.Pin()->CreateSession(0, NAME_GameSession, SessionSettings);
}


void UProxyInstance::NativeFindSession(const EFindSessionsCauses Causes)
{
	FindSessionsCauses = Causes;
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = (Online::GetSubsystem(GetWorld())->GetSubsystemName() == "NULL");
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set("SEARCH_PRESENCE", true, EOnlineComparisonOp::Equals);
	SessionSearch->QuerySettings.Set("SEARCH_VISIBILITY", ConvertVisibilityToParameter(), EOnlineComparisonOp::Equals);
	
	for (const FSessionPropertyKeyPair& Pair : Filters)
	{
		switch (Pair.Data.GetType())
		{
		case EOnlineKeyValuePairDataType::Int32:
			{
				int32 IntValue = 0;
				Pair.Data.GetValue(IntValue);
				SessionSearch->QuerySettings.Set(Pair.Key, IntValue, EOnlineComparisonOp::Equals);
				break;
			}
		case EOnlineKeyValuePairDataType::Bool:
			{
				bool bValue = false;
				Pair.Data.GetValue(bValue);
				SessionSearch->QuerySettings.Set(Pair.Key, bValue, EOnlineComparisonOp::Equals);
				break;
			}
		case EOnlineKeyValuePairDataType::String:
			{
				FString StrValue = "";
				Pair.Data.GetValue(StrValue);
				SessionSearch->QuerySettings.Set(Pair.Key, StrValue, EOnlineComparisonOp::Equals);
				break;
			}
		default:
			break;
		}
	}
	SessionInterface.Pin()->FindSessions(0, SessionSearch.ToSharedRef());
}

void UProxyInstance::NativeUpdateSession(FOnlineSessionSettings Settings, const bool bRefreshOnlineData) const
{
	FOnlineSessionSetting ExtraSetting;
	
	for (int32 i = 0; i < ExtraSettings.Num(); i++)
	{
		ExtraSetting.Data = ExtraSettings[i].Data;
		ExtraSetting.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineService;
		Settings.Settings.Add(ExtraSettings[i].Key, ExtraSetting);
	}
	SessionInterface.Pin()->UpdateSession(NAME_GameSession, Settings, bRefreshOnlineData);
}

void UProxyInstance::NativeLeaveSession() const
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
				UProxyLibrary::KickPlayer(GetWorld(), PController, KickReason::HostEnded);
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
	if (bSuccess) return;
	
	SessionCode = "";
	FindSessionsCauses = EFindSessionsCauses::E_None;
	SessionVisibility = ESessionVisibility::E_Public;
}

void UProxyInstance::OnFindSessionsComplete(bool bSuccess)
{
	
	if (bSuccess)
	{
		SessionResults = FilteredSessionResults();
		const TArray<FBlueprintSessionResult> ConvertResults = ConvertResultsToBlueprint();
		
		OnFindSessionsCompleted.Broadcast(true, ConvertResults);
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
	else
	{
		OnFindSessionsCompleted.Broadcast(false, TArray<FBlueprintSessionResult>());
		SessionCode = "";
		FindSessionsCauses = EFindSessionsCauses::E_None;
		SessionVisibility = ESessionVisibility::E_Public;
	}
}

void UProxyInstance::OnUpdateSessionComplete(FName SessionName, bool bSuccess)
{
	OnUpdateSessionCompleted.Broadcast(bSuccess);
}

void UProxyInstance::OnJoinSessionComplete(const FName SessionName, const EOnJoinSessionCompleteResult::Type Result)
{
	OnJoinSessionCompleted.Broadcast(SessionName, Result);
	if (APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FString JoinAddress = "";
		SessionInterface.Pin()->GetResolvedConnectString(SessionName, JoinAddress);
		if (JoinAddress != "")
		{
			PController->ClientTravel(JoinAddress, TRAVEL_Absolute);
		}
	}
	if (Result == EOnJoinSessionCompleteResult::CouldNotRetrieveAddress || 
		Result == EOnJoinSessionCompleteResult::SessionDoesNotExist ||
		Result == EOnJoinSessionCompleteResult::SessionIsFull ||
		Result == EOnJoinSessionCompleteResult::UnknownError)
	{
		SessionCode = "";
		FindSessionsCauses = EFindSessionsCauses::E_None;
		SessionVisibility = ESessionVisibility::E_Public;
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

FString UProxyInstance::ConvertVisibilityToParameter() const
{
	switch (SessionVisibility)
	{
	case ESessionVisibility::E_All:
		{
			break;
		}
	case ESessionVisibility::E_Public:
		{
			return "Public";
		}
		case ESessionVisibility::E_Private:
		{
			return "Private";
		}
	}
	return "";
}

ESessionVisibility UProxyInstance::ConvertParameterToVisibility(const FString& Parameter)
{
	if (Parameter.StartsWith("Private"))
	{
		return ESessionVisibility::E_Private;
	}
	return ESessionVisibility::E_Public;
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

TArray<FOnlineSessionSearchResult> UProxyInstance::FilteredSessionResults()
{
	TArray<FOnlineSessionSearchResult> Results;
	
	if (SessionVisibility != ESessionVisibility::E_All)
	{
		for (FOnlineSessionSearchResult Result : SessionSearch->SearchResults)
		{
			if (FString Visibility; Result.Session.SessionSettings.Get("SEARCH_VISIBILITY", Visibility))
			{
				if (SessionVisibility == ConvertParameterToVisibility(Visibility))
				{
					Results.Add(Result);
				}
			}
		}
	}
	else
	{
		Results = SessionSearch->SearchResults;
	}
	
	TArray<FOnlineSessionSearchResult> FinalResults;
	
	for (const auto& Result : Results)
	{
		bool bMatchesAll = true;
		
		for (const auto& [Key, Data] : Filters)
		{
			switch (Data.GetType())
			{
			case EOnlineKeyValuePairDataType::Int32:
				{
					int32 Expected = 0;
					Data.GetValue(Expected);
					
					int32 Actual = 0;
					if (!Result.Session.SessionSettings.Get(Key, Actual) || Actual != Expected)
					{
						bMatchesAll = false;
					}
					break;
				}
			case EOnlineKeyValuePairDataType::Bool:
				{
					bool Expected = false;
					Data.GetValue(Expected);
					
					bool bActual = false;
					if (!Result.Session.SessionSettings.Get(Key, bActual) || bActual != Expected)
					{
						bMatchesAll = false;
					}
					break;
				}
			case EOnlineKeyValuePairDataType::String:
				{
					FString Expected = "";
					Data.GetValue(Expected);
					
					FString Actual = "";
					if (!Result.Session.SessionSettings.Get(Key, Actual) || Actual != Expected)
					{
						bMatchesAll = false;
					}
					break;
				}
			default:
				bMatchesAll = false;
				break;
			}
			if (!bMatchesAll)
			{
				break;
			}
		}
		if (bMatchesAll)
		{
			FinalResults.Add(Result);
		}
	}
	return FinalResults;
}
