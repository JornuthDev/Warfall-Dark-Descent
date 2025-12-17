// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_HealthRegenRate.h"
#include "Gameplay/AbilitySystem/DefaultAttributeSet.h"
#include "GameplayEffectExecutionCalculation.h"

struct FHealthRegenRateStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxHealth);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseHealthRegenRate);
	DECLARE_ATTRIBUTE_CAPTUREDEF(HealthRegenBonus);

	FHealthRegenRateStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, MaxHealth, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, BaseHealthRegenRate, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, HealthRegenBonus, Target, false);
	}
};

static const FHealthRegenRateStatics& HealthRegenRateStatics()
{
	static FHealthRegenRateStatics Statics;
	return Statics;
}

UMMC_HealthRegenRate::UMMC_HealthRegenRate()
{
	RelevantAttributesToCapture.Add(HealthRegenRateStatics().MaxHealthDef);
	RelevantAttributesToCapture.Add(HealthRegenRateStatics().BaseHealthRegenRateDef);
	RelevantAttributesToCapture.Add(HealthRegenRateStatics().HealthRegenBonusDef);
}

float UMMC_HealthRegenRate::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float MaxHealth = 0.f;
	GetCapturedAttributeMagnitude(HealthRegenRateStatics().MaxHealthDef, Spec, EvaluateParameters, MaxHealth);

	float BaseHealthRegenRate = 0.f;
	GetCapturedAttributeMagnitude(HealthRegenRateStatics().BaseHealthRegenRateDef, Spec, EvaluateParameters, BaseHealthRegenRate);

	float HealthRegenBonus = 0.f;
	GetCapturedAttributeMagnitude(HealthRegenRateStatics().HealthRegenBonusDef, Spec, EvaluateParameters, HealthRegenBonus);

	// Formule : HealthRegenRate = (MaxHealth × BaseHealthRegenRate) + HealthRegenBonus (équipements/buffs)
	return (MaxHealth * BaseHealthRegenRate) + HealthRegenBonus;
}
