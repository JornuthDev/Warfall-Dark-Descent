#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Tools/Types/ProfileTypes.h"
#include "PlayerProfile.generated.h"

enum class EPlayerArchetype : uint8;
/**
 * 
 */
UCLASS()
class DARKSCRIPT_API UPlayerProfile : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(SaveGame)
	EPlayerArchetype PlayedArchetype = EPlayerArchetype::Tank;
	UPROPERTY(SaveGame)
	int32 PlayerLevel = 1;
};
