#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Utils/Types/OnlineTypes.h"
#include "PlayerSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class DARKSCRIPT_API UPlayerSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PlayerSave", SaveGame)
	EClassArchetype Archetype = EClassArchetype::Tank;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PlayerSave", SaveGame)
	int32 PlayerLevel = 1;
};
