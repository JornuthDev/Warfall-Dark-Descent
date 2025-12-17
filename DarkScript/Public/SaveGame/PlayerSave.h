#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Utils/Types/SaveTypes.h"
#include "PlayerSave.generated.h"


/**
 * 
 */
UCLASS()
class DARKSCRIPT_API UPlayerSave : public USaveGame
{
	GENERATED_BODY()
	
public:
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Helpers)
	// ═══════════════════════════════════════════════════════════════════════
    
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════
	UPROPERTY(SaveGame)
	FGlobalAbilitySave GlobalAbilitySave;
};
