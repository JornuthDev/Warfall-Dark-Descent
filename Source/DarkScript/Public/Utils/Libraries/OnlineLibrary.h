#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OnlineLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DARKSCRIPT_API UOnlineLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	UFUNCTION(BlueprintPure, Category="Online", meta=(WorldContext="WorldContextObject"))
	static FString GetPlayerSteamID(const UObject* WorldContextObject, const APlayerController* PlayerController);
	
	UFUNCTION(BlueprintPure, Category="Online", meta=(WorldContext="WorldContextObject"))
	static FString GetPieInstance(const UObject* WorldContextObject, const APlayerController* PlayerController);
	
};
