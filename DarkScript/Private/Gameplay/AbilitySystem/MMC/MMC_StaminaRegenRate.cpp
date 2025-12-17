// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_StaminaRegenRate.h"
#include "Gameplay/AbilitySystem/DefaultAttributeSet.h"
#include "GameplayEffectExecutionCalculation.h"
#include "Gameplay/AbilitySystem/AdvancedAttributeSet.h"

struct FStaminaRegenRateStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxStamina);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseStaminaRegenRate);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Strength);
	DECLARE_ATTRIBUTE_CAPTUREDEF(StaminaRegenBonus);

	FStaminaRegenRateStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, MaxStamina, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, BaseStaminaRegenRate, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAdvancedAttributeSet, Strength, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, StaminaRegenBonus, Target, false);
	}
};

static const FStaminaRegenRateStatics& StaminaRegenRateStatics()
{
	static FStaminaRegenRateStatics Statics;
	return Statics;
}

UMMC_StaminaRegenRate::UMMC_StaminaRegenRate()
{
	RelevantAttributesToCapture.Add(StaminaRegenRateStatics().MaxStaminaDef);
	RelevantAttributesToCapture.Add(StaminaRegenRateStatics().BaseStaminaRegenRateDef);
	RelevantAttributesToCapture.Add(StaminaRegenRateStatics().StrengthDef);
	RelevantAttributesToCapture.Add(StaminaRegenRateStatics().StaminaRegenBonusDef);
}

float UMMC_StaminaRegenRate::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float MaxStamina = 0.f;
	GetCapturedAttributeMagnitude(StaminaRegenRateStatics().MaxStaminaDef, Spec, EvaluateParameters, MaxStamina);

	float BaseStaminaRegenRate = 0.f;
	GetCapturedAttributeMagnitude(StaminaRegenRateStatics().BaseStaminaRegenRateDef, Spec, EvaluateParameters, BaseStaminaRegenRate);

	float Strength = 0.f;
	GetCapturedAttributeMagnitude(StaminaRegenRateStatics().StrengthDef, Spec, EvaluateParameters, Strength);

	float StaminaRegenBonus = 0.f;
	GetCapturedAttributeMagnitude(StaminaRegenRateStatics().StaminaRegenBonusDef, Spec, EvaluateParameters, StaminaRegenBonus);

	if (Spec.GetLevel() == 1)
	{
		return (MaxStamina * BaseStaminaRegenRate) + StaminaRegenBonus;
	}
	
	// Formule : StaminaRegenRate = MaxStamina × (BaseStaminaRegenRate + Strength × 0.001) + StaminaRegenBonus (équipements/buffs)
	return (MaxStamina * (BaseStaminaRegenRate + (Strength * 0.001f))) + StaminaRegenBonus;
}
