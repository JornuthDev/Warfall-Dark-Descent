#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GlobalAbilitySystem.generated.h"


UCLASS(ClassGroup=(DarkScript), meta=(BlueprintSpawnableComponent))
class DARKSCRIPT_API UGlobalAbilitySystem : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// ═══════════════════════════════════════════════════════════════════════
	// CONSTRUCTEUR
	// ═══════════════════════════════════════════════════════════════════════
    
	UGlobalAbilitySystem();
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Blueprint API)
	// ═══════════════════════════════════════════════════════════════════════
    
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════
};
