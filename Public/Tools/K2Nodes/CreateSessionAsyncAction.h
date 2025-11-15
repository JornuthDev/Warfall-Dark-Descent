#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
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
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject", Category = "Online|Network", DisplayName="Create Advanced Session"))
	static UCreateSessionAsyncAction* Create(UObject* WorldContextObject, const bool bIsPublic = true, const int32 MaxPlayers = 4);
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
	bool bIsPublic;
	int32 MaxPlayers;
};
