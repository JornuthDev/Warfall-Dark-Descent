#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "Utils/Types/OnlineTypes.h"
#include "QuickMatchOnlineSystem.generated.h"

class UOnlineSystem;
/**
 * 
 */
UCLASS()
class DARKSCRIPT_API UQuickMatchOnlineSystem : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", AutoCreateRefTerm="SearchForArchetypes", DisplayName="Quick Match (Online System)"), Category = "Online|Callback")
	static UQuickMatchOnlineSystem* Create(const UObject* WorldContextObject, const TArray<EClassArchetype>& SearchForArchetypes);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	virtual void Activate() override;
	
	UFUNCTION()
	void OnSessionSearchComplete(const TArray<FSessionSearchResult>& Results, const bool bSuccess, const ESessionJoinError Error);
	UFUNCTION()
	void OnJoinSessionComplete(const bool bSuccess, const ESessionJoinError Error);
	UFUNCTION()
	void OnCreateSessionComplete(const bool bSuccess);
	void OnCompleteQuickMatch(const bool bSuccess);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnJoin;
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnCreate;
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnFailure;
	
	TWeakObjectPtr<UWorld> WorldPtr;
	TWeakObjectPtr<UOnlineSystem> OnlineSystemPtr;
	TArray<EClassArchetype> SearchForArchetypes;
};
