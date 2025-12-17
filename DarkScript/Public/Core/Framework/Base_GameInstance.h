#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Base_GameInstance.generated.h"

enum class EKickReason : uint8;
/**
 * 
 */
UCLASS()
class DARKSCRIPT_API UBase_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	// ═══════════════════════════════════════════════════════════════════════
	// CONSTRUCTEUR
	// ═══════════════════════════════════════════════════════════════════════
    
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Blueprint API)
	// ═══════════════════════════════════════════════════════════════════════
	UFUNCTION(BlueprintNativeEvent)
	void OnKicked(EKickReason KickReason);
	
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════
};
