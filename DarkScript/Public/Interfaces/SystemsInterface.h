#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SystemsInterface.generated.h"

class UDefaultAbilitySystem;
class UGlobalAbilitySystem;
// This class does not need to be modified.
UINTERFACE()
class USystemsInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DARKSCRIPT_API ISystemsInterface
{
	GENERATED_BODY()

public:
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Blueprint API)
	// ═══════════════════════════════════════════════════════════════════════
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbilitySystem")
	UGlobalAbilitySystem* GetGlobalAbilitySystem();
	virtual UGlobalAbilitySystem* GetGlobalAbilitySystem_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbilitySystem")
	UDefaultAbilitySystem* GetDefaultAbilitySystem();
	virtual UDefaultAbilitySystem* GetDefaultAbilitySystem_Implementation();
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Statics)
	// ═══════════════════════════════════════════════════════════════════════
	
	static UGlobalAbilitySystem* GlobalAbilitySystem(UObject* Context);
	static UDefaultAbilitySystem* DefaultAbilitySystem(UObject* Context);
};
