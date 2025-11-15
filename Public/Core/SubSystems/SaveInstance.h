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

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	/**
 * Returns whether the game is currently running in an online context.
 * Used to distinguish between local and online save systems.
 */
	UFUNCTION(BlueprintPure, Category = "SaveGame", meta=(DisplayName="Is Online"))
	bool IsOnline() const { return bIsOnline; }
	
	/**
 * Saves the provided object’s data to disk.
 * Requires the object to implement ISaveableInterface.
 *
 * @param bAsync        If true, saves asynchronously and triggers callback upon completion.
 * @param ObjectToSave  The object that will serialize its data into the save slot.
 */
	UFUNCTION(BlueprintCallable, Category = "SaveGame", meta=(DisplayName="Save Game"))
	void SaveGame(bool bAsync, UObject* ObjectToSave);
	
	/**
 * Loads saved data into the specified object.
 * Requires the object to implement ISaveableInterface.
 *
 * @param ObjectToLoad  The object that will deserialize its data from the save slot.
 */
	UFUNCTION(BlueprintCallable, Category = "SaveGame", meta=(DisplayName="Load Game"))
	void LoadGame(UObject* ObjectToLoad);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	void OnSaveFinished(const FString& SlotName, const int32 UserIndex, bool bSuccess);
	bool CreateSaveGame();
	void NativeSaveGame(bool bAsync, UObject* ObjectToSave);
	void NativeLoadGame(UObject* ObjectToLoad);
	
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
