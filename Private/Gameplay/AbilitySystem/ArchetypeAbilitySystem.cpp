// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/ArchetypeAbilitySystem.h"
#include "Gameplay/AbilitySystem/ArchetypeAttributeSet.h"
#include "Gameplay/AbilitySystem/Effects/GE_PerkToCriticals.h"
#include "Gameplay/AbilitySystem/Effects/GE_PerkToDamageMultipliers.h"
#include "Gameplay/AbilitySystem/Effects/GE_PerkToMaxAttributes.h"
#include "Gameplay/AbilitySystem/Effects/GE_PerkToRegenRates.h"

// ═══════════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════════

UArchetypeAbilitySystem::UArchetypeAbilitySystem()
{
	// Configurer la réplication
	SetIsReplicatedByDefault(true);
	
	// Mode de réplication : Full (répliquer tous les GameplayEffects)
	// Alternative : Mixed (optimisé pour le multijoueur, à tester selon les besoins)
	ReplicationMode = EGameplayEffectReplicationMode::Full;
	
	GE_PerkToMaxAttributes = UGE_PerkToMaxAttributes::StaticClass();
	GE_PerkToRegenRates = UGE_PerkToRegenRates::StaticClass();
	GE_PerkToDamageMultipliers = UGE_PerkToDamageMultipliers::StaticClass();
	GE_PerkToCriticals = UGE_PerkToCriticals::StaticClass();
}

// ═══════════════════════════════════════════════════════════════════════════
// BEGIN PLAY
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAbilitySystem::BeginPlay()
{
	Super::BeginPlay();

	// Log pour vérifier que le component est bien créé
	UE_LOG(LogTemp, Log, TEXT("ArchetypeAbilitySystem: BeginPlay appelé sur %s"), 
		*GetOwner()->GetName());
}

// ═══════════════════════════════════════════════════════════════════════════
// MÉTHODES CUSTOM
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAbilitySystem::InitializeAttributes(const EClassArchetype Archetype)
{
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── 1. Récupérer l'ArchetypeAttributeSet ─────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	UArchetypeAttributeSet* ArchetypeAttributeSet = const_cast<UArchetypeAttributeSet*>(GetSet<UArchetypeAttributeSet>());
	if (!ArchetypeAttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("ArchetypeAbilitySystem::InitializeAttributes - ArchetypeAttributeSet not found!"));
		return;
	}

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── 2. Initialiser les attributs de base et Perks ────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	ArchetypeAttributeSet->InitAttributes(Archetype);

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── 3. Appliquer les GameplayEffects permanents ──────────────────────────────
	// Ces GE calculent automatiquement MaxHealth, MaxMana, RegenRates, DamageMultipliers, etc.
	// depuis les Base Attributes et les Perks
	// ───────────────────────────────────────────────────────────────────────────────

	FGameplayEffectContextHandle EffectContext = MakeEffectContext();
	EffectContext.AddSourceObject(this);

	// GE #1 : MaxHealth, MaxMana, MaxStamina, MaxFocus (depuis Perks)
	if (GE_PerkToMaxAttributes)
	{
		FGameplayEffectSpecHandle SpecMaxAttributes = MakeOutgoingSpec(
			GE_PerkToMaxAttributes, 1.0f, EffectContext
		);
		if (SpecMaxAttributes.IsValid())
		{
			ApplyGameplayEffectSpecToSelf(*SpecMaxAttributes.Data.Get());
		}
	}

	// GE #2 : HealthRegenRate, ManaRegenRate, StaminaRegenRate, FocusRegenRate
	// DOIT être appliqué APRÈS #1 car RegenRate dépend de MaxAttributes
	if (GE_PerkToRegenRates)
	{
		FGameplayEffectSpecHandle SpecRegenRates = MakeOutgoingSpec(
			GE_PerkToRegenRates, 1.0f, EffectContext
		);
		if (SpecRegenRates.IsValid())
		{
			ApplyGameplayEffectSpecToSelf(*SpecRegenRates.Data.Get());
		}
	}

	// GE #3 : PhysicDamageMultiplier, MagicDamageMultiplier, AccuracyDamageMultiplier
	if (GE_PerkToDamageMultipliers)
	{
		FGameplayEffectSpecHandle SpecDamage = MakeOutgoingSpec(
			GE_PerkToDamageMultipliers, 1.0f, EffectContext
		);
		if (SpecDamage.IsValid())
		{
			ApplyGameplayEffectSpecToSelf(*SpecDamage.Data.Get());
		}
	}

	// GE #4 : CriticalChance, CriticalDamage
	if (GE_PerkToCriticals)
	{
		FGameplayEffectSpecHandle SpecCriticals = MakeOutgoingSpec(
			GE_PerkToCriticals, 1.0f, EffectContext
		);
		if (SpecCriticals.IsValid())
		{
			ApplyGameplayEffectSpecToSelf(*SpecCriticals.Data.Get());
		}
	}

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── 4. Initialiser Health = MaxHealth (100% HP au départ) ────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	SetNumericAttributeBase(
		UArchetypeAttributeSet::GetHealthAttribute(),
		GetNumericAttribute(UArchetypeAttributeSet::GetMaxHealthAttribute())
	);

	// Initialiser Mana = MaxMana
	SetNumericAttributeBase(
		UArchetypeAttributeSet::GetManaAttribute(),
		GetNumericAttribute(UArchetypeAttributeSet::GetMaxManaAttribute())
	);

	// Initialiser Stamina = MaxStamina
	SetNumericAttributeBase(
		UArchetypeAttributeSet::GetStaminaAttribute(),
		GetNumericAttribute(UArchetypeAttributeSet::GetMaxStaminaAttribute())
	);

	// Initialiser Focus = MaxFocus
	SetNumericAttributeBase(
		UArchetypeAttributeSet::GetFocusAttribute(),
		GetNumericAttribute(UArchetypeAttributeSet::GetMaxFocusAttribute())
	);
}

// ═══════════════════════════════════════════════════════════════════════════
// MÉTHODES CUSTOM (à implémenter en Phase 8+)
// ═══════════════════════════════════════════════════════════════════════════

/*
float UArchetypeAbilitySystem::GetFinalStat(FGameplayAttribute Attribute) const
{
	// Récupérer la valeur de base de l'attribut
	float BaseValue = GetNumericAttribute(Attribute);
	
	// TODO : Ajouter les modificateurs actifs (équipements, buffs, talents)
	// float Modifiers = CalculateAttributeModifiers(Attribute);
	
	return BaseValue; // + Modifiers;
}

void UArchetypeAbilitySystem::ApplyEquipmentEffects(const FEquipmentEntry& Equipment)
{
	// TODO PHASE 8+ : Appliquer les GameplayEffects de l'équipement
	// 1. Récupérer les GE depuis l'ItemData de l'équipement
	// 2. Les appliquer à l'ASC
	// 3. Stocker les handles dans ActiveEquipmentEffects
}

void UArchetypeAbilitySystem::RemoveEquipmentEffects(const FEquipmentEntry& Equipment)
{
	// TODO PHASE 8+ : Retirer les GameplayEffects de l'équipement
	// 1. Récupérer le handle depuis ActiveEquipmentEffects
	// 2. Appeler RemoveActiveGameplayEffect(Handle)
	// 3. Supprimer l'entrée de la map
}

void UArchetypeAbilitySystem::ActivateArchetypePassive(EClassArchetype Archetype)
{
	// TODO PHASE 8+ : Activer les passives spécifiques à l'archétype
	// Exemple : Tank = +20% Armor, Healer = +10% Healing Power, etc.
}

bool UArchetypeAbilitySystem::IsDead() const
{
	// Vérifier si Health est à 0
	float Health = GetNumericAttribute(UArchetypeAttributeSet::GetHealthAttribute());
	return Health <= 0.0f;
}
*/