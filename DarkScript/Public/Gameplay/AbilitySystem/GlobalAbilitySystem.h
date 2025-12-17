#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Utils/Types/PlayerTypes.h"
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
	
	UFUNCTION(BlueprintCallable, Category = "Gameplay|Ability")
	void SwitchArchetype(const EClassArchetype NewArchetype);
	UFUNCTION(BlueprintPure, Category = "Gameplay|Ability", meta = (DisplayName = "Get Archetype"))
	EClassArchetype GetClassArchetype() const;
	UFUNCTION(BlueprintCallable, Category = "Gameplay|Ability")
	void SetClassArchetype(const EClassArchetype NewArchetype);
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Replication)
	// ═══════════════════════════════════════════════════════════════════════
	
	UFUNCTION(Server, Reliable)
	void Server_SwitchArchetype(const EClassArchetype NewArchetype);
	void Server_SwitchArchetype_Implementation(const EClassArchetype NewArchetype);
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Statics)
	// ═══════════════════════════════════════════════════════════════════════
	
	static UGlobalAbilitySystem* GetSystem(UObject* WorldContextObject);
	
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════
};

