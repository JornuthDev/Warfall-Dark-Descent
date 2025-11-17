#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "FindSessionsCallbackProxy.h"
#include "Tools/Types/ProxyTypes.h"
#include "FindSessionAsyncAction.generated.h"

/**
 * 
 */
UCLASS()
class DARK_SCRIPT_API UFindSessionAsyncAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	/**
	 * Searches for online sessions using advanced filters.
	 *
	 * This async action performs an online session search with optional visibility filtering
	 * and key/value comparison against session settings.  
	 * When the search completes, the node returns any valid found sessions through either
	 * OnSuccess or OnFailure.
	 *
	 * @param WorldContextObject   Context object used to retrieve the world and OnlineSubsystem.
	 * @param Filters              Array of key/value pairs used to filter the search results.
	 * @param Visibility           Session visibility filter (Public, Private, or All).
	 *
	 * @return                     A FindSessionAsyncAction instance that handles the asynchronous operation.
	 */
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject", AutoCreateRefTerm="Filters", DisplayName="Find Advanced Session"), Category = "Online|Network")
	static UFindSessionAsyncAction* Create(UObject* WorldContextObject, const TArray<FSessionPropertyKeyPair>& Filters, const ESessionVisibility Visibility = ESessionVisibility::E_All);
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	virtual void Activate() override;
	UFUNCTION()
	void OnFindSessionCompleted(const bool bSuccess, const TArray<FBlueprintSessionResult>& Results);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	UPROPERTY(BlueprintAssignable)
	FBlueprintFindSessionsResultDelegate OnSuccess;
	UPROPERTY(BlueprintAssignable)
	FBlueprintFindSessionsResultDelegate OnFailure;
	
	TWeakObjectPtr<UObject> WorldContextObject;
	TArray<FSessionPropertyKeyPair> Filters;
	ESessionVisibility SessionVisibility = ESessionVisibility::E_All;
};
