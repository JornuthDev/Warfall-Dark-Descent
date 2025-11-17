#pragma once
#include "CoreMinimal.h"
#include "Core/DarkGameInstance.h"
#include "Core/Game/DarkGameMode.h"
#include "Core/Game/DarkGameState.h"
#include "Core/DarkPlayerController.h"
#include "User Interfaces/NavigationRoot.h"
#include "DarkLibrary.generated.h"

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
	 * Sets a new active NavigationRoot within the NavigationInstance subsystem.
	 * Automatically triggers OnNavigationRootInitialized() on the new root.
	 *
	 * @param WorldContextObject  Any UObject providing a valid world context.
	 * @param InNavigationRoot    The new NavigationRoot to activate.
	 */	
	UFUNCTION(BlueprintCallable, Category="Navigation", meta=(WorldContext="WorldContextObject", DisplayName="Set Navigation Root"))
	static void SetNavigationRoot(UObject* WorldContextObject, UNavigationRoot* InNavigationRoot);
};