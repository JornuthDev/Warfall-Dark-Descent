#pragma once
#include "CoreMinimal.h"
#include "SaveableInterface.generated.h"

class UDarkSaveGame;

UINTERFACE(BlueprintType)
class USaveableInterface : public UInterface
{
	GENERATED_BODY()
};

class DARK_SCRIPT_API ISaveableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Network")
	bool SaveObject(UDarkSaveGame* SaveGame, const FString& Instructions);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Network")
	bool LoadObject(UDarkSaveGame* SaveGame, const FString& Instructions);
};
