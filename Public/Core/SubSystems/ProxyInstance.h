#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OnlineSessionSettings.h"
#include "FindSessionsCallbackProxy.h"
#include "Interfaces/OnlineSessionInterface.h"
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
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	void CreateSession(const bool bInIsPublic = true, const int32 InMaxPlayers = 4);
	void FindSessions(const bool bInIsPublic = true);
	void UpdateSession(const FOnlineSessionSettings& Settings, const bool bRefreshOnlineData) const;
	void JoinSessionByCode(const FString Code);
	void LeaveSession() const;
	
	void DestroySession() const;
	bool IsPublic() const { return bIsPublic; }
	
protected:
	void NativeCreateSession(const FString Code = "") const;
	void NativeFindSession(const EFindSessionsCauses Causes = EFindSessionsCauses::E_None);
	void NativeUpdateSession(FOnlineSessionSettings Settings, const bool bRefreshOnlineData) const;
	void NativeLeaveSession(const FString KickReason = "") const;
	void OnCreateSessionComplete(FName SessionName, bool bSuccess);
	void OnFindSessionsComplete(bool bSuccess);
	void OnUpdateSessionComplete(FName SessionName, bool bSuccess);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnLeaveSessionComplete(FName SessionName, bool bSuccess);
	
	FOnlineSessionSearchResult GetSessionByCode(const FString Code = "");
	
	static FString GenerateNewSessionCode();
	bool DoesCodeAlreadyExist(const FString& Code) const;
	TArray<FBlueprintSessionResult> ConvertResultsToBlueprint();
	TArray<FOnlineSessionSearchResult> FilteredSessionResults() const;
	
	bool IsServer() const
	{
		const UWorld* World = GetWorld();
		return World && World->GetNetMode() != NM_Client;
	}
	
public:	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	FOnCreateSessionNative OnCreateSessionCompleted;
	FOnFindSessionsNative OnFindSessionsCompleted;
	FOnCreateSessionNative OnUpdateSessionCompleted;
	FOnJoinSessionNative OnJoinSessionCompleted;
	FOnCreateSessionNative OnLeaveSessionCompleted;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Online|Session")
	FString SessionCode = "";
	
protected:	
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	TWeakPtr<IOnlineSession> SessionInterface;
	TArray<FOnlineSessionSearchResult> SessionResults;
	
	EFindSessionsCauses FindSessionsCauses = EFindSessionsCauses::E_None;
	int32 MaxPlayers = 4;
	bool bIsPublic = true;
};
