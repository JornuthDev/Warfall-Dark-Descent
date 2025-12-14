// Copyright Dark Script - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interfaces/SystemsInterface.h"
#include "Utils/Types/PlayerTypes.h"
#include "CharacterBase.generated.h"

// Forward declarations
class UArchetypeAbilitySystem;
class UArchetypeAttributeSet;

// ═══════════════════════════════════════════════════════════════════════════
// CLASSE : CHARACTER BASE
// ═══════════════════════════════════════════════════════════════════════════

/**
 * Character de base du projet Dark Script
 * 
 * Contient :
 * - ArchetypeAbilitySystem : ASC pour les données de l'archétype (Health, Mana)
 * - ArchetypeAttributeSet : Attributs spécifiques à l'archétype incarné
 * - IncarnatedArchetype : L'archétype que ce Character incarne
 * 
 * Implémente :
 * - IAbilitySystemInterface : Interface GAS standard
 * - ISystemsInterface : Interface custom pour accès aux systèmes
 */
UCLASS()
class DARKSCRIPT_API ACharacterBase : public ACharacter,
	public IAbilitySystemInterface,
	public ISystemsInterface
{
	GENERATED_BODY()

public:
	// ═══════════════════════════════════════════════════════════════════════
	// CONSTRUCTEUR
	// ═══════════════════════════════════════════════════════════════════════
	
	ACharacterBase();

	// ═══════════════════════════════════════════════════════════════════════
	// COMPONENTS
	// ═══════════════════════════════════════════════════════════════════════
	
	/** AbilitySystemComponent pour les données de l'ARCHÉTYPE (Health, Mana) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	TObjectPtr<UArchetypeAbilitySystem> ArchetypeAbilitySystem;

	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTE SETS
	// ═══════════════════════════════════════════════════════════════════════
	
	/** AttributeSet de l'archétype (Health, Mana, Stats) */
	UPROPERTY()
	TObjectPtr<UArchetypeAttributeSet> ArchetypeAttributeSet;

	// ═══════════════════════════════════════════════════════════════════════
	// ARCHETYPE INCARNÉ
	// ═══════════════════════════════════════════════════════════════════════
	
	/** L'archétype que ce Character incarne actuellement */
	UPROPERTY(BlueprintReadOnly, Category = "Archetype")
	EClassArchetype IncarnatedArchetype;

	// ═══════════════════════════════════════════════════════════════════════
	// INTERFACE : IAbilitySystemInterface (GAS standard)
	// ═══════════════════════════════════════════════════════════════════════
	
	/**
	 * Retourne l'AbilitySystemComponent principal
	 * Pour le Character, c'est l'ArchetypeAbilitySystem
	 */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// ═══════════════════════════════════════════════════════════════════════
	// INTERFACE : ISystemsInterface (custom)
	// ═══════════════════════════════════════════════════════════════════════
	
	/**
	 * Retourne le GlobalAbilitySystem (forward vers le PlayerState)
	 * Note : Le Character ne possède PAS le GlobalASC, il doit le demander au PlayerState
	 */
	virtual UGlobalAbilitySystem* GetGlobalAbilitySystem_Implementation() override;

	/**
	 * Retourne l'ArchetypeAbilitySystem (implémentation de l'interface)
	 */
	virtual UArchetypeAbilitySystem* GetArchetypeAbilitySystem_Implementation() override;
		
	// ═══════════════════════════════════════════════════════════════════════
	// MÉTHODES PUBLIQUES
	// ═══════════════════════════════════════════════════════════════════════
	
	/**
	 * Initialise l'ArchetypeAbilitySystem
	 * Appelé depuis PossessedBy (serveur) ou OnRep_PlayerState (client)
	 */
	UFUNCTION(BlueprintCallable, Category = "AbilitySystem")
	void InitializeArchetypeAbilitySystem();

	/**
	 * Appelé quand l'archétype change (switch dans le Lobby)
	 * Permet de changer le mesh, les animations, etc.
	 * 
	 * @param NewArchetype Le nouvel archétype incarné
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Archetype")
	void OnArchetypeChanged(EClassArchetype NewArchetype);
	virtual void OnArchetypeChanged_Implementation(EClassArchetype NewArchetype);

protected:
	// ═══════════════════════════════════════════════════════════════════════
	// OVERRIDES
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Appelé au début du jeu */
	virtual void BeginPlay() override;

	/**
	 * Appelé côté serveur quand le Character est possédé par un Controller
	 * C'est ici qu'on initialise l'ASC côté serveur
	 */
	virtual void PossessedBy(AController* NewController) override;

	/**
	 * Appelé côté client quand le PlayerState est répliqué
	 * C'est ici qu'on initialise l'ASC côté client
	 */
	virtual void OnRep_PlayerState() override;
};