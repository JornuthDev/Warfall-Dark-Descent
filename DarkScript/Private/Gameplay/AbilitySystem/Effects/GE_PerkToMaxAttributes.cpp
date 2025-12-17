// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/Effects/GE_PerkToMaxAttributes.h"
#include "Gameplay/AbilitySystem/DefaultAttributeSet.h"
#include "Gameplay/AbilitySystem/MMC/MMC_MaxHealth.h"
#include "Gameplay/AbilitySystem/MMC/MMC_MaxMana.h"
#include "Gameplay/AbilitySystem/MMC/MMC_MaxStamina.h"
#include "Gameplay/AbilitySystem/MMC/MMC_MaxFocus.h"

UGE_PerkToMaxAttributes::UGE_PerkToMaxAttributes()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	// Modifier 1: MaxHealth
	{
		FGameplayModifierInfo MaxHealthModifier;
		MaxHealthModifier.Attribute = UDefaultAttributeSet::GetMaxHealthAttribute();
		MaxHealthModifier.ModifierOp = EGameplayModOp::Override;
		
		FCustomCalculationBasedFloat CustomCalc;
		CustomCalc.CalculationClassMagnitude = UMMC_MaxHealth::StaticClass();
		MaxHealthModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(CustomCalc);
		
		Modifiers.Add(MaxHealthModifier);
	}

	// Modifier 2: MaxMana
	{
		FGameplayModifierInfo MaxManaModifier;
		MaxManaModifier.Attribute = UDefaultAttributeSet::GetMaxManaAttribute();
		MaxManaModifier.ModifierOp = EGameplayModOp::Override;

		FCustomCalculationBasedFloat CustomCalc;
		CustomCalc.CalculationClassMagnitude = UMMC_MaxMana::StaticClass();
		MaxManaModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(CustomCalc);

		Modifiers.Add(MaxManaModifier);
	}

	// Modifier 3: MaxStamina
	{
		FGameplayModifierInfo MaxStaminaModifier;
		MaxStaminaModifier.Attribute = UDefaultAttributeSet::GetMaxStaminaAttribute();
		MaxStaminaModifier.ModifierOp = EGameplayModOp::Override;

		FCustomCalculationBasedFloat CustomCalc;
		CustomCalc.CalculationClassMagnitude = UMMC_MaxStamina::StaticClass();
		MaxStaminaModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(CustomCalc);

		Modifiers.Add(MaxStaminaModifier);
	}

	// Modifier 4: MaxFocus
	{
		FGameplayModifierInfo MaxFocusModifier;
		MaxFocusModifier.Attribute = UDefaultAttributeSet::GetMaxFocusAttribute();
		MaxFocusModifier.ModifierOp = EGameplayModOp::Override;

		FCustomCalculationBasedFloat CustomCalc;
		CustomCalc.CalculationClassMagnitude = UMMC_MaxFocus::StaticClass();
		MaxFocusModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(CustomCalc);

		Modifiers.Add(MaxFocusModifier);
	}
}
