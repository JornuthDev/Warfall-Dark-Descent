#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Utils/Types/OnlineTypes.h"
#include "FindSessionOnlineSystem.generated.h"

/**
 * 
 */
UCLASS()
class DARKSCRIPT_API UFindSessionOnlineSystem : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Delegates ─────────────────────────────────────────────────────────────────
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSessionSearchComplete, const TArray<FSessionSearchResult>&, Results, ESessionJoinError, ErrorReason);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", AutoCreateRefTerm="SearchForArchetypes", DisplayName="Find Session (Online System)"), Category = "Online|Callback")
	static UFindSessionOnlineSystem* Create(const UObject* WorldContextObject, const TArray<EClassArchetype>& SearchForArchetypes, const ESessionType SessionType = ESessionType::Public, 
		const int32 MinimumPlayers = 1, const int32 MinLevelRequired = 1);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	virtual void Activate() override;
	
	UFUNCTION()
	void OnFindSessionComplete(const TArray<FSessionSearchResult>& Results, bool bWasSuccessful, ESessionJoinError ErrorReason);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	UPROPERTY(BlueprintAssignable)
	FOnSessionSearchComplete OnSuccess;
	UPROPERTY(BlueprintAssignable)
	FOnSessionSearchComplete OnFailure;
	
	TWeakObjectPtr<UWorld> WorldPtr;
	ESessionType SessionType;
	TArray<EClassArchetype> SearchForArchetypes;
	int32 MinimumPlayers;
	int32 MinLevelRequired;
};
