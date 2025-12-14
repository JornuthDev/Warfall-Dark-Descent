// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/Effects/GE_PerkToCriticals.h"
#include "Gameplay/AbilitySystem/ArchetypeAttributeSet.h"
#include "Gameplay/AbilitySystem/MMC/MMC_CriticalChance.h"
#include "Gameplay/AbilitySystem/MMC/MMC_CriticalDamage.h"

UGE_PerkToCriticals::UGE_PerkToCriticals()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	// Modifier 1: CriticalChance
	{
		FGameplayModifierInfo CriticalChanceModifier;
		CriticalChanceModifier.Attribute = UArchetypeAttributeSet::GetCriticalChanceAttribute();
		CriticalChanceModifier.ModifierOp = EGameplayModOp::Override;

		FCustomCalculationBasedFloat CustomCalc;
		CustomCalc.CalculationClassMagnitude = UMMC_CriticalChance::StaticClass();
		CriticalChanceModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(CustomCalc);

		Modifiers.Add(CriticalChanceModifier);
	}

	// Modifier 2: CriticalDamage
	{
		FGameplayModifierInfo CriticalDamageModifier;
		CriticalDamageModifier.Attribute = UArchetypeAttributeSet::GetCriticalDamageAttribute();
		CriticalDamageModifier.ModifierOp = EGameplayModOp::Override;

		FCustomCalculationBasedFloat CustomCalc;
		CustomCalc.CalculationClassMagnitude = UMMC_CriticalDamage::StaticClass();
		CriticalDamageModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(CustomCalc);

		Modifiers.Add(CriticalDamageModifier);
	}
}
