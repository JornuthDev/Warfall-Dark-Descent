#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "Tools/Types/ProxyTypes.h"
#include "CreateSessionAsyncAction.generated.h"

/**
 * 
 */
UCLASS()
class DARK_SCRIPT_API UCreateSessionAsyncAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
/**
 * Creates a new online session with optional advanced settings.
 *
 * This async action initializes an online session using the specified public/private visibility,
 * maximum allowed players, and any additional key/value settings supplied through ExtraSettings.
 * Once the session creation process completes, either OnSuccess or OnFailure will be triggered.
 *
 * @param WorldContextObject   Context object used to retrieve the world and OnlineSubsystem.
 * @param ExtraSettings        Array of key/value pairs added to the session's settings.
 * @param bIsPublic            Determines whether the session is publicly visible when searched.
 * @param MaxPlayers           Maximum number of allowed public connections.
 *
 * @return                     A CreateSessionAsyncAction instance that handles the asynchronous operation.
 */
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject", AutoCreateRefTerm="ExtraSettings", DisplayName="Create Advanced Session"), Category = "Online|Network")
	static UCreateSessionAsyncAction* Create(UObject* WorldContextObject,const TArray<FSessionPropertyKeyPair>& ExtraSettings, const bool bIsPublic = true, const int32 MaxPlayers = 4);
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	virtual void Activate() override;
	UFUNCTION()
	void OnCreateSessionCompleted(const bool bSuccess);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnSuccess;
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnFailure;
	
	TWeakObjectPtr<UObject> WorldContextObject;
	TArray<FSessionPropertyKeyPair> ExtraSettings;
	bool bIsPublic;
	int32 MaxPlayers;
};
