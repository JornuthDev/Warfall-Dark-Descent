#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Utils/Types/OnlineTypes.h"
#include "OnlineSystem.generated.h"

/**
 * 
 */
UCLASS()
class DARKSCRIPT_API UOnlineSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
private:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Delegates ─────────────────────────────────────────────────────────────────
	
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	UFUNCTION(BlueprintPure, Category="Online|Session")
	FString GetSessionCode() const
	{
		FString SessionCode = SessionSettings.SessionCode;
		return SessionCode;
	}
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	void CreateSession(FDarkSessionSettings& Settings);
	void CreatePrivateSession(FDarkSessionSettings& Settings);
	void FindSession(const FSessionFilters& Filters);
	void JoinSession(const FSessionSearchResult& SearchResult);
	void JoinSessionByCode(const FString& SessionCode);
	void LeaveSession() const;
	void UpdateSessionSettings(const FDarkSessionSettings& Settings);

	static FString GenerateSessionCode();
	ESessionType GetSessionType() const { return SessionSettings.SessionType; }
	static void SetupSessionSettings(FOnlineSessionSettings& OnlineSettings, const FDarkSessionSettings& Settings);
	static FSessionSearchResult ConvertSearchResult(const FOnlineSessionSearchResult& OnlineResult);
	void ToFilteredResults(TArray<FSessionSearchResult>& SearchResults, const FSessionFilters& Filters);
	bool IsValidBeforeJoin(const FSessionSearchResult& Result, const TArray<FSessionSearchResult>& Results, ESessionJoinError& Error);
	
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	FOnSessionCreated OnSessionCreated;
	FOnSessionJoined OnSessionJoined;
	FOnSessionDestroyed OnSessionDestroyed;
	FOnSessionSearchComplete OnSessionSearchComplete;
	
	FDelegateHandle OnCreateSessionCompleteHandle;
	FDelegateHandle OnFindSessionsCompleteHandle;
	FDelegateHandle OnJoinSessionCompleteHandle;
	FDelegateHandle OnDestroySessionCompleteHandle;
	
	// ReSharper disable once CppBoundToDelegateMethodIsNotMarkedAsUFunction
	IOnlineSessionPtr SessionInterface;
	
	bool bInSession = false;
	bool bIsHost = false;
	FString SteamID;
	
	// FlowControl
	TPair<bool, FSessionSearchResult> ValidityCheckingResult;
	bool bCheckingCode = false;
	FString PendingSessionCode;
	
	FDarkSessionSettings SessionSettings;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	int32 CurrentIndexFilters = INDEX_NONE;
	TMap<int32, FSessionFilters> SearchFilters;
	FSessionFilters CurrentSessionFilters;

	ESessionJoinError LastJoinError = ESessionJoinError::None;
	
	
};
