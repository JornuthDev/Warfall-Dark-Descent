// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/ArchetypeAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Gameplay/AbilitySystem/GlobalAbilitySystem.h"
#include "Gameplay/AbilitySystem/GlobalAttributeSet.h"
#include "Interfaces/SystemsInterface.h"
#include "SaveGame/PlayerSaveGame.h"
#include "Utils/Helpers/SystemsHelpers.h"
#include "Utils/Types/AbilityTypes.h"

// ═══════════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════════

UArchetypeAttributeSet::UArchetypeAttributeSet()
{
	
}

void UArchetypeAttributeSet::InitAttributes(const EClassArchetype Archetype)
{
	ArchetypeData = UPlayerSaveGame::GetDefaultGameplay(Archetype);
	if (!ArchetypeData)
	{
		ArchetypeData = new FInternalAttributeData();
	}

	int32 Level = static_cast<int32>(ISystemsInterface::Execute_GetGlobalAbilitySystem(GetOwningAbilitySystemComponent()->GetOwnerActor())->GetNumericAttribute(UGlobalAttributeSet::GetLevelAttribute()));
	if (Level <= 0)
	{
		Level = 1;
	}

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── 1. Initialiser les Base Attributes (depuis Data) ─────────────────────────
	// Ces valeurs sont persistantes et peuvent être modifiées par des talents/quêtes
	InitBaseHealth(ArchetypeData->Health);
	InitBaseMana(ArchetypeData->Mana);
	InitBaseStamina(ArchetypeData->Stamina);
	InitBaseFocus(ArchetypeData->Focus);

	InitBaseHealthRegenRate(ArchetypeData->HealthRegenRate);
	InitBaseManaRegenRate(ArchetypeData->ManaRegenRate);
	InitBaseStaminaRegenRate(ArchetypeData->StaminaRegenRate);
	InitBaseFocusRegenRate(ArchetypeData->FocusRegenRate);

	InitBasePhysicDamageMultiplier(ArchetypeData->PhysicDamageMultiplier);
	InitBaseMagicDamageMultiplier(ArchetypeData->MagicDamageMultiplier);
	InitBaseAccuracyDamageMultiplier(ArchetypeData->AccuracyDamageMultiplier);
	InitBaseCriticalChance(ArchetypeData->CriticalChance);
	InitBaseCriticalDamage(ArchetypeData->CriticalDamage);

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── 2. Initialiser les Perks (Primary Attributes) ────────────────────────────
	InitVitality(CalculatePerkPoints(Level, ArchetypeData->Vitality));
	UE_LOG(LogTemp, Log, TEXT("ArchetypeAttributeSet::Init: Vitality = %f"), CalculatePerkPoints(Level, ArchetypeData->Vitality));
	InitStrength(CalculatePerkPoints(Level, ArchetypeData->Strength));
	UE_LOG(LogTemp, Log, TEXT("ArchetypeAttributeSet::Init: Strength = %f"), CalculatePerkPoints(Level, ArchetypeData->Strength));
	InitAccuracy(CalculatePerkPoints(Level, ArchetypeData->Accuracy));
	UE_LOG(LogTemp, Log, TEXT("ArchetypeAttributeSet::Init: Accuracy = %f"), CalculatePerkPoints(Level, ArchetypeData->Accuracy));
	InitIntelligence(CalculatePerkPoints(Level, ArchetypeData->Intelligence));
	UE_LOG(LogTemp, Log, TEXT("ArchetypeAttributeSet::Init: Intelligence = %f"), CalculatePerkPoints(Level, ArchetypeData->Intelligence));
	InitMentality(CalculatePerkPoints(Level, ArchetypeData->Mentality));
	UE_LOG(LogTemp, Log, TEXT("ArchetypeAttributeSet::Init: Mentality = %f"), CalculatePerkPoints(Level, ArchetypeData->Mentality));

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── 3. Initialiser les Resistances (fixes depuis Data) ───────────────────────
	InitArmor(ArchetypeData->Armor);
	InitIceResistance(ArchetypeData->IceResistance);
	InitFireResistance(ArchetypeData->FireResistance);
	InitLightningResistance(ArchetypeData->LightningResistance);
	InitHolyResistance(ArchetypeData->HolyResistance);
	InitPoisonResistance(ArchetypeData->PoisonResistance);
	InitBleedingResistance(ArchetypeData->BleedingResistance);

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── 4. NE PAS initialiser MaxHealth, MaxMana, RegenRates, DamageMultipliers, etc.
	// Ces attributs seront calculés automatiquement par les GameplayEffects permanents
	// appliqués dans ArchetypeAbilitySystem::InitializeAttributes()
}

void UArchetypeAttributeSet::OnLevelUp()
{
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── 1. Sauvegarder les pourcentages AVANT modification ───────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	const float HealthPercentage = GetHealth() / FMath::Max(GetMaxHealth(), 1.0f);
	const float ManaPercentage = GetMana() / FMath::Max(GetMaxMana(), 1.0f);
	const float StaminaPercentage = GetStamina() / FMath::Max(GetMaxStamina(), 1.0f);
	const float FocusPercentage = GetFocus() / FMath::Max(GetMaxFocus(), 1.0f);

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── 2. Modifier les Perks (Primary Attributes) ───────────────────────────────
	// Le GAS va automatiquement recalculer MaxHealth, MaxMana, etc. via les GE permanents
	// ───────────────────────────────────────────────────────────────────────────────

	SetVitality(GetVitality() + CalculatePerkPoints(1, ArchetypeData->Vitality));
	SetStrength(GetStrength() + CalculatePerkPoints(1, ArchetypeData->Strength));
	SetAccuracy(GetAccuracy() + CalculatePerkPoints(1, ArchetypeData->Accuracy));
	SetIntelligence(GetIntelligence() + CalculatePerkPoints(1, ArchetypeData->Intelligence));
	SetMentality(GetMentality() + CalculatePerkPoints(1, ArchetypeData->Mentality));

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── 3. Restaurer les pourcentages sur les nouvelles valeurs Max ──────────────
	// Les GameplayEffects permanents ont déjà recalculé MaxHealth, MaxMana, etc.
	// ───────────────────────────────────────────────────────────────────────────────

	SetHealth(HealthPercentage * GetMaxHealth());
	SetMana(ManaPercentage * GetMaxMana());
	SetStamina(StaminaPercentage * GetMaxStamina());
	SetFocus(FocusPercentage * GetMaxFocus());
}

// ═══════════════════════════════════════════════════════════════════════
// CALCUL
// ═══════════════════════════════════════════════════════════════════════

float UArchetypeAttributeSet::CalculatePerkPoints(const int32 Level, const float PerkWeight) const
{
	return FMath::FloorToInt(static_cast<float>(Level) / PerkWeight);
}


// ═══════════════════════════════════════════════════════════════════════════
// RÉPLICATION
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// ───────────────────────────────────────────────────────────────────────
	// BASE ATTRIBUTES
	// ───────────────────────────────────────────────────────────────────────
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, BaseHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, BaseMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, BaseStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, BaseFocus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, BaseHealthRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, BaseManaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, BaseStaminaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, BaseFocusRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, BasePhysicDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, BaseMagicDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, BaseAccuracyDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, BaseCriticalChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, BaseCriticalDamage, COND_None, REPNOTIFY_Always);

	// ───────────────────────────────────────────────────────────────────────
	// HEALTH
	// ───────────────────────────────────────────────────────────────────────
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, HealthRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);

	// ───────────────────────────────────────────────────────────────────────
	// MANA
	// ───────────────────────────────────────────────────────────────────────
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, ManaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	// ───────────────────────────────────────────────────────────────────────
	// STAMINA
	// ───────────────────────────────────────────────────────────────────────
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, StaminaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);

	// ───────────────────────────────────────────────────────────────────────
	// FOCUS
	// ───────────────────────────────────────────────────────────────────────
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, Focus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, FocusRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, MaxFocus, COND_None, REPNOTIFY_Always);

	// ───────────────────────────────────────────────────────────────────────
	// ATTACK
	// ───────────────────────────────────────────────────────────────────────
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, PhysicDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, MagicDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, AccuracyDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, CriticalChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, CriticalDamage, COND_None, REPNOTIFY_Always);

	// ───────────────────────────────────────────────────────────────────────
	// RESISTANCE
	// ───────────────────────────────────────────────────────────────────────
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, IceResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, HolyResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, PoisonResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, BleedingResistance, COND_None, REPNOTIFY_Always);

	// ───────────────────────────────────────────────────────────────────────
	// PERKS
	// ───────────────────────────────────────────────────────────────────────
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, Vitality, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, Accuracy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArchetypeAttributeSet, Mentality, COND_None, REPNOTIFY_Always);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - BASE ATTRIBUTES
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::OnRep_BaseHealth(const FGameplayAttributeData& OldBaseHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, BaseHealth, OldBaseHealth);
}

void UArchetypeAttributeSet::OnRep_BaseMana(const FGameplayAttributeData& OldBaseMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, BaseMana, OldBaseMana);
}

void UArchetypeAttributeSet::OnRep_BaseStamina(const FGameplayAttributeData& OldBaseStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, BaseStamina, OldBaseStamina);
}

void UArchetypeAttributeSet::OnRep_BaseFocus(const FGameplayAttributeData& OldBaseFocus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, BaseFocus, OldBaseFocus);
}

void UArchetypeAttributeSet::OnRep_BaseHealthRegenRate(const FGameplayAttributeData& OldBaseHealthRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, BaseHealthRegenRate, OldBaseHealthRegenRate);
}

void UArchetypeAttributeSet::OnRep_BaseManaRegenRate(const FGameplayAttributeData& OldBaseManaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, BaseManaRegenRate, OldBaseManaRegenRate);
}

void UArchetypeAttributeSet::OnRep_BaseStaminaRegenRate(const FGameplayAttributeData& OldBaseStaminaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, BaseStaminaRegenRate, OldBaseStaminaRegenRate);
}

void UArchetypeAttributeSet::OnRep_BaseFocusRegenRate(const FGameplayAttributeData& OldBaseFocusRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, BaseFocusRegenRate, OldBaseFocusRegenRate);
}

void UArchetypeAttributeSet::OnRep_BasePhysicDamageMultiplier(const FGameplayAttributeData& OldBasePhysicDamageMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, BasePhysicDamageMultiplier, OldBasePhysicDamageMultiplier);
}

void UArchetypeAttributeSet::OnRep_BaseMagicDamageMultiplier(const FGameplayAttributeData& OldBaseMagicDamageMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, BaseMagicDamageMultiplier, OldBaseMagicDamageMultiplier);
}

void UArchetypeAttributeSet::OnRep_BaseAccuracyDamageMultiplier(const FGameplayAttributeData& OldBaseAccuracyDamageMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, BaseAccuracyDamageMultiplier, OldBaseAccuracyDamageMultiplier);
}

void UArchetypeAttributeSet::OnRep_BaseCriticalChance(const FGameplayAttributeData& OldBaseCriticalChance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, BaseCriticalChance, OldBaseCriticalChance);
}

void UArchetypeAttributeSet::OnRep_BaseCriticalDamage(const FGameplayAttributeData& OldBaseCriticalDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, BaseCriticalDamage, OldBaseCriticalDamage);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - HEALTH
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, Health, OldHealth);
}

void UArchetypeAttributeSet::OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, HealthRegenRate, OldHealthRegenRate);
}

void UArchetypeAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, MaxHealth, OldMaxHealth);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - MANA
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, Mana, OldMana);
}

void UArchetypeAttributeSet::OnRep_ManaRegenRate(const FGameplayAttributeData& OldManaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, ManaRegenRate, OldManaRegenRate);
}

void UArchetypeAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, MaxMana, OldMaxMana);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - STAMINA
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, Stamina, OldStamina);
}

void UArchetypeAttributeSet::OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, StaminaRegenRate, OldStaminaRegenRate);
}

void UArchetypeAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, MaxStamina, OldMaxStamina);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - FOCUS
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::OnRep_Focus(const FGameplayAttributeData& OldFocus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, Focus, OldFocus);
}

void UArchetypeAttributeSet::OnRep_FocusRegenRate(const FGameplayAttributeData& OldFocusRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, FocusRegenRate, OldFocusRegenRate);
}

void UArchetypeAttributeSet::OnRep_MaxFocus(const FGameplayAttributeData& OldMaxFocus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, MaxFocus, OldMaxFocus);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - PHYSIC DAMAGE MULTIPLIER
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::OnRep_PhysicDamageMultiplier(const FGameplayAttributeData& OldPhysicDamageMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, PhysicDamageMultiplier, OldPhysicDamageMultiplier);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - MAGIC DAMAGE MULTIPLIER
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::OnRep_MagicDamageMultiplier(const FGameplayAttributeData& OldMagicDamageMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, MagicDamageMultiplier, OldMagicDamageMultiplier);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - ACCURACY DAMAGE MULTIPLIER
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::OnRep_AccuracyDamageMultiplier(const FGameplayAttributeData& OldAccuracyDamageMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, AccuracyDamageMultiplier, OldAccuracyDamageMultiplier);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - CRITICAL CHANCE
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::OnRep_CriticalChance(const FGameplayAttributeData& OldCriticalChance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, CriticalChance, OldCriticalChance);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - CRITICAL DAMAGE
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::OnRep_CriticalDamage(const FGameplayAttributeData& OldCriticalDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, CriticalDamage, OldCriticalDamage);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - ARMOR
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, Armor, OldArmor);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - ICE RESISTANCE
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::OnRep_IceResistance(const FGameplayAttributeData& OldIceResistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, IceResistance, OldIceResistance);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - FIRE RESISTANCE
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, FireResistance, OldFireResistance);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - LIGHTNING RESISTANCE
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, LightningResistance, OldLightningResistance);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - HOLY RESISTANCE
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::OnRep_HolyResistance(const FGameplayAttributeData& OldHolyResistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, HolyResistance, OldHolyResistance);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - POISON RESISTANCE
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::OnRep_PoisonResistance(const FGameplayAttributeData& OldPoisonResistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, PoisonResistance, OldPoisonResistance);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - BLEEDING RESISTANCE
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::OnRep_BleedingResistance(const FGameplayAttributeData& OldBleedingResistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, BleedingResistance, OldBleedingResistance);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - VITALITY
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::OnRep_Vitality(const FGameplayAttributeData& OldVitality)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, Vitality, OldVitality);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - STRENGTH
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, Strength, OldStrength);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - ACCURACY
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::OnRep_Accuracy(const FGameplayAttributeData& OldAccuracy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, Accuracy, OldAccuracy);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - INTELLIGENCE
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, Intelligence, OldIntelligence);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - MENTALITY
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::OnRep_Mentality(const FGameplayAttributeData& OldMentality)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArchetypeAttributeSet, Mentality, OldMentality);
}

// ═══════════════════════════════════════════════════════════════════════════
// PRE ATTRIBUTE CHANGE - Clamping des valeurs
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// ───────────────────────────────────────────────────────────────────────
	// HEALTH : Ne peut pas être négatif ni dépasser MaxHealth
	// ───────────────────────────────────────────────────────────────────────
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}

	// ───────────────────────────────────────────────────────────────────────
	// MAX HEALTH : Ne peut pas être négatif
	// ───────────────────────────────────────────────────────────────────────
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f); // Minimum 1 HP
	}

	// ───────────────────────────────────────────────────────────────────────
	// MANA : Ne peut pas être négatif ni dépasser MaxMana
	// ───────────────────────────────────────────────────────────────────────
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
	}

	// ───────────────────────────────────────────────────────────────────────
	// MAX MANA : Ne peut pas être négatif
	// ───────────────────────────────────────────────────────────────────────
	else if (Attribute == GetMaxManaAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f); // Peut être 0 (certains archétypes n'ont pas de mana)
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// POST GAMEPLAY EFFECT EXECUTE - Logique post-modification
// ═══════════════════════════════════════════════════════════════════════════

void UArchetypeAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// Récupérer l'attribut qui a été modifié
	FGameplayAttribute Attribute = Data.EvaluatedData.Attribute;

	// ───────────────────────────────────────────────────────────────────────
	// HEALTH : Gérer la mort si Health atteint 0
	// ───────────────────────────────────────────────────────────────────────
	if (Attribute == GetHealthAttribute())
	{
		// Clamper Health entre 0 et MaxHealth
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));

		// Si le personnage est mort (Health = 0)
		if (GetHealth() <= 0.0f)
		{
			// TODO PHASE 10+ : Déclencher la mort du personnage
			// - Désactiver les inputs
			// - Jouer l'animation de mort
			// - Notifier le GameMode
			// - Appeler un événement Blueprint "OnDeath"
			
			UE_LOG(LogTemp, Warning, TEXT("ArchetypeAttributeSet: Personnage mort (Health = 0)"));
		}
	}

	// ───────────────────────────────────────────────────────────────────────
	// MAX HEALTH : Ajuster Health si MaxHealth change
	// ───────────────────────────────────────────────────────────────────────
	else if (Attribute == GetMaxHealthAttribute())
	{
		// Si MaxHealth diminue, s'assurer que Health ne dépasse pas
		if (GetHealth() > GetMaxHealth())
		{
			SetHealth(GetMaxHealth());
		}
	}

	// ───────────────────────────────────────────────────────────────────────
	// MANA : Clamper entre 0 et MaxMana
	// ───────────────────────────────────────────────────────────────────────
	else if (Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));
	}

	// ───────────────────────────────────────────────────────────────────────
	// MAX MANA : Ajuster Mana si MaxMana change
	// ───────────────────────────────────────────────────────────────────────
	else if (Attribute == GetMaxManaAttribute())
	{
		// Si MaxMana diminue, s'assurer que Mana ne dépasse pas
		if (GetMana() > GetMaxMana())
		{
			SetMana(GetMaxMana());
		}
	}
}