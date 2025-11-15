// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
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
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject", Category = "Online|Network", DisplayName="Update Session", 
		AdvancedDisplay="bUseLAN,bAllowInvites,bAllowJoinInProgress,bRefreshOnlineData,bIsDedicatedServer,bShouldAdvertise,bAllowJoinViaPresence,bAllowJoinViaPresenceFriendsOnly"))
	static UUpdateSessionAsyncAction* Create(UObject* WorldContextObject, int32 PublicConnections = 4, 
		int32 PrivateConnections = 0, bool bUseLAN = false, 
		bool bAllowInvites = false, bool bAllowJoinInProgress = false, 
		bool bRefreshOnlineData = true, bool bIsDedicatedServer = false, 
		bool bShouldAdvertise = true, bool bAllowJoinViaPresence = true, bool bAllowJoinViaPresenceFriendsOnly = false);
	
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
