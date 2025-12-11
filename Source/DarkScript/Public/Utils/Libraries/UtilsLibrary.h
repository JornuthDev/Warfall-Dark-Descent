#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Utils/Types/UtilsTypes.h"
#include "UtilsLibrary.generated.h"

struct FGameplayEventData;
struct FGameplayTag;
/**
 * 
 */
UCLASS()
class DARKSCRIPT_API UUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	UFUNCTION(BlueprintPure, Category = "Utils|Variables")
	static FPropertyKeyPair MakeLiteralSessionPropertyByte(FName Key, uint8 Value);
	UFUNCTION(BlueprintPure, Category = "Utils|Variables")
	static FPropertyKeyPair MakeLiteralSessionPropertyBool(FName Key, bool Value);
	UFUNCTION(BlueprintPure, Category = "Utils|Variables")
	static FPropertyKeyPair MakeLiteralSessionPropertyString(FName Key, const FString& Value);
	UFUNCTION(BlueprintPure, Category = "Utils|Variables")
	static FPropertyKeyPair MakeLiteralSessionPropertyInt(FName Key, int32 Value);
	UFUNCTION(BlueprintPure, Category = "Utils|Variables")
	static FPropertyKeyPair MakeLiteralSessionPropertyFloat(FName Key, float Value);
	
	UFUNCTION(BlueprintCallable, Category = "Utils|Variables", meta=(ExpandEnumAsExecs = "Result"))
	static void GetSessionPropertyByte(const TArray<FPropertyKeyPair>& Properties, const FName Key, EBlueprintResultSwitch &Result, uint8& Value);
	UFUNCTION(BlueprintCallable, Category = "Utils|Variables", meta=(ExpandEnumAsExecs = "Result"))
	static void GetSessionPropertyBool(const TArray<FPropertyKeyPair>& Properties, const FName Key, EBlueprintResultSwitch &Result, bool& Value);
	UFUNCTION(BlueprintCallable, Category = "Utils|Variables", meta=(ExpandEnumAsExecs = "Result"))
	static void GetSessionPropertyString(const TArray<FPropertyKeyPair>& Properties, const FName Key, EBlueprintResultSwitch &Result, FString& Value);
	UFUNCTION(BlueprintCallable, Category = "Utils|Variables", meta=(ExpandEnumAsExecs = "Result"))
	static void GetSessionPropertyInt(const TArray<FPropertyKeyPair>& Properties, const FName Key, EBlueprintResultSwitch &Result, int32& Value);
	UFUNCTION(BlueprintCallable, Category = "Utils|Variables", meta=(ExpandEnumAsExecs = "Result"))
	static void GetSessionPropertyFloat(const TArray<FPropertyKeyPair>& Properties, const FName Key, EBlueprintResultSwitch &Result, float& Value);
};
