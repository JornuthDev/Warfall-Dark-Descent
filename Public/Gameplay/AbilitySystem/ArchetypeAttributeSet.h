// Copyright Dark Script - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ArchetypeAttributeSet.generated.h"

// ═══════════════════════════════════════════════════════════════════════════
// MACROS HELPER POUR LES ATTRIBUTS
// ═══════════════════════════════════════════════════════════════════════════

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

// ═══════════════════════════════════════════════════════════════════════════
// CLASSE : ARCHETYPE ATTRIBUTE SET
// ═══════════════════════════════════════════════════════════════════════════

struct FInternalAttributeData;
enum class EClassArchetype : uint8;
/**
 * AttributeSet pour les données SPÉCIFIQUES à un archétype
 * 
 * Attaché au Character via ArchetypeAbilitySystemComponent
 * 
 * Attributs de base (Phase 2) :
 * - Health : Points de vie actuels
 * - MaxHealth : Points de vie maximums
 * - Mana : Points de mana actuels
 * - MaxMana : Points de mana maximums
 * 
 * Attributs futurs (Phase 8+) :
 * - Strength, Intelligence, Agility, etc.
 * - Armor, MagicResist, etc.
 * 
 * Ces attributs sont RÉPLIQUÉS et UNIQUES pour chaque archétype
 */
UCLASS()
class DARKSCRIPT_API UArchetypeAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	// ═══════════════════════════════════════════════════════════════════════
	// CONSTRUCTEUR
	// ═══════════════════════════════════════════════════════════════════════
	
	UArchetypeAttributeSet();
	void InitAttributes(const EClassArchetype Archetype);
	void OnLevelUp();
	
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════
	
	const FInternalAttributeData* ArchetypeData;
	float RegenRateWeight = 0.001f;
	
	// ═══════════════════════════════════════════════════════════════════════
	// CALCUL
	// ═══════════════════════════════════════════════════════════════════════

	/**
	 * Calcule les points de Perk en fonction du Level et du PerkWeight
	 *
	 * Formule : Points = Floor(Level / PerkWeight)
	 *
	 * @param Level Niveau du personnage
	 * @param PerkWeight Poids du Perk (depuis Data)
	 * @return Points de Perk à attribuer
	 */
	float CalculatePerkPoints(const int32 Level, const float PerkWeight) const;
	
	// ═══════════════════════════════════════════════════════════════════════
	// BASE ATTRIBUTES (Valeurs de base depuis DataTable)
	// ═══════════════════════════════════════════════════════════════════════

	/** Valeur de base pour Health (depuis DataTable) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHealth, Category = "Base Attributes")
	FGameplayAttributeData BaseHealth;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, BaseHealth)

	UFUNCTION()
	virtual void OnRep_BaseHealth(const FGameplayAttributeData& OldBaseHealth);

	/** Valeur de base pour Mana (depuis DataTable) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseMana, Category = "Base Attributes")
	FGameplayAttributeData BaseMana;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, BaseMana)

	UFUNCTION()
	virtual void OnRep_BaseMana(const FGameplayAttributeData& OldBaseMana);

	/** Valeur de base pour Stamina (depuis DataTable) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseStamina, Category = "Base Attributes")
	FGameplayAttributeData BaseStamina;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, BaseStamina)

	UFUNCTION()
	virtual void OnRep_BaseStamina(const FGameplayAttributeData& OldBaseStamina);

	/** Valeur de base pour Focus (depuis DataTable) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseFocus, Category = "Base Attributes")
	FGameplayAttributeData BaseFocus;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, BaseFocus)

	UFUNCTION()
	virtual void OnRep_BaseFocus(const FGameplayAttributeData& OldBaseFocus);

	/** Taux de régénération de base pour Health */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHealthRegenRate, Category = "Base Attributes")
	FGameplayAttributeData BaseHealthRegenRate;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, BaseHealthRegenRate)

	UFUNCTION()
	virtual void OnRep_BaseHealthRegenRate(const FGameplayAttributeData& OldBaseHealthRegenRate);

	/** Taux de régénération de base pour Mana */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseManaRegenRate, Category = "Base Attributes")
	FGameplayAttributeData BaseManaRegenRate;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, BaseManaRegenRate)

	UFUNCTION()
	virtual void OnRep_BaseManaRegenRate(const FGameplayAttributeData& OldBaseManaRegenRate);

	/** Taux de régénération de base pour Stamina */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseStaminaRegenRate, Category = "Base Attributes")
	FGameplayAttributeData BaseStaminaRegenRate;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, BaseStaminaRegenRate)

	UFUNCTION()
	virtual void OnRep_BaseStaminaRegenRate(const FGameplayAttributeData& OldBaseStaminaRegenRate);

	/** Taux de régénération de base pour Focus */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseFocusRegenRate, Category = "Base Attributes")
	FGameplayAttributeData BaseFocusRegenRate;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, BaseFocusRegenRate)

	UFUNCTION()
	virtual void OnRep_BaseFocusRegenRate(const FGameplayAttributeData& OldBaseFocusRegenRate);

	/** Multiplicateur de dégâts physiques de base */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BasePhysicDamageMultiplier, Category = "Base Attributes")
	FGameplayAttributeData BasePhysicDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, BasePhysicDamageMultiplier)

	UFUNCTION()
	virtual void OnRep_BasePhysicDamageMultiplier(const FGameplayAttributeData& OldBasePhysicDamageMultiplier);

	/** Multiplicateur de dégâts magiques de base */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseMagicDamageMultiplier, Category = "Base Attributes")
	FGameplayAttributeData BaseMagicDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, BaseMagicDamageMultiplier)

	UFUNCTION()
	virtual void OnRep_BaseMagicDamageMultiplier(const FGameplayAttributeData& OldBaseMagicDamageMultiplier);

	/** Multiplicateur de dégâts de précision de base */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseAccuracyDamageMultiplier, Category = "Base Attributes")
	FGameplayAttributeData BaseAccuracyDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, BaseAccuracyDamageMultiplier)

	UFUNCTION()
	virtual void OnRep_BaseAccuracyDamageMultiplier(const FGameplayAttributeData& OldBaseAccuracyDamageMultiplier);

	/** Chance de coup critique de base */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseCriticalChance, Category = "Base Attributes")
	FGameplayAttributeData BaseCriticalChance;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, BaseCriticalChance)

	UFUNCTION()
	virtual void OnRep_BaseCriticalChance(const FGameplayAttributeData& OldBaseCriticalChance);

	/** Dégâts critiques de base */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseCriticalDamage, Category = "Base Attributes")
	FGameplayAttributeData BaseCriticalDamage;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, BaseCriticalDamage)

	UFUNCTION()
	virtual void OnRep_BaseCriticalDamage(const FGameplayAttributeData& OldBaseCriticalDamage);

	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS : HEALTH (Points de vie)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de vie actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, Health)

	/** Callback de réplication pour Health */
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	/** Points de vie rendu par seconde */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegenRate, Category = "Attributes")
	FGameplayAttributeData HealthRegenRate;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, HealthRegenRate)

	/** Callback de réplication pour HealthRegenRate */
	UFUNCTION()
	virtual void OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate);
	
	/** Points de vie maximums */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, MaxHealth)

	/** Callback de réplication pour MaxHealth */
	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS : MANA (Points de mana)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de mana actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, Mana)

	/** Callback de réplication pour Mana */
	UFUNCTION()
	virtual void OnRep_Mana(const FGameplayAttributeData& OldMana);
	
	/** Points de mana rendu par seconde */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ManaRegenRate, Category = "Attributes")
	FGameplayAttributeData ManaRegenRate;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, ManaRegenRate)

	/** Callback de réplication pour ManaRegenRate */
	UFUNCTION()
	virtual void OnRep_ManaRegenRate(const FGameplayAttributeData& OldManaRegenRate);
	
	/** Points de mana maximums */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, MaxMana)

	/** Callback de réplication pour MaxMana */
	UFUNCTION()
	virtual void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);

	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS : STAMINA (Points de stamina)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de stamina actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stamina, Category = "Attributes")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, Stamina)

	/** Callback de réplication pour Stamina */
	UFUNCTION()
	virtual void OnRep_Stamina(const FGameplayAttributeData& OldStamina);

	/** Points de stamina rendu par seconde */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_StaminaRegenRate, Category = "Attributes")
	FGameplayAttributeData StaminaRegenRate;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, StaminaRegenRate)

	/** Callback de réplication pour StaminaRegenRate */
	UFUNCTION()
	virtual void OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate);
	
	/** Points de Stamina maximums */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxStamina, Category = "Attributes")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, MaxStamina)

	/** Callback de réplication pour MaxStamina */
	UFUNCTION()
	virtual void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS : FOCUS (Points de focus)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de Focus actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Focus, Category = "Attributes")
	FGameplayAttributeData Focus;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, Focus)

	/** Callback de réplication pour Focus */
	UFUNCTION()
	virtual void OnRep_Focus(const FGameplayAttributeData& OldFocus);

	/** Points de focus rendu par seconde */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FocusRegenRate, Category = "Attributes")
	FGameplayAttributeData FocusRegenRate;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, FocusRegenRate)

	/** Callback de réplication pour FocusRegenRate */
	UFUNCTION()
	virtual void OnRep_FocusRegenRate(const FGameplayAttributeData& OldFocusRegenRate);
	
	/** Points de focus maximums */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxFocus, Category = "Attributes")
	FGameplayAttributeData MaxFocus;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, MaxFocus)

	/** Callback de réplication pour MaxFocus */
	UFUNCTION()
	virtual void OnRep_MaxFocus(const FGameplayAttributeData& OldMaxFocus);
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|ATTACK : PHYSIC DAMAGE MULTIPLIER (Points de Physic Damage Multiplier)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de PhysicDamageMultiplier actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicDamageMultiplier, Category = "Attributes|Attack")
	FGameplayAttributeData PhysicDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, PhysicDamageMultiplier)

	/** Callback de réplication pour PhysicDamageMultiplier */
	UFUNCTION()
	virtual void OnRep_PhysicDamageMultiplier(const FGameplayAttributeData& OldPhysicDamageMultiplier);
		
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|ATTACK : MAGIC DAMAGE MULTIPLIER (Points de Magic Damage Multiplier)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de MagicDamageMultiplier actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicDamageMultiplier, Category = "Attributes|Attack")
	FGameplayAttributeData MagicDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, MagicDamageMultiplier)

	/** Callback de réplication pour MagicDamageMultiplier */
	UFUNCTION()
	virtual void OnRep_MagicDamageMultiplier(const FGameplayAttributeData& OldMagicDamageMultiplier);
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|ATTACK : ACCURACY DAMAGE MULTIPLIER (Points de Accuracy Damage Multiplier)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de AccuracyDamageMultiplier actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AccuracyDamageMultiplier, Category = "Attributes|Attack")
	FGameplayAttributeData AccuracyDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, AccuracyDamageMultiplier)

	/** Callback de réplication pour AccuracyDamageMultiplier */
	UFUNCTION()
	virtual void OnRep_AccuracyDamageMultiplier(const FGameplayAttributeData& OldAccuracyDamageMultiplier);
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|ATTACK : CRITICAL CHANCE (Points de Critical Chance)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de CriticalChance actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalChance, Category = "Attributes|Attack")
	FGameplayAttributeData CriticalChance;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, CriticalChance)

	/** Callback de réplication pour CriticalChance */
	UFUNCTION()
	virtual void OnRep_CriticalChance(const FGameplayAttributeData& OldCriticalChance);
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|ATTACK : CRITICAL DAMAGE (Points de Critical Damage)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de CriticalDamage actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalDamage, Category = "Attributes|Attack")
	FGameplayAttributeData CriticalDamage;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, CriticalDamage)

	/** Callback de réplication pour CriticalDamage */
	UFUNCTION()
	virtual void OnRep_CriticalDamage(const FGameplayAttributeData& OldCriticalDamage);
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|RESISTANCE : ARMOR (Points de Armor)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de IceResistance actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Attributes|Resistance")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, Armor)

	/** Callback de réplication pour IceResistance */
	UFUNCTION()
	virtual void OnRep_Armor(const FGameplayAttributeData& OldArmor);
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|RESISTANCE : ICE RESISTANCE (Points de Ice Resistance)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de IceResistance actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IceResistance, Category = "Attributes|Resistance")
	FGameplayAttributeData IceResistance;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, IceResistance)

	/** Callback de réplication pour IceResistance */
	UFUNCTION()
	virtual void OnRep_IceResistance(const FGameplayAttributeData& OldIceResistance);
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|RESISTANCE : FIRE RESISTANCE (Points de Fire Resistance)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de FireResistance actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FireResistance, Category = "Attributes|Resistance")
	FGameplayAttributeData FireResistance;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, FireResistance)

	/** Callback de réplication pour FireResistance */
	UFUNCTION()
	virtual void OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance);
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|RESISTANCE : LIGHTNING RESISTANCE (Points de Lightning Resistance)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de LightningResistance actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LightningResistance, Category = "Attributes|Resistance")
	FGameplayAttributeData LightningResistance;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, LightningResistance)

	/** Callback de réplication pour LightningResistance */
	UFUNCTION()
	virtual void OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance);
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|RESISTANCE : HOLY RESISTANCE (Points de Holy Resistance)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de HolyResistance actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HolyResistance, Category = "Attributes|Resistance")
	FGameplayAttributeData HolyResistance;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, HolyResistance)

	/** Callback de réplication pour HolyResistance */
	UFUNCTION()
	virtual void OnRep_HolyResistance(const FGameplayAttributeData& OldHolyResistance);
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|RESISTANCE : POISON RESISTANCE (Points de Poison Resistance)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de PoisonResistance actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PoisonResistance, Category = "Attributes|Resistance")
	FGameplayAttributeData PoisonResistance;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, PoisonResistance)

	/** Callback de réplication pour PoisonResistance */
	UFUNCTION()
	virtual void OnRep_PoisonResistance(const FGameplayAttributeData& OldPoisonResistance);
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|RESISTANCE : BLEEDING RESISTANCE (Points de Bleeding Resistance)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de BleedingResistance actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BleedingResistance, Category = "Attributes|Resistance")
	FGameplayAttributeData BleedingResistance;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, BleedingResistance)

	/** Callback de réplication pour BleedingResistance */
	UFUNCTION()
	virtual void OnRep_BleedingResistance(const FGameplayAttributeData& OldBleedingResistance);
	
	// ═══════════════════════════════════════════════════════════════════════
	// PERKS : VITALITY (Points de Vitality)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de Vitality actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Vitality, Category = "Perks")
	FGameplayAttributeData Vitality;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, Vitality)

	/** Callback de réplication pour Vitality */
	UFUNCTION()
	virtual void OnRep_Vitality(const FGameplayAttributeData& OldVitality);
	
	// ═══════════════════════════════════════════════════════════════════════
	// PERKS : STRENGTH (Points de Strength)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de Strength actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category = "Perks")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, Strength)

	/** Callback de réplication pour Strength */
	UFUNCTION()
	virtual void OnRep_Strength(const FGameplayAttributeData& OldStrength);
	
	// ═══════════════════════════════════════════════════════════════════════
	// PERKS : ACCURACY (Points de Accuracy)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de Accuracy actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Accuracy, Category = "Perks")
	FGameplayAttributeData Accuracy;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, Accuracy)

	/** Callback de réplication pour Accuracy */
	UFUNCTION()
	virtual void OnRep_Accuracy(const FGameplayAttributeData& OldAccuracy);
	
	// ═══════════════════════════════════════════════════════════════════════
	// PERKS : INTELLIGENCE (Points de Intelligence)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de Intelligence actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intelligence, Category = "Perks")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, Intelligence)

	/** Callback de réplication pour Intelligence */
	UFUNCTION()
	virtual void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence);
	
	// ═══════════════════════════════════════════════════════════════════════
	// PERKS : MENTALITY (Points de Mentality)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de Mentality actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mentality, Category = "Perks")
	FGameplayAttributeData Mentality;
	ATTRIBUTE_ACCESSORS(UArchetypeAttributeSet, Mentality)

	/** Callback de réplication pour Mentality */
	UFUNCTION()
	virtual void OnRep_Mentality(const FGameplayAttributeData& OldMentality);
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS FUTURS (Phase 8+)
	// ═══════════════════════════════════════════════════════════════════════
	
	// TODO PHASE 8+ : Ajouter les stats secondaires
	// - Strength (Force)
	// - Intelligence
	// - Agility (Agilité)
	// - Armor (Armure)
	// - MagicResist (Résistance magique)
	// - AttackPower (Puissance d'attaque)
	// - SpellPower (Puissance magique)
	// - CritChance (Chance de coup critique)
	// - etc.

	// ═══════════════════════════════════════════════════════════════════════
	// MÉTHODES OVERRIDES
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Override pour enregistrer les propriétés répliquées */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * Appelé avant qu'un attribut soit modifié
	 * Permet de clamper les valeurs (ex: Health ne peut pas dépasser MaxHealth)
	 */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/**
	 * Appelé après qu'un GameplayEffect a modifié un attribut
	 * Permet de gérer la mort, les effets secondaires, etc.
	 */
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};

