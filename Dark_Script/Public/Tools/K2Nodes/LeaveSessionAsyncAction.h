#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "LeaveSessionAsyncAction.generated.h"

/**
 * 
 */
UCLASS()
class DARK_SCRIPT_API ULeaveSessionAsyncAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
/**
 * Leaves the currently active online session.
 *
 * This async action requests the OnlineSubsystem to destroy or leave the active session.
 * Once the process completes, either OnSuccess or OnFailure is broadcast depending on 
 * the outcome.
 *
 * @param WorldContextObject   Context object used to retrieve the world and OnlineSubsystem.
 *
 * @return                     A LeaveSessionAsyncAction instance handling the asynchronous operation.
 */
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject", DisplayName="Leave Session"), Category = "Online|Network")
	static ULeaveSessionAsyncAction* Create(UObject* WorldContextObject);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	virtual void Activate() override;
	
	UFUNCTION()
	void OnLeaveSessionCompleted(const bool bSuccess);
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnSuccess;
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnFailure;
	
	TWeakObjectPtr<UObject> WorldContextObject;
};
