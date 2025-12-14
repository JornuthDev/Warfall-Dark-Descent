// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_CriticalChance.h"
#include "Gameplay/AbilitySystem/ArchetypeAttributeSet.h"
#include "Utils/Helpers/SystemsHelpers.h"

UMMC_CriticalChance::UMMC_CriticalChance()
{
	FGameplayEffectAttributeCaptureDefinition BaseCriticalChanceDef(
		UArchetypeAttributeSet::GetBaseCriticalChanceAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	FGameplayEffectAttributeCaptureDefinition AccuracyDef(
		UArchetypeAttributeSet::GetAccuracyAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	RelevantAttributesToCapture.Add(BaseCriticalChanceDef);
	RelevantAttributesToCapture.Add(AccuracyDef);
}

float UMMC_CriticalChance::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseCriticalChance = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetBaseCriticalChanceAttribute()),
		Spec,
		EvaluateParameters,
		BaseCriticalChance
	);

	float Accuracy = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetAccuracyAttribute()),
		Spec,
		EvaluateParameters,
		Accuracy
	);

	// Formule : CriticalChance = BaseCriticalChance + (Accuracy × 0.002)
	const float CriticalChance = BaseCriticalChance + (Accuracy * 0.002f);

	return CriticalChance;
}
