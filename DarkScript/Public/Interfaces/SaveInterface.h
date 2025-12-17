#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SaveInterface.generated.h"

enum class ESaveType : uint8;
class UPlayerSave;
// This class does not need to be modified.
UINTERFACE()
class USaveInterface : public UInterface
{
	GENERATED_BODY()
	
};

/**
 * 
 */
class DARKSCRIPT_API ISaveInterface
{
	GENERATED_BODY()

public:
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Blueprint API)
	// ═══════════════════════════════════════════════════════════════════════
	
    UFUNCTION(BlueprintNativeEvent, Category = "SaveGame")
	bool SaveToPlayer(UPlayerSave* SaveGame, const FString& Context);
	virtual bool SaveToPlayer_Implementation(UPlayerSave* SaveGame, const FString& Context);
	UFUNCTION(BlueprintNativeEvent, Category = "SaveGame")
	bool LoadFromPlayer(UPlayerSave* SaveGame, const FString& Context);
	virtual bool LoadFromPlayer_Implementation(UPlayerSave* SaveGame, const FString& Context);
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Statics)
	// ═══════════════════════════════════════════════════════════════════════
	
	static bool Save(ESaveType Type, UObject* ObjectToSave, const FString& Context);
	static bool Load(ESaveType Type, UObject* ObjectToLoad, const FString& Context);
};
