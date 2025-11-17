#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OnlineSessionSettings.h"
#include "FindSessionsCallbackProxy.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Tools/Types/ProxyTypes.h"
#include "ProxyInstance.generated.h"

enum class EFindSessionsCauses : uint8
{
	E_None,
	E_PendingCreation,
	E_JoinSession,
};

/**
 * 
 */
UCLASS()
class DARK_SCRIPT_API UProxyInstance : public UGameInstanceSubsystem
{
	GENERATED_BODY()
private:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Delegates ─────────────────────────────────────────────────────────────────
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnCreateSessionNative, bool /*bWasSuccessful*/);
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnFindSessionsNative,	bool /*bWasSuccessful*/, const TArray<FBlueprintSessionResult>& /*Results*/);
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnJoinSessionNative, FName, uint8);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	FString GetSessionCode() const { return SessionCode; }
	bool GetSessionSettings(int32& NumConnections, int32& NumPrivateConnections, bool& bIsLAN, bool& bAllowInvites, bool& bAllowJoinInProgress, bool& bIsAnticheatEnabled, int32& BuildUniqueID, TArray<FSessionPropertyKeyPair>& OutExtraSettings) const;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	void CreateSession(const TArray<FSessionPropertyKeyPair>& InExtraSettings, const bool bInIsPublic = true, const int32 InMaxPlayers = 4);
	void FindSessions(const TArray<FSessionPropertyKeyPair>& InFilters, const ESessionVisibility Visibility = ESessionVisibility::E_All);
	void UpdateSession(int32 PublicConnections = 4, int32 PrivateConnections = 0, bool bUseLAN = false, bool bAllowInvites = false, 
		bool bAllowJoinInProgress = false, bool bIsDedicatedServer = false, bool bShouldAdvertise = true, bool bAllowJoinViaPresence = true, 
		bool bAllowJoinViaPresenceFriendsOnly = false, const TArray<FSessionPropertyKeyPair>& InExtraSettings = TArray<FSessionPropertyKeyPair>(), const bool bRefreshOnlineData = true);
	void JoinSessionByCode(const FString Code);
	void LeaveSession() const;
	
	void DestroySession() const;
	bool IsOnline() const;
	bool IsPublic() const { return SessionVisibility == ESessionVisibility::E_Public; }
	bool IsServer() const
	{
		const UWorld* World = GetWorld();
		return World && World->GetNetMode() != NM_Client;
	}
	
	void OpenLobby(const bool bSuccess);
	
protected:
	void NativeCreateSession(const FString& Code = "") const;
	void NativeFindSession(const EFindSessionsCauses Causes = EFindSessionsCauses::E_None);
	void NativeUpdateSession(FOnlineSessionSettings Settings, const bool bRefreshOnlineData) const;
	void NativeLeaveSession() const;
	void OnCreateSessionComplete(FName SessionName, bool bSuccess);
	void OnFindSessionsComplete(bool bSuccess);
	void OnUpdateSessionComplete(FName SessionName, bool bSuccess);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnLeaveSessionComplete(FName SessionName, bool bSuccess);
	
	FOnlineSessionSearchResult GetSessionByCode(const FString Code = "");
	
	static FString GenerateNewSessionCode();
	bool DoesCodeAlreadyExist(const FString& Code) const;
	
	FString ConvertVisibilityToParameter() const;
	static ESessionVisibility ConvertParameterToVisibility(const FString& Parameter);
	
	TArray<FBlueprintSessionResult> ConvertResultsToBlueprint();
	TArray<FOnlineSessionSearchResult> FilteredSessionResults();
	
public:	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	FOnCreateSessionNative OnCreateSessionCompleted;
	FOnFindSessionsNative OnFindSessionsCompleted;
	FOnCreateSessionNative OnUpdateSessionCompleted;
	FOnJoinSessionComplete OnJoinSessionCompleted;
	FOnCreateSessionNative OnLeaveSessionCompleted;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Online|Session")
	FString SessionCode = "";
	
protected:	
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	TWeakPtr<IOnlineSession> SessionInterface;
	TArray<FOnlineSessionSearchResult> SessionResults;
	ESessionVisibility SessionVisibility = ESessionVisibility::E_Public;
	int32 MaxPlayers = 4;
	TArray<FSessionPropertyKeyPair> ExtraSettings ;
	TArray<FSessionPropertyKeyPair> Filters;
	
	
	EFindSessionsCauses FindSessionsCauses = EFindSessionsCauses::E_None;
};

namespace KickReason
{
	static const FString HostEnded = TEXT("HostEndedSession");
	static const FString ConnectionLost = TEXT("ConnectionLost");
	static const FString SessionFull = TEXT("SessionIsFull");
}