// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SaveableInterface.generated.h"

class URunProgress;
class UPlayerProfile;
// This class does not need to be modified.
UINTERFACE()
class USaveableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DARKSCRIPT_API ISaveableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save")
	bool SaveToPlayerProfile(UPlayerProfile* SaveGame, const FString& Context);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save")
	bool LoadToPlayerProfile(const UPlayerProfile* SaveGame, const FString& Context);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save")
	bool SaveToRunProgress(URunProgress* SaveGame, const FString& Context);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save")
	bool LoadToRunProgress(const URunProgress* SaveGame, const FString& Context);
};
