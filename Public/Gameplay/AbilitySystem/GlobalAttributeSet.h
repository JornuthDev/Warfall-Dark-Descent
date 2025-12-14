// Copyright Dark Script - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GlobalAttributeSet.generated.h"

// ═══════════════════════════════════════════════════════════════════════════
// MACROS HELPER POUR LES ATTRIBUTS
// ═══════════════════════════════════════════════════════════════════════════

// Utilise les macros fournies par le moteur pour simplifier la définition des accesseurs
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

// ═══════════════════════════════════════════════════════════════════════════
// CLASSE : GLOBAL ATTRIBUTE SET
// ═══════════════════════════════════════════════════════════════════════════

/**
 * AttributeSet pour les données GLOBALES du joueur (partagées entre archétypes)
 * 
 * Attaché au PlayerState via GlobalAbilitySystem
 * 
 * Attributs :
 * - Level : Niveau global du joueur
 * - CurrentExp : Expérience actuelle du joueur
 * 
 * Ces attributs sont RÉPLIQUÉS et partagés entre tous les archétypes
 */
UCLASS()
class DARKSCRIPT_API UGlobalAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	// ═══════════════════════════════════════════════════════════════════════
	// CONSTRUCTEUR
	// ═══════════════════════════════════════════════════════════════════════
	
	UGlobalAttributeSet();
	
	/**
	 * Helper pour récupérer le Controller propriétaire
	 * @return Le Controller ou nullptr
	 */
	AController* GetController() const;

	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUT : LEVEL (Niveau global)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Niveau global du joueur (partagé entre tous les archétypes) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Level, Category = "Attributes|Global")
	FGameplayAttributeData Level;
	ATTRIBUTE_ACCESSORS(UGlobalAttributeSet, Level)

	/** Callback de réplication pour Level */
	UFUNCTION()
	virtual void OnRep_Level(const FGameplayAttributeData& OldLevel);

	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUT : CURRENT EXP (Expérience actuelle)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Expérience actuelle du joueur */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentExp, Category = "Attributes|Global")
	FGameplayAttributeData CurrentExp;
	ATTRIBUTE_ACCESSORS(UGlobalAttributeSet, CurrentExp)

	/** Callback de réplication pour CurrentExp */
	UFUNCTION()
	virtual void OnRep_CurrentExp(const FGameplayAttributeData& OldCurrentExp);

	// ═══════════════════════════════════════════════════════════════════════
	// OVERRIDES
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Override pour enregistrer les propriétés répliquées */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	/**
	 * Appelé après qu'un GameplayEffect a modifié un attribut (SERVEUR UNIQUEMENT)
	 * Utilisé pour sauvegarder automatiquement côté serveur
	 */
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};