// Copyright Dark Script - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SystemsInterface.generated.h"

// Forward declarations
class UGlobalAbilitySystem;
class UArchetypeAbilitySystem;

// ═══════════════════════════════════════════════════════════════════════════
// INTERFACE UOBJECT (obligatoire pour le système de réflexion UE)
// ═══════════════════════════════════════════════════════════════════════════

/**
 * UInterface vide nécessaire pour le système de réflexion d'Unreal Engine
 * Ne pas ajouter de code ici
 */
UINTERFACE(MinimalAPI, BlueprintType)
class USystemsInterface : public UInterface
{
	GENERATED_BODY()
};

// ═══════════════════════════════════════════════════════════════════════════
// INTERFACE NATIVE
// ═══════════════════════════════════════════════════════════════════════════

/**
 * Interface générique pour accéder aux différents systèmes du jeu
 * 
 * Cette interface permet d'accéder facilement aux systèmes principaux :
 * - GlobalAbilitySystem : Attaché au PlayerState, contient Level et CurrentExp (partagé entre archétypes)
 * - ArchetypeAbilitySystem : Attaché au Character, contient Health, Mana, etc. (spécifique à l'archétype incarné)
 * 
 * Extensible pour d'autres systèmes futurs :
 * - InventorySystem (Phase 8+)
 * - QuestSystem (Phase future)
 * - SocialSystem (Phase future)
 * - etc.
 * 
 * Implémentée par :
 * - APlayerControllerBase (forward vers PlayerState et Character)
 * - APlayerStateBase (possède le GlobalAbilitySystem)
 * - ACharacterBase (possède l'ArchetypeAbilitySystem)
 * 
 * Utilisation typique :
 * 
 * // Récupérer le Level (attribut global)
 * if (UGlobalAbilitySystem* GlobalASC = ISystemsInterface::Execute_GetGlobalAbilitySystem(ActorWithInterface))
 * {
 *     float Level = GlobalASC->GetNumericAttribute(UGlobalAttributeSet::GetLevelAttribute());
 *     // Accès direct aux méthodes custom de GlobalAbilitySystem
 *     // GlobalASC->AddExperience(100);
 * }
 * 
 * // Récupérer la Health (attribut de l'archétype)
 * if (UArchetypeAbilitySystem* ArchetypeASC = ISystemsInterface::Execute_GetArchetypeAbilitySystem(ActorWithInterface))
 * {
 *     float Health = ArchetypeASC->GetNumericAttribute(UArchetypeAttributeSet::GetHealthAttribute());
 *     // Accès direct aux méthodes custom d'ArchetypeAbilitySystem
 *     // ArchetypeASC->ApplyEquipmentEffects(Equipment);
 * }
 */
class DARKSCRIPT_API ISystemsInterface
{
	GENERATED_BODY()

public:
	// ═══════════════════════════════════════════════════════════════════════
	// MÉTHODE : GetGlobalAbilitySystem (du PlayerState)
	// ═══════════════════════════════════════════════════════════════════════
	
	/**
	 * Récupère le GlobalAbilitySystem (attaché au PlayerState)
	 * 
	 * Contient les attributs partagés entre tous les archétypes :
	 * - Level (Niveau global du joueur)
	 * - CurrentExp (Expérience globale)
	 * 
	 * Retourne un type spécifique pour permettre l'accès aux méthodes custom
	 * (ex: AddExperience(), GetLevelWithBonuses(), etc.)
	 * 
	 * @return Pointeur vers le GlobalAbilitySystem, ou nullptr si non disponible
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Systems|Ability")
	UGlobalAbilitySystem* GetGlobalAbilitySystem();

	/**
	 * Implémentation par défaut (retourne nullptr)
	 * Les classes qui implémentent l'interface doivent override _Implementation
	 */
	virtual UGlobalAbilitySystem* GetGlobalAbilitySystem_Implementation();

	// ═══════════════════════════════════════════════════════════════════════
	// MÉTHODE : GetArchetypeAbilitySystem (du Character)
	// ═══════════════════════════════════════════════════════════════════════
	
	/**
	 * Récupère l'ArchetypeAbilitySystem (attaché au Character)
	 * 
	 * Contient les attributs spécifiques à l'archétype incarné :
	 * - Health, MaxHealth
	 * - Mana, MaxMana
	 * - Stats secondaires (Strength, Intelligence, etc. - Phase 8+)
	 * 
	 * Retourne un type spécifique pour permettre l'accès aux méthodes custom
	 * (ex: ApplyEquipmentEffects(), IsDead(), etc.)
	 * 
	 * @return Pointeur vers l'ArchetypeAbilitySystem, ou nullptr si non disponible
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Systems|Ability")
	UArchetypeAbilitySystem* GetArchetypeAbilitySystem();

	/**
	 * Implémentation par défaut (retourne nullptr)
	 * Les classes qui implémentent l'interface doivent override _Implementation
	 */
	virtual UArchetypeAbilitySystem* GetArchetypeAbilitySystem_Implementation();

	// ═══════════════════════════════════════════════════════════════════════
	// MÉTHODES HELPER STATIQUES (C++ uniquement)
	// ═══════════════════════════════════════════════════════════════════════
	
	/**
	 * Helper pour récupérer le GlobalAbilitySystem depuis n'importe quel UObject
	 * @param Object L'objet qui implémente ISystemsInterface
	 * @return GlobalAbilitySystem ou nullptr
	 */
	static UGlobalAbilitySystem* GetGlobalAbilitySystemFromObject(UObject* Object);

	/**
	 * Helper pour récupérer l'ArchetypeAbilitySystem depuis n'importe quel UObject
	 * @param Object L'objet qui implémente ISystemsInterface
	 * @return ArchetypeAbilitySystem ou nullptr
	 */
	static UArchetypeAbilitySystem* GetArchetypeAbilitySystemFromObject(UObject* Object);

	// ═══════════════════════════════════════════════════════════════════════
	// MÉTHODES FUTURES (Phase 8+)
	// ═══════════════════════════════════════════════════════════════════════
	
	// TODO PHASE 8+ : Ajouter d'autres systèmes ici
	
	/**
	 * Exemple de système futur : InventorySystem
	 */
	// UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Systems|Inventory")
	// UInventorySystem* GetInventorySystem();
	
	/**
	 * Exemple de système futur : QuestSystem
	 */
	// UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Systems|Quest")
	// UQuestSystem* GetQuestSystem();
};