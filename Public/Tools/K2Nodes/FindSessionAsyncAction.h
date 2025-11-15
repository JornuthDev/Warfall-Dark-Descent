#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "FindSessionsCallbackProxy.h"
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
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject", Category = "Online|Network", DisplayName="Find Advanced Session"))
	static UFindSessionAsyncAction* Create(UObject* WorldContextObject, const bool bIsPublic = true);
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
	bool bIsPublic;
};
