// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/Effects/GE_PerkToRegenRates.h"
#include "Gameplay/AbilitySystem/DefaultAttributeSet.h"
#include "Gameplay/AbilitySystem/MMC/MMC_HealthRegenRate.h"
#include "Gameplay/AbilitySystem/MMC/MMC_ManaRegenRate.h"
#include "Gameplay/AbilitySystem/MMC/MMC_StaminaRegenRate.h"
#include "Gameplay/AbilitySystem/MMC/MMC_FocusRegenRate.h"

UGE_PerkToRegenRates::UGE_PerkToRegenRates()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	// Modifier 1: HealthRegenRate
	{
		FGameplayModifierInfo HealthRegenRateModifier;
		HealthRegenRateModifier.Attribute = UDefaultAttributeSet::GetHealthRegenRateAttribute();
		HealthRegenRateModifier.ModifierOp = EGameplayModOp::Override;

		FCustomCalculationBasedFloat CustomCalc;
		CustomCalc.CalculationClassMagnitude = UMMC_HealthRegenRate::StaticClass();
		HealthRegenRateModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(CustomCalc);

		Modifiers.Add(HealthRegenRateModifier);
	}

	// Modifier 2: ManaRegenRate
	{
		FGameplayModifierInfo ManaRegenRateModifier;
		ManaRegenRateModifier.Attribute = UDefaultAttributeSet::GetManaRegenRateAttribute();
		ManaRegenRateModifier.ModifierOp = EGameplayModOp::Override;

		FCustomCalculationBasedFloat CustomCalc;
		CustomCalc.CalculationClassMagnitude = UMMC_ManaRegenRate::StaticClass();
		ManaRegenRateModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(CustomCalc);

		Modifiers.Add(ManaRegenRateModifier);
	}

	// Modifier 3: StaminaRegenRate
	{
		FGameplayModifierInfo StaminaRegenRateModifier;
		StaminaRegenRateModifier.Attribute = UDefaultAttributeSet::GetStaminaRegenRateAttribute();
		StaminaRegenRateModifier.ModifierOp = EGameplayModOp::Override;

		FCustomCalculationBasedFloat CustomCalc;
		CustomCalc.CalculationClassMagnitude = UMMC_StaminaRegenRate::StaticClass();
		StaminaRegenRateModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(CustomCalc);

		Modifiers.Add(StaminaRegenRateModifier);
	}

	// Modifier 4: FocusRegenRate
	{
		FGameplayModifierInfo FocusRegenRateModifier;
		FocusRegenRateModifier.Attribute = UDefaultAttributeSet::GetFocusRegenRateAttribute();
		FocusRegenRateModifier.ModifierOp = EGameplayModOp::Override;

		FCustomCalculationBasedFloat CustomCalc;
		CustomCalc.CalculationClassMagnitude = UMMC_FocusRegenRate::StaticClass();
		FocusRegenRateModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(CustomCalc);

		Modifiers.Add(FocusRegenRateModifier);
	}
}
