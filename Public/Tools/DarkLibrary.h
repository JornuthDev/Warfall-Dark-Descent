#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DarkLibrary.generated.h"

struct FBlueprintSessionResult;
class UNavigationRoot;
/**
 * 
 */
UCLASS()
class DARK_SCRIPT_API UDarkLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	/**
	 * Returns the Dark-style PlayerController for the given player index.
	 * @param WorldContextObject  Context object required for Blueprint calls.
	 * @param PlayerIndex         Index of the player to retrieve (usually 0 for local).
	 */
	UFUNCTION(BlueprintPure, Category="Game", meta=(WorldContext="WorldContextObject", UnsafeDuringActorConstruction="true", DisplayName="Get Player Controller [Dark]"))
	static class ADarkPlayerController* GetPlayerController(const UObject* WorldContextObject, int32 PlayerIndex);
	
	/**
	 * Returns the GUI Controller associated with the specified player.
	 * @param WorldContextObject  Context object for lookup.
	 * @param PlayerIndex         Index of the player (usually 0).
	 */
	UFUNCTION(BlueprintPure, Category="Game", meta=(WorldContext="WorldContextObject", UnsafeDuringActorConstruction="true", DisplayName="Get Gui Controller"))
	static class UGuiController* GetGuiController(const UObject* WorldContextObject, int32 PlayerIndex);
	
	/**
	 * Retrieves the active DarkGameMode.
	 * @param WorldContextObject  Context object for lookup.
	 */
	UFUNCTION(BlueprintPure, Category="Game", meta=(WorldContext="WorldContextObject", DisplayName="Get Game Mode [Dark]"))
	static class ADarkGameMode* GetGameMode(const UObject* WorldContextObject);
	
	/**
	 * Retrieves the active DarkGameState.
	 * @param WorldContextObject  Context object for lookup.
	 */
	UFUNCTION(BlueprintPure, Category="Game", meta=(WorldContext="WorldContextObject", DisplayName="Get Game State [Dark]"))
	static class ADarkGameState* GetGameState(const UObject* WorldContextObject);
	
	/**
	 * Retrieves the active DarkGameInstance.
	 * @param WorldContextObject  Context object for lookup.
	 */
	UFUNCTION(BlueprintPure, Category="Game", meta=(WorldContext="WorldContextObject", DisplayName="Get Game Instance [Dark]"))
	static class UDarkGameInstance* GetGameInstance(const UObject* WorldContextObject);
	
	/**
	 * Saves the game by delegating to the SaveInstance subsystem.
	 * @param WorldContextObject  Context object used to resolve the GameInstance.
	 * @param bAsync              If true, performs asynchronous save.
	 * @return True if the save process started successfully.
	 */
	UFUNCTION(BlueprintCallable, Category="SaveGame", meta=(WorldContext="WorldContextObject", DisplayName="Save Game"))
	static bool SaveGame(UObject* WorldContextObject, bool bAsync = true);
	
	/**
	 * Loads the game by delegating to the SaveInstance subsystem.
	 * @param WorldContextObject  Context object used to resolve the GameInstance.
	 * @return True if the load process started successfully.
	 */
	UFUNCTION(BlueprintCallable, Category="SaveGame", meta=(WorldContext="WorldContextObject", DisplayName="Load Game"))
	static bool LoadGame(UObject* WorldContextObject);
	
	/**
	 * Sets a new active NavigationRoot within the NavigationInstance subsystem.
	 * Automatically triggers OnNavigationRootInitialized() on the new root.
	 *
	 * @param WorldContextObject  Any UObject providing a valid world context.
	 * @param InNavigationRoot    The new NavigationRoot to activate.
	 */	
	UFUNCTION(BlueprintCallable, Category="Navigation", meta=(WorldContext="WorldContextObject", DisplayName="Set Navigation Root"))
	static void SetNavigationRoot(UObject* WorldContextObject, UNavigationRoot* InNavigationRoot);
	
	UFUNCTION(BlueprintPure, Category="Online|Network", meta=(WorldContext="WorldContextObject", DisplayName="Get Session Code"))
	static FString GetSessionCode(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category="Online|Network", meta=(WorldContext="WorldContextObject", DisplayName="Is Public"))
	static bool IsPublic(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category="Online|Session", meta=(WorldContext="WorldContextObject", DisplayName="Get Session Code"))
	static FString GetSessionCode_FromSession(const FBlueprintSessionResult& Session);
	
	UFUNCTION(BlueprintCallable, Category="Online|Session", meta=(WorldContext="WorldContextObject", DisplayName="Kick Player"))
	static bool KickPlayer(const UObject* WorldContextObject, APlayerController* PlayerToKick, const FString KickReason);

protected:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	
};
