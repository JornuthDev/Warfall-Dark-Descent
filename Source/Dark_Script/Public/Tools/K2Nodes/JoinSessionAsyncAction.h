#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "JoinSessionAsyncAction.generated.h"

/**
 * 
 */
UCLASS()
class DARK_SCRIPT_API UJoinSessionAsyncAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Delegates ─────────────────────────────────────────────────────────────────
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBlueprintJoinSessionResultDelegate, EOnJoinSessionResult, FailureCause);

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
/**
 * Attempts to join an online session using a session code.
 *
 * This async action searches through previously discovered session results and tries to
 * locate a matching entry using the provided Code. If a matching session is found, the
 * OnlineSubsystem attempts to join it.  
 * Completion triggers either OnSuccess (joined successfully) or OnFailure (with cause).
 *
 * @param WorldContextObject   Context object used to retrieve the world and OnlineSubsystem.
 * @param Code                 User-defined session code used to locate and join a matching session.
 *
 * @return                     A JoinSessionAsyncAction instance that handles the asynchronous operation.
 */
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject", DisplayName="Join Session By Code"), Category = "Online|Network")
	static UJoinSessionAsyncAction* Create(UObject* WorldContextObject, const FString Code);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	virtual void Activate() override;
	
	void OnJoinSessionCompleted(FName SessionName, const EOnJoinSessionCompleteResult::Type Result);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	UPROPERTY(BlueprintAssignable)
	FBlueprintJoinSessionResultDelegate OnSuccess;
	UPROPERTY(BlueprintAssignable)
	FBlueprintJoinSessionResultDelegate OnFailure;
	
	TWeakObjectPtr<UObject> WorldContextObject;
	FString SessionCode = "";
};
