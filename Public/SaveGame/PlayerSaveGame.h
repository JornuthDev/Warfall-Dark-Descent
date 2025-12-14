// Copyright Dark Script - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Utils/Types/AbilityTypes.h"
#include "Utils/Types/PlayerTypes.h"
#include "PlayerSaveGame.generated.h"

// ═══════════════════════════════════════════════════════════════════════════
// CLASSE : SAUVEGARDE JOUEUR
// ═══════════════════════════════════════════════════════════════════════════

/**
 * Sauvegarde principale du joueur
 * Contient les données GLOBALES (partagées) et les données des 4 ARCHÉTYPES
 */
UCLASS()
class DARKSCRIPT_API UPlayerSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPlayerSaveGame();
	
	// ═══════════════════════════════════════════════════════════════════════
	// DONNÉES GLOBALES (partagées entre tous les archétypes)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Niveau global du joueur (partagé entre archétypes) */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Global")
	int32 Level = 1;
	
	/** Expérience globale du joueur */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Global")
	float CurrentExp = 0;

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Global")
	EClassArchetype Archetype = EClassArchetype::Tank;
	
	// TODO PHASE 8+ : Inventaire partagé, monnaie, etc.
	// UPROPERTY(SaveGame)
	// TArray<FInventoryItem> SharedInventory;
	
	// UPROPERTY(SaveGame)
	// int32 Gold = 0;

	// ═══════════════════════════════════════════════════════════════════════
	// DONNÉES PAR ARCHÉTYPE (4 instances distinctes)
	// ═══════════════════════════════════════════════════════════════════════
	
	/**
	 * Map contenant les données des 4 archétypes
	 * Clé : EClassArchetype (Tank, Healer, Ranger, Wizard)
	 * Valeur : FArchetypeData (Health, équipements, talents, etc.)
	 */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Archetypes")
	TMap<EClassArchetype, FInternalAttributeData> ArchetypesData;

	// ═══════════════════════════════════════════════════════════════════════
	// MÉTHODES PUBLIQUES
	// ═══════════════════════════════════════════════════════════════════════

	/**
	 * Récupère les données d'un archétype (créé l'entrée si inexistante)
	 * @param InArchetype L'archétype dont on veut les données
	 * @return Référence vers les données de l'archétype
	 */
	UFUNCTION(BlueprintCallable, Category = "Save")
	FInternalAttributeData& GetOrCreateArchetypeData(EClassArchetype InArchetype);

	/**
	 * Initialise les 4 archétypes avec des valeurs par défaut
	 * Appelé lors de la première création de la sauvegarde
	 */
	UFUNCTION(BlueprintCallable, Category = "Save")
	void InitializeAllArchetypes();

	/**
	 * Vérifie si un archétype existe déjà dans la sauvegarde
	 * @param InArchetype L'archétype à vérifier
	 * @return true si l'archétype existe
	 */
	UFUNCTION(BlueprintPure, Category = "Save")
	bool HasArchetypeData(EClassArchetype InArchetype) const;

	/**
	 * Récupère les données d'un archétype (const, ne crée pas l'entrée)
	 * @param InArchetype L'archétype dont on veut les données
	 * @return Pointeur const vers les données (nullptr si inexistant)
	 */
	const FInternalAttributeData* GetArchetypeDataConst(EClassArchetype InArchetype) const;

	static const FInternalAttributeData* GetDefaultGameplay(EClassArchetype InArchetype);
};