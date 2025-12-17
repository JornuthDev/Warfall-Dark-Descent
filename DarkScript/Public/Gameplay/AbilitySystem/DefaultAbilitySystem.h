#pragma once
#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Utils/Types/PlayerTypes.h"
#include "DefaultAbilitySystem.generated.h"

UCLASS(ClassGroup=(DarkScript), meta=(BlueprintSpawnableComponent))
class DARKSCRIPT_API UDefaultAbilitySystem : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// ═══════════════════════════════════════════════════════════════════════
	// CONSTRUCTEUR
	// ═══════════════════════════════════════════════════════════════════════
	
	UDefaultAbilitySystem();
	void Init(AActor* InOwnerActor, AActor* InAvatarActor, const EClassArchetype Archetype = EClassArchetype::None, const FName RowName = NAME_None);
	void InitAdvanced();
	void InitAttributes(const EClassArchetype Archetype = EClassArchetype::None, const FName RowName = NAME_None);
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Overrides)
	// ═══════════════════════════════════════════════════════════════════════
	
    virtual FActiveGameplayEffectHandle ApplyGameplayEffectSpecToTarget(const FGameplayEffectSpec& GameplayEffect, UAbilitySystemComponent* Target, 
    	FPredictionKey PredictionKey = FPredictionKey()) override;
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Helper)
	// ═══════════════════════════════════════════════════════════════════════
	
	void ApplyPermanentGameplayEffects();
	float GetContextLevel() const { return bIsPlayerCharacter ? 2.f : 1.f; }
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Statics)
	// ═══════════════════════════════════════════════════════════════════════
	
	static UDefaultAbilitySystem* GetSystem(UObject* WorldContextObject);
	
protected:
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Replication)
	// ═══════════════════════════════════════════════════════════════════════
	
	UFUNCTION(Server, Reliable)
	void Server_Init(AActor* InOwnerActor, AActor* InAvatarActor, const EClassArchetype Archetype = EClassArchetype::None, const FName RowName = NAME_None);
	void Server_Init_Implementation(AActor* InOwnerActor, AActor* InAvatarActor, const EClassArchetype Archetype = EClassArchetype::None, const FName RowName = NAME_None);
	
	UFUNCTION(Server, Reliable)
	void Server_InitAttributes(const EClassArchetype Archetype = EClassArchetype::None, const FName RowName = NAME_None);
	void Server_InitAttributes_Implementation(const EClassArchetype Archetype = EClassArchetype::None, const FName RowName = NAME_None);
	
public:
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay Effects")
	bool bIsPlayerCharacter = false;
	
protected:	
	/** GameplayEffect permanent qui calcule MaxHealth, MaxMana, MaxStamina, MaxFocus depuis les Perks */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> GE_PerkToMaxAttributes;

	/** GameplayEffect permanent qui calcule les RegenRates depuis les Perks et MaxAttributes */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> GE_PerkToRegenRates;

	/** GameplayEffect permanent qui calcule les Damage Multipliers depuis les Perks */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> GE_PerkToDamageMultipliers;

	/** GameplayEffect permanent qui calcule CriticalChance et CriticalDamage depuis les Perks */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> GE_PerkToCriticals;
};
