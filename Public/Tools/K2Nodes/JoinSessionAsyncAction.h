#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "JoinSessionAsyncAction.generated.h"

UENUM(BlueprintType)
enum class EOnJoinSessionResult : uint8
{
	E_Success UMETA(DisplayName = "Success"),
	E_SessionFull UMETA(DisplayName = "Session Full"),
	E_WrongCode UMETA(DisplayName = "Wrong Code"),
	E_AlreadyInSession UMETA(DisplayName = "Already In Session")
};

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
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject", Category = "Online|Network", DisplayName="Join Session By Code"))
	static UJoinSessionAsyncAction* Create(UObject* WorldContextObject, const FString Code);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	virtual void Activate() override;
	UFUNCTION()
	void OnJoinSessionCompleted(FName SessionName, uint8 Result);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	UPROPERTY(BlueprintAssignable)
	FBlueprintJoinSessionResultDelegate OnSuccess;
	UPROPERTY(BlueprintAssignable)
	FBlueprintJoinSessionResultDelegate OnFailure;
	
	TWeakObjectPtr<UObject> WorldContextObject;
	FString SessionCode = "";
};
