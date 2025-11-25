#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Utils/Types/OnlineTypes.h"
#include "DarkLibrary.generated.h"

class UProgression;
class ABasePlayerController;
/**
 * 
 */
UCLASS()
class DARKSCRIPT_API UDarkLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	UFUNCTION(BlueprintPure, Category="Game", meta=(WorldContext="WorldContextObject", UnsafeDuringActorConstruction="true"))
	static ABasePlayerController* GetPlayerController(const UObject* WorldContextObject, int32 PlayerIndex);
	
	UFUNCTION(BlueprintPure, Category="Gameplay", meta=(WorldContext="WorldContextObject"))
	static EClassArchetype GetArchetype(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category="Gameplay", meta=(WorldContext="WorldContextObject"))
	static int32 GetPlayerLevel(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category="Gameplay", meta=(WorldContext="WorldContextObject"))
	static void SetArchetype(const UObject* WorldContextObject, const EClassArchetype Archetype);
	
	UFUNCTION(BlueprintCallable, Category="Gameplay", meta=(WorldContext="WorldContextObject"))
	static void SetPlayerLevel(const UObject* WorldContextObject, const int32 PlayerLevel);
	
private:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	static UProgression* GetProgressionComponent(const UObject* WorldContextObject);
};
