// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/Effects/GE_PerkToDamageMultipliers.h"
#include "Gameplay/AbilitySystem/ArchetypeAttributeSet.h"
#include "Gameplay/AbilitySystem/MMC/MMC_PhysicDamageMultiplier.h"
#include "Gameplay/AbilitySystem/MMC/MMC_MagicDamageMultiplier.h"
#include "Gameplay/AbilitySystem/MMC/MMC_AccuracyDamageMultiplier.h"

UGE_PerkToDamageMultipliers::UGE_PerkToDamageMultipliers()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	// Modifier 1: PhysicDamageMultiplier
	{
		FGameplayModifierInfo PhysicDamageModifier;
		PhysicDamageModifier.Attribute = UArchetypeAttributeSet::GetPhysicDamageMultiplierAttribute();
		PhysicDamageModifier.ModifierOp = EGameplayModOp::Override;

		FCustomCalculationBasedFloat CustomCalc;
		CustomCalc.CalculationClassMagnitude = UMMC_PhysicDamageMultiplier::StaticClass();
		PhysicDamageModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(CustomCalc);

		Modifiers.Add(PhysicDamageModifier);
	}

	// Modifier 2: MagicDamageMultiplier
	{
		FGameplayModifierInfo MagicDamageModifier;
		MagicDamageModifier.Attribute = UArchetypeAttributeSet::GetMagicDamageMultiplierAttribute();
		MagicDamageModifier.ModifierOp = EGameplayModOp::Override;

		FCustomCalculationBasedFloat CustomCalc;
		CustomCalc.CalculationClassMagnitude = UMMC_MagicDamageMultiplier::StaticClass();
		MagicDamageModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(CustomCalc);

		Modifiers.Add(MagicDamageModifier);
	}

	// Modifier 3: AccuracyDamageMultiplier
	{
		FGameplayModifierInfo AccuracyDamageModifier;
		AccuracyDamageModifier.Attribute = UArchetypeAttributeSet::GetAccuracyDamageMultiplierAttribute();
		AccuracyDamageModifier.ModifierOp = EGameplayModOp::Override;

		FCustomCalculationBasedFloat CustomCalc;
		CustomCalc.CalculationClassMagnitude = UMMC_AccuracyDamageMultiplier::StaticClass();
		AccuracyDamageModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(CustomCalc);

		Modifiers.Add(AccuracyDamageModifier);
	}
}
