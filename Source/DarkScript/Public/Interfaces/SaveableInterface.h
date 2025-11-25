// ReSharper disable CppUEBlueprintCallableFunctionUnused
#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "UObject/Interface.h"
#include "Core/Systems/SaveSystem.h"
#include "SaveableInterface.generated.h"

class URunSaveGame;
class UPlayerSaveGame;

UINTERFACE()
class USaveableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DARKSCRIPT_API ISaveableInterface
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SaveGame")
	bool SaveToPlayer(UPlayerSaveGame* SaveGame, const FString& Context);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SaveGame")
	bool SaveToRun(URunSaveGame* SaveGame, const FString& Context);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SaveGame")
	bool LoadFromPlayer(UPlayerSaveGame* SaveGame, const FString& Context);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SaveGame")
	bool LoadFromRun(URunSaveGame* SaveGame, const FString& Context);
};

UCLASS()
class DARKSCRIPT_API USaveCallbackProxy : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category="SaveGame", meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject", DisplayName="Save Game By Type"))
	static USaveCallbackProxy* CreateSaveGameByType(UObject* WorldContextObject,const ESaveType Type, const FString& Context)
	{
		USaveCallbackProxy* Node = NewObject<USaveCallbackProxy>();
		Node->WorldContextObject = WorldContextObject;
		Node->Type = Type;
		Node->Context = Context;
		return Node;
	}
	
	UFUNCTION(BlueprintCallable, Category="SaveGame", meta=(BlueprintInternalUseOnly = "true", DisplayName="Save Game By Type From"))
	static USaveCallbackProxy* CreateSaveGameByTypeFrom(UObject* From,const ESaveType Type, const FString& Context)
	{
		USaveCallbackProxy* Node = NewObject<USaveCallbackProxy>();
		Node->WorldContextObject = From;
		Node->Type = Type;
		Node->Context = Context;
		return Node;
	}
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	virtual void Activate() override
	{
		const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull);
		if (!World)
		{
			return;
		}
		if (!World->GetGameInstance())
		{
			return;
		}
		USaveSystem* SaveSystem = World->GetGameInstance()->GetSubsystem<USaveSystem>();
		if (!SaveSystem)
		{
			return;
		}
		SaveSystem->OnSaveComplete.RemoveAll(this);
		SaveSystem->OnSaveComplete.AddUObject(this, &USaveCallbackProxy::SaveCompleted);
		SaveSystem->SaveGame(Type, WorldContextObject.Get(), Context);
	}
	void SaveCompleted(const bool bSuccess)
	{
		if (bSuccess)
		{
			OnSuccess.Broadcast();
		}
		else
		{
			OnFailure.Broadcast();
		}
		OnSuccess.Clear();
		OnFailure.Clear();
		SetReadyToDestroy();
	}
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnSuccess;
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnFailure;
	
	TWeakObjectPtr<UObject> WorldContextObject;
	ESaveType Type;
	FString Context;
};

UCLASS()
class DARKSCRIPT_API ULoadCallbackProxy : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category="SaveGame", meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject", DisplayName="Load Game By Type"))
	static ULoadCallbackProxy* CreateLoadGameByType(UObject* WorldContextObject,const ESaveType Type, const FString& Context)
	{
		ULoadCallbackProxy* Node = NewObject<ULoadCallbackProxy>();
		Node->WorldContextObject = WorldContextObject;
		Node->Type = Type;
		Node->Context = Context;
		return Node;
	}
	
	UFUNCTION(BlueprintCallable, Category="SaveGame", meta=(BlueprintInternalUseOnly = "true", DisplayName="Load Game By Type To"))
	static ULoadCallbackProxy* CreateLoadGameByTypeTo(UObject* From,const ESaveType Type, const FString& Context)
	{
		ULoadCallbackProxy* Node = NewObject<ULoadCallbackProxy>();
		Node->WorldContextObject = From;
		Node->Type = Type;
		Node->Context = Context;
		return Node;
	}
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	virtual void Activate() override
	{
		const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull);
		if (!World)
		{
			return;
		}
		if (!World->GetGameInstance())
		{
			return;
		}
		USaveSystem* SaveSystem = World->GetGameInstance()->GetSubsystem<USaveSystem>();
		if (!SaveSystem)
		{
			return;
		}
		SaveSystem->OnLoadComplete.RemoveAll(this);
		SaveSystem->OnLoadComplete.AddUObject(this, &ULoadCallbackProxy::LoadCompleted);
		SaveSystem->LoadGame(Type, WorldContextObject.Get(), Context);
	}
	void LoadCompleted(const bool bSuccess)
	{
		if (bSuccess)
		{
			OnSuccess.Broadcast();
		}
		else
		{
			OnFailure.Broadcast();
		}
		OnSuccess.Clear();
		OnFailure.Clear();
		SetReadyToDestroy();
	}
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnSuccess;
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnFailure;
	
	TWeakObjectPtr<UObject> WorldContextObject;
	ESaveType Type;
	FString Context;
};