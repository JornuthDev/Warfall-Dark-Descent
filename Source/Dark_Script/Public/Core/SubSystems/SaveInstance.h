#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveInstance.generated.h"

class UDarkSaveGame;

/**
 * USaveInstance
 *
 * GameInstance subsystem responsible for handling save and load operations.
 * Provides both synchronous and asynchronous saving functionality and broadcasts
 * a completion event when saving is finished.
 *
 * Works together with UDarkSaveGame and ISaveableInterface to serialize game data.
 */
UCLASS()
class DARK_SCRIPT_API USaveInstance : public UGameInstanceSubsystem
{
	GENERATED_BODY()
private:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Delegates ─────────────────────────────────────────────────────────────────
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveCompleted, bool, bSuccess);
	
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	void SaveGame(bool bAsync, UObject* ObjectToSave);
	void LoadGame(UObject* ObjectToLoad);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	void OnSaveFinished(const FString& SlotName, const int32 UserIndex, bool bSuccess);
	bool CreateSaveGame();
	void NativeSaveGame(bool bAsync, UObject* ObjectToSave, FString Instructions = "");
	void NativeLoadGame(UObject* ObjectToLoad, FString Instructions = "");
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	/** Reference to the active UDarkSaveGame used for persistence. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SaveGame", meta=(DisplayName="SaveGame"))
	UDarkSaveGame* SaveGameRef = nullptr;

	/** The save slot name used when saving or loading game data. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SaveGame", meta=(DisplayName="Save Slot"))
	FString SaveSlot = "DarkDescent_Save";

	/** Broadcast when a save operation completes (async or sync). */
	UPROPERTY(BlueprintAssignable, Category="SaveGame")
	FOnSaveCompleted OnSaveCompleted;
	
private:
	/** Whether the current game session is considered online. */
	bool bIsOnline = false;
};
