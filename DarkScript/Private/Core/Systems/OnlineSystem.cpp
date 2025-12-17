#include "Core/Systems/OnlineSystem.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/AbilitySystem/GlobalAbilitySystem.h"
#include "Gameplay/AbilitySystem/GlobalAttributeSet.h"
#include "Utils/Types/OnlineTypes.h"
#include "Utils/Helpers/SystemsHelper.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
void UOnlineSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (GetWorld())
	{
		UE_LOG(DarkScript, Warning, TEXT("UOnlineSystem::Initialize - Start Timer"))
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			PlayerController = GetWorld()->GetFirstPlayerController();
			GlobalAbilitySystem = UGlobalAbilitySystem::GetSystem(PlayerController.Get());
			
			UE_LOG(DarkScript, Warning, TEXT("UOnlineSystem::Initialize - PlayerController is %s | GlobalAbilitySystem is %s"), 
				PlayerController.Get() ? *PlayerController.Get()->GetName() : TEXT("not valid"),
				GlobalAbilitySystem.Get() ? *GlobalAbilitySystem.Get()->GetName() : TEXT("not valid"));
			
			if (FOnlineHelper Helper = FOnlineHelper(this); Helper.OnlineSub)
			{
				Helper.QueryIDFromPlayerController(PlayerController.Get());
				SteamID = Helper.UserID;
				SessionInterface = Helper.OnlineSub->GetSessionInterface();
				
				if (SessionInterface.IsValid())
				{
					OnCreateSessionCompleteHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
						FOnCreateSessionCompleteDelegate::CreateUObject(this, &UOnlineSystem::OnCreateSessionComplete));
					OnFindSessionsCompleteHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
						FOnFindSessionsCompleteDelegate::CreateUObject(this, &UOnlineSystem::OnFindSessionsComplete));
					OnJoinSessionCompleteHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
						FOnJoinSessionCompleteDelegate::CreateUObject(this, &UOnlineSystem::OnJoinSessionComplete));
					OnDestroySessionCompleteHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
						FOnDestroySessionCompleteDelegate::CreateUObject(this, &UOnlineSystem::OnDestroySessionComplete));
				}
			}
		}), 1.f, false);
	}
}

void UOnlineSystem::Deinitialize()
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteHandle);
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteHandle);
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteHandle);
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteHandle);
	}
	Super::Deinitialize();
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
void UOnlineSystem::CreateSession(FDarkSessionSettings& Settings)
{
	if (!SessionInterface.IsValid())
	{
		OnSessionCreated.Broadcast(false);
		return;
	}

	if (const auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession); ExistingSession != nullptr)
	{
		SessionInterface->DestroySession(NAME_GameSession);
	}
	
	if (Settings.SessionType == ESessionType::QuickMatch)
	{
		Settings.AllowedArchetypes = GetAllArchetypes();
	}
	Settings.AvailableArchetypes = Settings.AllowedArchetypes;
		
	FOnlineSessionSettings OnlineSettings;
	SetupSessionSettings(OnlineSettings, Settings);

	if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController(); !LocalPlayer)
	{
		OnSessionCreated.Broadcast(false);
		return;
	}
	
	SessionSettings = Settings;
		
	if (Settings.SessionType == ESessionType::Private)
	{
		CreatePrivateSession(Settings);
		return;
	}
	
	if (!SessionInterface->CreateSession(0, NAME_GameSession, OnlineSettings))
	{
		OnSessionCreated.Broadcast(false);
	}
}

void UOnlineSystem::CreatePrivateSession(FDarkSessionSettings& Settings)
{
	PendingSessionCode = GenerateSessionCode();
	Settings.SessionCode = PendingSessionCode;
	bCheckingCode = true;
	
	TSharedRef<FOnlineSessionSearch> CodeCheckSearch = MakeShareable(new FOnlineSessionSearch());
	CodeCheckSearch->MaxSearchResults = 100;
	CodeCheckSearch->bIsLanQuery = false;
	CodeCheckSearch->QuerySettings.Set("PRESENCESEARCH", true, EOnlineComparisonOp::Equals);
	
	TSharedPtr<FDelegateHandle> TempHandlePtr = MakeShareable(new FDelegateHandle);
	
	*TempHandlePtr = SessionInterface->OnFindSessionsCompleteDelegates.AddLambda(
		[this, CodeCheckSearch, TempHandlePtr] (bool bWasSuccessful) mutable
		{
			if (TempHandlePtr.IsValid() && TempHandlePtr->IsValid())
			{
				SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(*TempHandlePtr);
			}
			
			bCheckingCode = false;
			
			if (bWasSuccessful && CodeCheckSearch->SearchResults.Num() > 0)
			{
				bool bCodeExists = false;
				for (const FOnlineSessionSearchResult& Result : CodeCheckSearch->SearchResults)
				{
					FString ExistingCode;
					if (int32 SessionIndex = 0; Result.Session.SessionSettings.Get(SessionSettingsKeys::SESSION_TYPE, SessionIndex))
					{
						if (IntToSessionType(SessionIndex) != ESessionType::Private) continue;
					}
					Result.Session.SessionSettings.Get(SessionSettingsKeys::SESSION_CODE, ExistingCode);
					
					if (ExistingCode == PendingSessionCode)
					{
						bCodeExists = true;
						break;
					}
				}
				
				if (bCodeExists)
				{
					FDarkSessionSettings NewSettings = SessionSettings;
					CreatePrivateSession(NewSettings);
					return;
				}
			}
			SessionSettings.SessionCode = PendingSessionCode;
			FOnlineSessionSettings OnlineSettings;
			SetupSessionSettings(OnlineSettings, SessionSettings);
			SessionInterface->CreateSession(0, NAME_GameSession, OnlineSettings);
		});
	SessionInterface->FindSessions(0, CodeCheckSearch);
}

void UOnlineSystem::FindSession(const FSessionFilters& Filters)
{
	LastJoinError = ESessionJoinError::None;
	if (!SessionInterface.IsValid() || !GetWorld()->GetFirstLocalPlayerFromController())
	{
		LastJoinError = ESessionJoinError::NetworkError;
		OnSessionSearchComplete.Broadcast(TArray<FSessionSearchResult>(), false, LastJoinError);
		return;
	}
	
	CurrentSessionFilters = Filters;
	
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 100;
	SessionSearch->bIsLanQuery = false;
	SessionSearch->QuerySettings.Set("PRESENCESEARCH", true, EOnlineComparisonOp::Equals);
	
	if (!SessionInterface->FindSessions(0, SessionSearch.ToSharedRef()))
	{
		LastJoinError = ESessionJoinError::SearchFailed;
		OnSessionSearchComplete.Broadcast(TArray<FSessionSearchResult>(), false, LastJoinError);
	}
}

void UOnlineSystem::JoinSession(const FSessionSearchResult& SearchResult)
{
	LastJoinError = ESessionJoinError::None;

	if (!SessionInterface.IsValid() || !GetWorld()->GetFirstLocalPlayerFromController())
	{
		LastJoinError = ESessionJoinError::NetworkError;
		OnSessionJoined.Broadcast(false, LastJoinError);
		return;
	}
	ValidityCheckingResult = {true, SearchResult};
	FindSession(FSessionFilters());
}

void UOnlineSystem::JoinSessionByCode(const FString& SessionCode)
{
	FSessionFilters Filters;
	Filters.SessionType = ESessionType::Private;
	Filters.SessionCode = SessionCode;
	
	FindSession(Filters);
}

void UOnlineSystem::LeaveSession() const
{
	if (!SessionInterface.IsValid())
	{
		return;
	}
	if (bIsHost)
	{
		if (const UWorld* World = GetWorld())
		{
			if (const AGameModeBase* GameMode = World->GetAuthGameMode())
			{
				if (AGameSession* GameSession = GameMode->GameSession)
				{
					for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
					{
						if (APlayerController* PC = It->Get())
						{
							if (!PC->IsLocalController())
							{
								GameSession->KickPlayer(PC, GetKickReasonText(EKickReason::HostLeft));
							}
						}
					}
				}
			}
		}
	}
	SessionInterface->DestroySession(NAME_GameSession);
}

void UOnlineSystem::UpdateSessionSettings(const FDarkSessionSettings& Settings)
{
	if (!SessionInterface.IsValid() || !bIsHost) return;
	
	SessionSettings = Settings;
	
	FOnlineSessionSettings UpdatedSettings;
	SetupSessionSettings(UpdatedSettings, Settings);
	SessionInterface->UpdateSession(NAME_GameSession, UpdatedSettings);
}

UOnlineSystem* UOnlineSystem::GetSystem(UObject* WorldContextObject)
{
	if (!WorldContextObject)
		return nullptr;

	const AController* PC = nullptr;
	if (const ACharacter* Character = Cast<ACharacter>(WorldContextObject))
	{
		PC = Character->GetController();
	}
	else if (const APlayerState* PS = Cast<APlayerState>(WorldContextObject))
	{
		PC = PS->GetOwningController();
	}
	
	if (!PC)
	{
		PC = Cast<APlayerController>(WorldContextObject);
	}
	if (PC)
	{
		if (UOnlineSystem* OnlineSystem = PC->GetGameInstance()->GetSubsystem<UOnlineSystem>())
		{
			return OnlineSystem;
		}
	}
	return nullptr;
}

FString UOnlineSystem::GenerateSessionCode()
{
	// Caractères lisibles : pas de O/0, I/1/L, ni de caractères ambigus
	const TCHAR* Characters = TEXT("23456789ABCDEFGHJKMNPQRSTUVWXYZ");
	const int32 CharCount = FCString::Strlen(Characters);
	
	FString Code;
	Code.Reserve(6);
	
	for (int32 i = 0; i < 6; i++)
	{
		const int32 RandomIndex = FMath::RandRange(0, CharCount - 1);
		Code.AppendChar(Characters[RandomIndex]);
	}
	
	return Code;
}

void UOnlineSystem::SetupSessionSettings(FOnlineSessionSettings& OnlineSettings, const FDarkSessionSettings& Settings)
{
	OnlineSettings.NumPublicConnections = (Settings.SessionType == ESessionType::Private) ? Settings.MaxPlayers : 4;
	OnlineSettings.NumPrivateConnections = 0;
	OnlineSettings.bIsLANMatch = false;
	OnlineSettings.bUsesPresence = true;
	OnlineSettings.bAllowJoinInProgress = true;
	OnlineSettings.bAllowInvites = true;
	OnlineSettings.bShouldAdvertise = true;
	OnlineSettings.bAllowJoinViaPresence = false;
	OnlineSettings.bAllowJoinViaPresenceFriendsOnly = true;
	OnlineSettings.bUseLobbiesIfAvailable = false;
	
	OnlineSettings.Set(SessionSettingsKeys::SESSION_TYPE, static_cast<int32>(Settings.SessionType), EOnlineDataAdvertisementType::ViaOnlineService);
	OnlineSettings.Set(SessionSettingsKeys::SESSION_CUSTOM_NAME, Settings.SessionName, EOnlineDataAdvertisementType::ViaOnlineService);
	OnlineSettings.Set(SessionSettingsKeys::SESSION_CODE, Settings.SessionCode, EOnlineDataAdvertisementType::ViaOnlineService);
	OnlineSettings.Set(SessionSettingsKeys::MIN_LEVEL_REQUIRED, Settings.MinLevelRequired, EOnlineDataAdvertisementType::ViaOnlineService);
		
	FString ArchetypesAllowed = ArchetypesToString(Settings.AllowedArchetypes);
	OnlineSettings.Set(SessionSettingsKeys::ALLOWED_ARCHETYPES, ArchetypesAllowed, EOnlineDataAdvertisementType::ViaOnlineService);
	FString AvailableArchetypes = ArchetypesToString(Settings.AvailableArchetypes);
	OnlineSettings.Set(SessionSettingsKeys::AVAILABLE_ARCHETYPES, AvailableArchetypes, EOnlineDataAdvertisementType::ViaOnlineService);
}

FSessionSearchResult UOnlineSystem::ConvertSearchResult(const FOnlineSessionSearchResult& OnlineResult)
{
	FSessionSearchResult Result;
	Result.OnlineResults = OnlineResult;
	
	Result.CurrentPlayers = OnlineResult.Session.SessionSettings.NumPublicConnections - OnlineResult.Session.NumOpenPublicConnections;
	Result.MaxPlayers = OnlineResult.Session.SessionSettings.NumPublicConnections;
	Result.Ping = OnlineResult.PingInMs;
	
	FString SessionName;
	OnlineResult.Session.SessionSettings.Get(SessionSettingsKeys::SESSION_CUSTOM_NAME, SessionName);
	Result.SessionName = SessionName;
	
	FString SessionCode;
	OnlineResult.Session.SessionSettings.Get(SessionSettingsKeys::SESSION_CODE, SessionCode);
	Result.SessionCode = SessionCode;
	
	FString AllowedArchetypes;
	OnlineResult.Session.SessionSettings.Get(SessionSettingsKeys::ALLOWED_ARCHETYPES, AllowedArchetypes);
	Result.AllowedArchetypes = StringToArchetypes(AllowedArchetypes);
	
	FString AvailableArchetypes;
	OnlineResult.Session.SessionSettings.Get(SessionSettingsKeys::AVAILABLE_ARCHETYPES, AvailableArchetypes);
	Result.AvailableArchetypes = StringToArchetypes(AvailableArchetypes);
	
	int32 SessionType;
	OnlineResult.Session.SessionSettings.Get(SessionSettingsKeys::SESSION_TYPE, SessionType);
	Result.SessionType = IntToSessionType(SessionType);
	return Result;
}

void UOnlineSystem::ToFilteredResults(TArray<FSessionSearchResult>& SearchResults, const FSessionFilters& Filters) const
{
	TArray<FSessionSearchResult> Results;
	for (const FSessionSearchResult& SearchResult : SearchResults)
	{
		if (Filters.SessionType == ESessionType::Private)
		{
			if (SearchResult.SessionType != Filters.SessionType) continue;
		}
		else
		{
			if (SearchResult.SessionType == ESessionType::Private) continue;
		}
		if (SearchResult.CurrentPlayers < Filters.MinPlayers) continue;
		if (SearchResult.CurrentPlayers >= SearchResult.MaxPlayers) continue;
		if (SearchResult.MinLevelRequired < Filters.MinLevelRequired) continue;
		if (SearchResult.SessionCode != Filters.SessionCode) continue;
		if (Filters.Archetypes.Num() > 0)
		{
			bool bIsEligible = false;
			if (SearchResult.AllowedArchetypes.Num() > 0)
			{
				EClassArchetype CurrentPlayerArchetype = EClassArchetype::Tank;
				if (GlobalAbilitySystem)
				{
					CurrentPlayerArchetype = GlobalAbilitySystem->GetClassArchetype();
				}
				bool bCurrentArchetypeIsValid = false;
				if (Filters.Archetypes.Contains(CurrentPlayerArchetype) && 
					SearchResult.AllowedArchetypes.Contains(CurrentPlayerArchetype) &&
					SearchResult.AvailableArchetypes.Contains(CurrentPlayerArchetype))
				{
					bCurrentArchetypeIsValid = true;
					bIsEligible = true;
				}
				
				if (!bCurrentArchetypeIsValid)
				{
					for (const EClassArchetype& FilterArchetype : Filters.Archetypes)
					{
						if (SearchResult.AllowedArchetypes.Contains(FilterArchetype) && 
							SearchResult.AvailableArchetypes.Contains(FilterArchetype))
						{
							bIsEligible = true;
						
							if (Filters.SessionType == ESessionType::QuickMatch || Filters.SessionType == ESessionType::Public)
							{
								if (GlobalAbilitySystem)
								{
									GlobalAbilitySystem->SwitchArchetype(FilterArchetype);
								}
							}
							break;
						}
					}
				}
				
			}
			if (!bIsEligible) continue;
		}
		Results.Add(SearchResult);
	}
	SearchResults = Results;
}

bool UOnlineSystem::IsValidBeforeJoin(const FSessionSearchResult& Result, const TArray<FSessionSearchResult>& Results,
	ESessionJoinError& Error) const
{
	Error = ESessionJoinError::None;
	
	const FSessionSearchResult* FoundSession = Results.FindByPredicate([&Result](const FSessionSearchResult& SearchResult)
	{
		return Result.OnlineResults.Session.SessionInfo->GetSessionId() ==
			   SearchResult.OnlineResults.Session.SessionInfo->GetSessionId();
	});
	
	if (!FoundSession)
	{
		Error = ESessionJoinError::SessionNotFound;
		return false;
	}
	
	int32 PlayerLevel = 0;
	EClassArchetype PlayerArchetype = EClassArchetype::Tank;
	if (GlobalAbilitySystem)
	{
		PlayerLevel = GlobalAbilitySystem->GetNumericAttribute(UGlobalAttributeSet::GetLevelAttribute());
		PlayerArchetype = GlobalAbilitySystem->GetClassArchetype();
	}
	
	if (PlayerLevel < FoundSession->MinLevelRequired)
	{
		Error = ESessionJoinError::PlayerLevelTooLow;
		return false;
	}
	
	if (FoundSession->CurrentPlayers >= FoundSession->MaxPlayers)
	{
		Error = ESessionJoinError::SessionFull;
		return false;
	}
	
	if (FoundSession->AllowedArchetypes.Num() > 0)
	{
		if (!FoundSession->AllowedArchetypes.Contains(PlayerArchetype))
		{
			Error = ESessionJoinError::ArchetypeNotAllowed;
			return false;
		}
		if (!FoundSession->AvailableArchetypes.Contains(PlayerArchetype))
		{
			Error = ESessionJoinError::ArchetypeNotAvailable;
			return false;
		}
	}
	return true;
}


void UOnlineSystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	bIsHost = true;
	bInSession = bWasSuccessful;
	OnSessionCreated.Broadcast(bWasSuccessful);
}

void UOnlineSystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (bCheckingCode)
	{
		return;
	}
	
	TArray<FSessionSearchResult> Results;
	
	if (bWasSuccessful && SessionSearch.IsValid())
	{
		for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
		{
			FSessionSearchResult Result = ConvertSearchResult(SearchResult);
			Results.Add(Result);
		}
		
		if (ValidityCheckingResult.Key == true)
		{
			if (IsValidBeforeJoin(ValidityCheckingResult.Value, Results, LastJoinError))
			{
				OnSessionJoined.Broadcast(true, LastJoinError);
				ValidityCheckingResult.Key = false;
				ValidityCheckingResult.Value = FSessionSearchResult();
				SessionInterface->JoinSession(0, NAME_GameSession, Results[0].OnlineResults);
				return;
			}
			ValidityCheckingResult.Key = false;
			ValidityCheckingResult.Value = FSessionSearchResult();
			OnSessionJoined.Broadcast(false, LastJoinError);
			return;
		}
		
		switch (CurrentSessionFilters.SessionType)
		{
		case ESessionType::Public:
			{
				ToFilteredResults(Results, CurrentSessionFilters);
				if (Results.Num() > 0)
				{
					break;
				}
				LastJoinError = ESessionJoinError::NoSessionsFound;
				OnSessionSearchComplete.Broadcast(TArray<FSessionSearchResult>(), false, LastJoinError);
				return;
			}
		case ESessionType::Private:
			{
				ToFilteredResults(Results, CurrentSessionFilters);
				if (Results.Num() > 0)
				{
					bIsHost = false;
					OnSessionSearchComplete.Broadcast(Results, true, LastJoinError);
					SessionInterface->JoinSession(0, NAME_GameSession, Results[0].OnlineResults);
					return;
				}
				LastJoinError = ESessionJoinError::NoSessionsFound;
				OnSessionJoined.Broadcast(false, LastJoinError);
				OnSessionSearchComplete.Broadcast(TArray<FSessionSearchResult>(), false, LastJoinError);
				return;
			}
		case ESessionType::QuickMatch:
			{
				ToFilteredResults(Results, CurrentSessionFilters);
				if (Results.Num() > 0)
				{
					Results.Sort([this] (const FSessionSearchResult& A, const FSessionSearchResult& B)
					{
						return A.Ping < B.Ping;
					});
					bIsHost = false;
					OnSessionSearchComplete.Broadcast(Results, true, LastJoinError);
					SessionInterface->JoinSession(0, NAME_GameSession, Results[0].OnlineResults);
					return;
				}
				LastJoinError = ESessionJoinError::NoSessionsFound;
				OnSessionSearchComplete.Broadcast(TArray<FSessionSearchResult>(), false, LastJoinError);
				FDarkSessionSettings Settings;
				Settings.SessionType = ESessionType::QuickMatch;
				CreateSession(Settings);
				return;
			}
		case ESessionType::None:
			{
				LastJoinError = ESessionJoinError::Unknown;
				OnSessionSearchComplete.Broadcast(TArray<FSessionSearchResult>(), false, LastJoinError);
				return;
			}
		}
		if (Results.Num() == 0)
		{
			LastJoinError = ESessionJoinError::NoSessionsFound;
		}
	}
	else
	{
		LastJoinError = ESessionJoinError::SearchFailed;
	}
	OnSessionSearchComplete.Broadcast(Results, bWasSuccessful, LastJoinError);
}

void UOnlineSystem::OnJoinSessionComplete(const FName SessionName, const EOnJoinSessionCompleteResult::Type Result)
{
	const bool bWasSuccessful = (Result == EOnJoinSessionCompleteResult::Success);
	
	if (bWasSuccessful)
	{
		bInSession = true;
		LastJoinError = ESessionJoinError::None;

		if (FString ConnectInfo; SessionInterface->GetResolvedConnectString(SessionName, ConnectInfo))
		{
			if (PlayerController)
			{
				FString TravelURL = ConnectInfo;
				
				if (GlobalAbilitySystem)
				{
					EClassArchetype PlayerArchetype = GlobalAbilitySystem->GetClassArchetype();
					const int32 PlayerLevel = GlobalAbilitySystem->GetNumericAttribute(UGlobalAttributeSet::GetLevelAttribute());
					
					TravelURL += FString::Printf(TEXT("?Archetype=%d&Level=%d"), 
						static_cast<int32>(PlayerArchetype), 
						PlayerLevel);
					
					UE_LOG(DarkScript, Log, TEXT("Joining with Archetype: %s, Level: %d"), 
					  *UEnum::GetValueAsString(PlayerArchetype), PlayerLevel);
				}
				PlayerController->ClientTravel(TravelURL, TRAVEL_Absolute);
			}
		}
		return;
	}
	switch (Result)
	{
	case EOnJoinSessionCompleteResult::SessionIsFull:
		LastJoinError = ESessionJoinError::SessionFull;
		break;
	case EOnJoinSessionCompleteResult::SessionDoesNotExist:
		LastJoinError = ESessionJoinError::SessionNotFound;
		break;
	case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
		LastJoinError = ESessionJoinError::ConnectionFailed;
		break;
	case EOnJoinSessionCompleteResult::AlreadyInSession:
		LastJoinError = ESessionJoinError::JoinFailed;
		break;
	case EOnJoinSessionCompleteResult::UnknownError:
	default:
		LastJoinError = ESessionJoinError::Unknown;
		break;
	}
	OnSessionJoined.Broadcast(bWasSuccessful, LastJoinError);
}

void UOnlineSystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		bInSession = false;
		bIsHost = false;
		
		if (PlayerController)
		{
			PlayerController->ClientTravel(LevelPaths::Menu, TRAVEL_Absolute);
		}
	}
	OnSessionDestroyed.Broadcast(bWasSuccessful);
}


