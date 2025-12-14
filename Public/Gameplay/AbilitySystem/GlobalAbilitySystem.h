// Copyright Dark Script - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GlobalAbilitySystem.generated.h"

// ═══════════════════════════════════════════════════════════════════════════
// CLASSE : GLOBAL ABILITY SYSTEM
// ═══════════════════════════════════════════════════════════════════════════

/**
 * AbilitySystemComponent pour les données GLOBALES du joueur
 * 
 * Attaché au PlayerState
 * Contient le GlobalAttributeSet (Level, CurrentExp)
 * 
 * Ces données sont PARTAGÉES entre tous les archétypes :
 * - Level : Niveau global du joueur
 * - CurrentExp : Expérience globale
 * 
 * Utilisé pour :
 * - Gérer la progression globale (Level, XP)
 * - Appliquer des GameplayEffects globaux (buffs de guilde, boosts temporaires)
 * - Gérer les abilities globales (Phase 8+ : systèmes de crafting, talents partagés)
 * 
 * Réplication :
 * - Répliqué depuis le serveur vers tous les clients
 * - Les attributs sont répliqués via le GlobalAttributeSet
 */
UCLASS(ClassGroup = "AbilitySystem", meta = (BlueprintSpawnableComponent))
class DARKSCRIPT_API UGlobalAbilitySystem : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// ═══════════════════════════════════════════════════════════════════════
	// CONSTRUCTEUR
	// ═══════════════════════════════════════════════════════════════════════
	
	UGlobalAbilitySystem();

	// ═══════════════════════════════════════════════════════════════════════
	// MÉTHODES CUSTOM (Phase future)
	// ═══════════════════════════════════════════════════════════════════════
	
	// TODO PHASE 8+ : Ajouter des méthodes custom pour la progression globale
	
	/**
	 * Exemple de méthode custom future :
	 * Récupère le niveau avec les bonus appliqués
	 */
	// UFUNCTION(BlueprintPure, Category = "Global|Level")
	// float GetLevelWithBonuses() const;
	
	/**
	 * Exemple : Ajouter de l'expérience et gérer le level up
	 */
	// UFUNCTION(BlueprintCallable, Category = "Global|Experience")
	// void AddExperience(int32 Amount);
	
	/**
	 * Exemple : Vérifier si le joueur peut monter de niveau
	 */
	// UFUNCTION(BlueprintPure, Category = "Global|Experience")
	// bool CanLevelUp() const;

protected:
	// ═══════════════════════════════════════════════════════════════════════
	// OVERRIDES
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Appelé quand le component est initialisé */
	virtual void BeginPlay() override;
};