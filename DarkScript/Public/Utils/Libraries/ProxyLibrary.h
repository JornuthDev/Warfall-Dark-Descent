#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ProxyLibrary.generated.h"

enum class EBlueprintResultSwitch : uint8;
struct FSessionPropertyKeyPair;
/**
 * 
 */
UCLASS()
class DARKSCRIPT_API UProxyLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintPure, Category="Online|Session", meta=(DisplayName="Make Literal Session Property Int"))
	static FSessionPropertyKeyPair MakeLiteralSessionPropertyInt(const FName Key, const int32 Integer = 0);
	
	UFUNCTION(BlueprintPure, Category="Online|Session", meta=(DisplayName="Make Literal Session Property Float"))
	static FSessionPropertyKeyPair MakeLiteralSessionPropertyFloat(const FName Key, const float Float = 0.f);
	
	UFUNCTION(BlueprintPure, Category="Online|Session", meta=(DisplayName="Make Literal Session Property Bool"))
	static FSessionPropertyKeyPair MakeLiteralSessionPropertyBool(const FName Key, const bool Bool = false);
	
	UFUNCTION(BlueprintPure, Category="Online|Session", meta=(DisplayName="Make Literal Session Property String"))
	static FSessionPropertyKeyPair MakeLiteralSessionPropertyString(const FName Key, const FString& String = FString(""));
	
	UFUNCTION(BlueprintCallable, Category="Online|Session", meta=(ExpandEnumAsExecs = "Result", DisplayName="Get Session Property Int"))
	static void GetSessionPropertyInt(const FSessionPropertyKeyPair& Property, FName& Key, int32& Value, EBlueprintResultSwitch &Result);
	
	UFUNCTION(BlueprintCallable, Category="Online|Session", meta=(ExpandEnumAsExecs = "Result", DisplayName="Get Session Property Float"))
	static void GetSessionPropertyFloat(const FSessionPropertyKeyPair& Property, FName& Key, float& Value, EBlueprintResultSwitch &Result);
	
	UFUNCTION(BlueprintCallable, Category="Online|Session", meta=(ExpandEnumAsExecs = "Result", DisplayName="Get Session Property Bool"))
	static void GetSessionPropertyBool(const FSessionPropertyKeyPair& Property, FName& Key, bool& Value, EBlueprintResultSwitch &Result);
	
	UFUNCTION(BlueprintCallable, Category="Online|Session", meta=(ExpandEnumAsExecs = "Result", DisplayName="Get Session Property String"))
	static void GetSessionPropertyString(const FSessionPropertyKeyPair& Property, FName& Key, FString& Value, EBlueprintResultSwitch &Result);
	
	UFUNCTION(BlueprintCallable, Category="Online|Session", meta=(ExpandEnumAsExecs = "Result", DisplayName="Get Session Property By Key"))
	static void GetSessionPropertyByKey(const TArray<FSessionPropertyKeyPair>& ExtraSettings, const FName Key, FSessionPropertyKeyPair& Property, EBlueprintResultSwitch &Result);
};
