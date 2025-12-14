// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_FocusRegenRate.h"
#include "Gameplay/AbilitySystem/ArchetypeAttributeSet.h"
#include "Utils/Helpers/SystemsHelpers.h"

UMMC_FocusRegenRate::UMMC_FocusRegenRate()
{
	FGameplayEffectAttributeCaptureDefinition MaxFocusDef(
		UArchetypeAttributeSet::GetMaxFocusAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	FGameplayEffectAttributeCaptureDefinition BaseFocusRegenRateDef(
		UArchetypeAttributeSet::GetBaseFocusRegenRateAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	FGameplayEffectAttributeCaptureDefinition AccuracyDef(
		UArchetypeAttributeSet::GetAccuracyAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	RelevantAttributesToCapture.Add(MaxFocusDef);
	RelevantAttributesToCapture.Add(BaseFocusRegenRateDef);
	RelevantAttributesToCapture.Add(AccuracyDef);
}

float UMMC_FocusRegenRate::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float MaxFocus = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetMaxFocusAttribute()),
		Spec,
		EvaluateParameters,
		MaxFocus
	);

	float BaseFocusRegenRate = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetBaseFocusRegenRateAttribute()),
		Spec,
		EvaluateParameters,
		BaseFocusRegenRate
	);

	float Accuracy = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetAccuracyAttribute()),
		Spec,
		EvaluateParameters,
		Accuracy
	);

	// Formule : FocusRegenRate = MaxFocus × (BaseFocusRegenRate + (Accuracy × 0.001))
	const float FocusRegenRate = MaxFocus * (BaseFocusRegenRate + (Accuracy * 0.001f));

	return FocusRegenRate;
}
