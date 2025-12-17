#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Utils/Types/OnlineTypes.h"
#include "JoinSessionOnlineSystem.generated.h"

/**
 * 
 */
UCLASS()
class DARKSCRIPT_API UJoinSessionOnlineSystem : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Delegates ─────────────────────────────────────────────────────────────────
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionJoined, ESessionJoinError, ErrorReason);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", DisplayName="Join Session (Online System)"), Category = "Online|Callback")
	static UJoinSessionOnlineSystem* JoinSession(const UObject* WorldContextObject, const FSessionSearchResult& Session);
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", DisplayName="Join By Session Code"), Category = "Online|Callback")
	static UJoinSessionOnlineSystem* JoinSessionByCode(const UObject* WorldContextObject, const FString& SessionCode);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	virtual void Activate() override;
	
	UFUNCTION()
	void OnJoinSessionComplete(bool bSuccess, ESessionJoinError Error);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	UPROPERTY(BlueprintAssignable)
	FOnSessionJoined OnSuccess;
	UPROPERTY(BlueprintAssignable)
	FOnSessionJoined OnFailure;
	
	TWeakObjectPtr<UWorld> WorldPtr;
	FSessionSearchResult Session;
	FString SessionCode;
	bool bIsByCode = false;
};
