// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_StaminaRegenRate.h"
#include "Gameplay/AbilitySystem/ArchetypeAttributeSet.h"
#include "Utils/Helpers/SystemsHelpers.h"

UMMC_StaminaRegenRate::UMMC_StaminaRegenRate()
{
	FGameplayEffectAttributeCaptureDefinition MaxStaminaDef(
		UArchetypeAttributeSet::GetMaxStaminaAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	FGameplayEffectAttributeCaptureDefinition BaseStaminaRegenRateDef(
		UArchetypeAttributeSet::GetBaseStaminaRegenRateAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	FGameplayEffectAttributeCaptureDefinition StrengthDef(
		UArchetypeAttributeSet::GetStrengthAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	RelevantAttributesToCapture.Add(MaxStaminaDef);
	RelevantAttributesToCapture.Add(BaseStaminaRegenRateDef);
	RelevantAttributesToCapture.Add(StrengthDef);
}

float UMMC_StaminaRegenRate::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float MaxStamina = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetMaxStaminaAttribute()),
		Spec,
		EvaluateParameters,
		MaxStamina
	);

	float BaseStaminaRegenRate = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetBaseStaminaRegenRateAttribute()),
		Spec,
		EvaluateParameters,
		BaseStaminaRegenRate
	);

	float Strength = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetStrengthAttribute()),
		Spec,
		EvaluateParameters,
		Strength
	);

	// Formule : StaminaRegenRate = MaxStamina × (BaseStaminaRegenRate + (Strength × 0.001))
	const float StaminaRegenRate = MaxStamina * (BaseStaminaRegenRate + (Strength * 0.001f));

	return StaminaRegenRate;
}
