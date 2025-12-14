// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_HealthRegenRate.h"
#include "Gameplay/AbilitySystem/ArchetypeAttributeSet.h"
#include "Utils/Helpers/SystemsHelpers.h"

UMMC_HealthRegenRate::UMMC_HealthRegenRate()
{
	FGameplayEffectAttributeCaptureDefinition MaxHealthDef(
		UArchetypeAttributeSet::GetMaxHealthAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	FGameplayEffectAttributeCaptureDefinition BaseHealthRegenRateDef(
		UArchetypeAttributeSet::GetBaseHealthRegenRateAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	RelevantAttributesToCapture.Add(MaxHealthDef);
	RelevantAttributesToCapture.Add(BaseHealthRegenRateDef);
}

float UMMC_HealthRegenRate::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float MaxHealth = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetMaxHealthAttribute()),
		Spec,
		EvaluateParameters,
		MaxHealth
	);

	float BaseHealthRegenRate = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetBaseHealthRegenRateAttribute()),
		Spec,
		EvaluateParameters,
		BaseHealthRegenRate
	);

	// Formule : HealthRegenRate = MaxHealth × BaseHealthRegenRate
	const float HealthRegenRate = MaxHealth * BaseHealthRegenRate;

	return HealthRegenRate;
}
