#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SaveLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DARK_SCRIPT_API USaveLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	
	/**
	 * Saves the game by delegating to the SaveInstance subsystem.
	 * @param WorldContextObject  Context object used to resolve the GameInstance.
	 * @param bAsync              If true, performs asynchronous save.
	 * @param Instructions		  Instructions for help interface to dispatch logic.
	 * @return True if the save process started successfully.
	 */
	UFUNCTION(BlueprintCallable, Category="SaveGame", meta=(WorldContext="WorldContextObject", DisplayName="Save Game"))
	static bool SaveGame(UObject* WorldContextObject, bool bAsync = true, const FString& Instructions = "");
	
	/**
	 * Loads the game by delegating to the SaveInstance subsystem.
	 * @param WorldContextObject  Context object used to resolve the GameInstance.
	 * @param Instructions		  Instructions for help interface to dispatch logic. 
	 * @return True if the load process started successfully.
	 */
	UFUNCTION(BlueprintCallable, Category="SaveGame", meta=(WorldContext="WorldContextObject", DisplayName="Load Game"))
	static bool LoadGame(UObject* WorldContextObject, const FString& Instructions = "");
};
