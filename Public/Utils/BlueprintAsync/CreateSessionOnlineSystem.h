#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "Utils/Types/OnlineTypes.h"
#include "CreateSessionOnlineSystem.generated.h"

/**
 * 
 */
UCLASS()
class DARKSCRIPT_API UCreateSessionOnlineSystem : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", AutoCreateRefTerm="AllowedArchetypes", DisplayName="Create Session (Online System)"), Category = "Online|Callback")
	static UCreateSessionOnlineSystem* Create(const UObject* WorldContextObject, const TArray<EClassArchetype>& AllowedArchetypes, const ESessionType SessionType = ESessionType::Public,
		const FString& SessionName = FString(""), const int32 MinLevelRequired = 1);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	virtual void Activate() override;
	
	UFUNCTION()
	void OnCreateSessionComplete(bool bSuccess);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnSuccess;
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnFailure;
	
	TWeakObjectPtr<UWorld> WorldPtr;
	ESessionType SessionType;
	FString SessionName;
	TArray<EClassArchetype> AllowedArchetypes;
	int32 MinLevelRequired;
};
