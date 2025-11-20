#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "SaveGameAsync.generated.h"

/**
 * 
 */
UCLASS()
class DARKSCRIPT_API USaveGameAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	
	//---SAVE---
	
	UFUNCTION(BlueprintCallable, Category="Save", meta=(BlueprintInternalUseOnly = "true", WorldContext="ObjectToSave", DisplayName="Save Player Profile"))
	static USaveGameAsync* SavePlayerProfile(UObject* ObjectToSave, const FString& Context);
	
	UFUNCTION(BlueprintCallable, Category="Save", meta=(BlueprintInternalUseOnly = "true", DisplayName="Save Player Profile From"))
	static USaveGameAsync* SavePlayerProfileFrom(UObject* From, const FString& Context);
	
	UFUNCTION(BlueprintCallable, Category="Save", meta=(BlueprintInternalUseOnly = "true", WorldContext="ObjectToSave", DisplayName="Save Run Progression"))
	static USaveGameAsync* SaveRunProgress(UObject* ObjectToSave, const FString& Context);
	
	UFUNCTION(BlueprintCallable, Category="Save", meta=(BlueprintInternalUseOnly = "true", DisplayName="Save Run Progression From"))
	static USaveGameAsync* SaveRunProgressFrom(UObject* From, const FString& Context);
	
	//---LOAD---
	
	UFUNCTION(BlueprintCallable, Category="Load", meta=(BlueprintInternalUseOnly = "true", WorldContext="ObjectToLoad", DisplayName="Load Player Profile"))
	static USaveGameAsync* LoadPlayerProfile(UObject* ObjectToLoad, const FString& Context);
	
	UFUNCTION(BlueprintCallable, Category="Load", meta=(BlueprintInternalUseOnly = "true", DisplayName="Load Player Profile From"))
	static USaveGameAsync* LoadPlayerProfileFrom(UObject* From, const FString& Context);
	
	UFUNCTION(BlueprintCallable, Category="Load", meta=(BlueprintInternalUseOnly = "true", WorldContext="ObjectToLoad", DisplayName="Load Run Progression"))
	static USaveGameAsync* LoadRunProgress(UObject* ObjectToLoad, const FString& Context);
	
	UFUNCTION(BlueprintCallable, Category="Load", meta=(BlueprintInternalUseOnly = "true", DisplayName="Load Run Progression From"))
	static USaveGameAsync* LoadRunProgressFrom(UObject* From, const FString& Context);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	virtual void Activate() override;
	void OnCompleted(const bool bSuccess);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnSuccess;
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnFailure;
	
	TWeakObjectPtr<UObject> Object;
	FString Context;
	TPair<bool, bool> bIsPlayerSaveGame;
};
