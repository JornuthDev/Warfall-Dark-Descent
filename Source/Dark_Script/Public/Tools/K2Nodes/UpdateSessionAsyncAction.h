// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "Tools/Types/ProxyTypes.h"
#include "UpdateSessionAsyncAction.generated.h"

/**
 * 
 */
UCLASS()
class DARK_SCRIPT_API UUpdateSessionAsyncAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
/**
 * Updates an existing online session with new configuration and session metadata.
 *
 * This async action modifies the active session's settings, including connection limits,
 * invite rules, advertising visibility, LAN mode, and any additional key/value pairs.
 * When the update completes, either OnSuccess or OnFailure is broadcast.
 *
 * @param WorldContextObject                   Context object used to retrieve the world and OnlineSubsystem.
 * @param ExtraSettings                        Array of additional key/value session properties to update.
 * @param PublicConnections                    Maximum number of public connections.
 * @param PrivateConnections                   Maximum number of private connections.
 * @param bUseLAN                              Enables LAN mode instead of online mode.
 * @param bAllowInvites                        Allows players to invite others into the session.
 * @param bAllowJoinInProgress                 Allows joining after the session has begun.
 * @param bRefreshOnlineData                   Forces the OnlineSubsystem to refresh metadata.
 * @param bIsDedicatedServer                   Declares whether the host behaves as a dedicated server.
 * @param bShouldAdvertise                     Makes the session visible in searches.
 * @param bAllowJoinViaPresence                Allows joining through presence-based invites.
 * @param bAllowJoinViaPresenceFriendsOnly     Restricts presence-based join permissions to friends only.
 *
 * @return                                     An UpdateSessionAsyncAction instance that handles the asynchronous operation.
 */
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject", AutoCreateRefTerm="ExtraSettings", DisplayName="Update Session", 
		AdvancedDisplay="bUseLAN,bAllowInvites,bAllowJoinInProgress,bRefreshOnlineData,bIsDedicatedServer,bShouldAdvertise,bAllowJoinViaPresence,bAllowJoinViaPresenceFriendsOnly"), Category = "Online|Network")
	static UUpdateSessionAsyncAction* Create(UObject* WorldContextObject, const TArray<FSessionPropertyKeyPair> ExtraSettings, 
		int32 PublicConnections = 4, int32 PrivateConnections = 0, 
		bool bUseLAN = false, bool bAllowInvites = false, 
		bool bAllowJoinInProgress = false, bool bRefreshOnlineData = true, 
		bool bIsDedicatedServer = false, bool bShouldAdvertise = true, 
		bool bAllowJoinViaPresence = true, bool bAllowJoinViaPresenceFriendsOnly = false);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	virtual void Activate() override;
	UFUNCTION()
	void OnUpdateSessionCompleted(const bool bSuccess);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnSuccess;
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnFailure;
	
	TWeakObjectPtr<UObject> WorldContextObject;
	TArray<FSessionPropertyKeyPair> ExtraSettings;
	int32 PublicConnections = 4;
	int32 PrivateConnections = 0;
	bool bUseLAN = false;
	bool bAllowInvites = false;
	bool bAllowJoinInProgress = false;
	bool bRefreshOnlineData = false;
	bool bIsDedicatedServer = false;
	bool bShouldAdvertise = false;
	bool bAllowJoinViaPresence = false;
	bool bAllowJoinViaPresenceFriendsOnly = false;
	
};
