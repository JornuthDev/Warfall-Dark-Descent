// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_AccuracyDamageMultiplier.h"
#include "Gameplay/AbilitySystem/ArchetypeAttributeSet.h"
#include "Utils/Helpers/SystemsHelpers.h"

UMMC_AccuracyDamageMultiplier::UMMC_AccuracyDamageMultiplier()
{
	FGameplayEffectAttributeCaptureDefinition BaseAccuracyDamageMultiplierDef(
		UArchetypeAttributeSet::GetBaseAccuracyDamageMultiplierAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	FGameplayEffectAttributeCaptureDefinition AccuracyDef(
		UArchetypeAttributeSet::GetAccuracyAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	RelevantAttributesToCapture.Add(BaseAccuracyDamageMultiplierDef);
	RelevantAttributesToCapture.Add(AccuracyDef);
}

float UMMC_AccuracyDamageMultiplier::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseAccuracyDamageMultiplier = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetBaseAccuracyDamageMultiplierAttribute()),
		Spec,
		EvaluateParameters,
		BaseAccuracyDamageMultiplier
	);

	float Accuracy = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetAccuracyAttribute()),
		Spec,
		EvaluateParameters,
		Accuracy
	);

	// Formule : AccuracyDamageMultiplier = BaseAccuracyDamageMultiplier + (Accuracy × 0.005)
	const float AccuracyDamageMultiplier = BaseAccuracyDamageMultiplier + (Accuracy * 0.005f);

	return AccuracyDamageMultiplier;
}
