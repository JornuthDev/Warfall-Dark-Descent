// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_MaxFocus.h"
#include "Gameplay/AbilitySystem/ArchetypeAttributeSet.h"
#include "Utils/Helpers/SystemsHelpers.h"

UMMC_MaxFocus::UMMC_MaxFocus()
{
	FGameplayEffectAttributeCaptureDefinition BaseFocusDef(
		UArchetypeAttributeSet::GetBaseFocusAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	FGameplayEffectAttributeCaptureDefinition AccuracyDef(
		UArchetypeAttributeSet::GetAccuracyAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	RelevantAttributesToCapture.Add(BaseFocusDef);
	RelevantAttributesToCapture.Add(AccuracyDef);
}

float UMMC_MaxFocus::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseFocus = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetBaseFocusAttribute()),
		Spec,
		EvaluateParameters,
		BaseFocus
	);

	float Accuracy = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetAccuracyAttribute()),
		Spec,
		EvaluateParameters,
		Accuracy
	);

	// Formule : MaxFocus = BaseFocus + (Accuracy × 2.5)
	const float MaxFocus = BaseFocus + (Accuracy * 2.5f);

	return MaxFocus;
}
