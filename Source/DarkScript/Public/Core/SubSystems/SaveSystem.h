#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveSystem.generated.h"

class USaveGame;
class UPlayerProfile;
class URunProgress;
/**
 * 
 */
UCLASS()
class DARKSCRIPT_API USaveSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnSaveComplete, bool);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoadComplete, bool);
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	FString GetSteamID() const;
	FString GetPlayerSlotName() const;
	FString GetRunProgressSlotName() const;
	
	void CreateSaveGames();
	void SavePlayerProfile(UObject* ObjectToSave, const FString& Context);
	void LoadPlayerProfile(UObject* ObjectToLoad, const FString& Context);
	void SaveRunProgress(UObject* ObjectToSave, const FString& Context);
	void LoadRunProgress(UObject* ObjectToLoad, const FString& Context);
	
	void OnSaveCompleteHandle(const FString& SlotName, const int32 UserIndex, const bool bSuccess);
	void OnLoadCompleteHandle(const FString& SlotName, const int32 UserIndex, const bool bSuccess);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	UPROPERTY(BlueprintReadOnly, Category="Save")
	FString SteamID = "";

	FOnSaveComplete OnSaveComplete;
	FOnLoadComplete OnLoadComplete;
private:
	UPROPERTY()
	UPlayerProfile* PlayerProfile;
	UPROPERTY()
	URunProgress* RunProgress;
};
