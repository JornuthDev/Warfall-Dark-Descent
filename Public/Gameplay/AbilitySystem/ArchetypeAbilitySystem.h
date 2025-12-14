// Copyright Dark Script - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Utils/Types/PlayerTypes.h"
#include "ArchetypeAbilitySystem.generated.h"

// ═══════════════════════════════════════════════════════════════════════════
// CLASSE : ARCHETYPE ABILITY SYSTEM
// ═══════════════════════════════════════════════════════════════════════════

/**
 * AbilitySystemComponent pour les données SPÉCIFIQUES à un archétype
 * 
 * Attaché au Character
 * Contient l'ArchetypeAttributeSet (Health, Mana, Stats)
 * 
 * Ces données sont UNIQUES pour chaque archétype :
 * - Health / MaxHealth
 * - Mana / MaxMana
 * - Stats secondaires (Strength, Intelligence, etc. - Phase 8+)
 * 
 * Utilisé pour :
 * - Gérer le combat (Health, Mana)
 * - Appliquer les abilities de l'archétype
 * - Gérer les équipements et leurs GameplayEffects (Phase 8+)
 * - Appliquer les talents et passives (Phase 8+)
 * 
 * Réplication :
 * - Répliqué depuis le serveur vers tous les clients
 * - Les attributs sont répliqués via l'ArchetypeAttributeSet
 */
UCLASS(ClassGroup = "AbilitySystem", meta = (BlueprintSpawnableComponent))
class DARKSCRIPT_API UArchetypeAbilitySystem : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// ═══════════════════════════════════════════════════════════════════════
	// CONSTRUCTEUR
	// ═══════════════════════════════════════════════════════════════════════
	
	UArchetypeAbilitySystem();

	// ═══════════════════════════════════════════════════════════════════════
	// MÉTHODES CUSTOM
	// ═══════════════════════════════════════════════════════════════════════

	/**
	 * Initialise les attributs en appliquant les GameplayEffects permanents
	 *
	 * DOIT être appelé APRÈS ArchetypeAttributeSet::InitAttributes()
	 *
	 * Applique dans l'ordre :
	 * 1. GE_PerkToMaxAttributes (MaxHealth, MaxMana, MaxStamina, MaxFocus)
	 * 2. GE_PerkToRegenRates (HealthRegenRate, ManaRegenRate, StaminaRegenRate, FocusRegenRate)
	 * 3. GE_PerkToDamageMultipliers (PhysicDamageMultiplier, MagicDamageMultiplier, AccuracyDamageMultiplier)
	 * 4. GE_PerkToCriticals (CriticalChance, CriticalDamage)
	 *
	 * Puis initialise Health = MaxHealth, Mana = MaxMana, etc. (100% au départ)
	 */
	UFUNCTION(BlueprintCallable, Category = "Archetype|Initialization")
	void InitializeAttributes(const EClassArchetype Archetype);

	// TODO PHASE 8+ : Ajouter des méthodes custom pour la gestion de l'archétype

	/**
	 * Exemple de méthode custom future :
	 * Récupère les stats finales avec tous les modificateurs appliqués
	 */
	// UFUNCTION(BlueprintPure, Category = "Archetype|Stats")
	// float GetFinalStat(FGameplayAttribute Attribute) const;

	/**
	 * Exemple : Appliquer les GameplayEffects d'un équipement
	 */
	// UFUNCTION(BlueprintCallable, Category = "Archetype|Equipment")
	// void ApplyEquipmentEffects(const FEquipmentEntry& Equipment);

	/**
	 * Exemple : Retirer les GameplayEffects d'un équipement
	 */
	// UFUNCTION(BlueprintCallable, Category = "Archetype|Equipment")
	// void RemoveEquipmentEffects(const FEquipmentEntry& Equipment);

	/**
	 * Exemple : Activer une passive d'archétype
	 */
	// UFUNCTION(BlueprintCallable, Category = "Archetype|Passives")
	// void ActivateArchetypePassive(EClassArchetype Archetype);

	/**
	 * Exemple : Vérifier si le personnage est mort
	 */
	// UFUNCTION(BlueprintPure, Category = "Archetype|Health")
	// bool IsDead() const;

protected:
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES INTERNES
	// ═══════════════════════════════════════════════════════════════════════

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── GameplayEffects permanents pour les attributs calculés ───────────────────
	// ───────────────────────────────────────────────────────────────────────────────

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

	// TODO PHASE 8+ : Stocker les handles des GameplayEffects actifs
	// /** Handles des effets d'équipements actifs */
	// TMap<FGuid, FActiveGameplayEffectHandle> ActiveEquipmentEffects;

	// /** Handles des passives d'archétype actives */
	// TArray<FActiveGameplayEffectHandle> ActivePassives;

	// ═══════════════════════════════════════════════════════════════════════
	// OVERRIDES
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Appelé quand le component est initialisé */
	virtual void BeginPlay() override;
};