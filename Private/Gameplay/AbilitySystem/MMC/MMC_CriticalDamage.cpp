// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_CriticalDamage.h"
#include "Gameplay/AbilitySystem/ArchetypeAttributeSet.h"
#include "Utils/Helpers/SystemsHelpers.h"

UMMC_CriticalDamage::UMMC_CriticalDamage()
{
	FGameplayEffectAttributeCaptureDefinition BaseCriticalDamageDef(
		UArchetypeAttributeSet::GetBaseCriticalDamageAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	FGameplayEffectAttributeCaptureDefinition AccuracyDef(
		UArchetypeAttributeSet::GetAccuracyAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	RelevantAttributesToCapture.Add(BaseCriticalDamageDef);
	RelevantAttributesToCapture.Add(AccuracyDef);
}

float UMMC_CriticalDamage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseCriticalDamage = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetBaseCriticalDamageAttribute()),
		Spec,
		EvaluateParameters,
		BaseCriticalDamage
	);

	float Accuracy = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetAccuracyAttribute()),
		Spec,
		EvaluateParameters,
		Accuracy
	);

	// Formule : CriticalDamage = BaseCriticalDamage + (Accuracy × 0.002)
	const float CriticalDamage = BaseCriticalDamage + (Accuracy * 0.002f);

	return CriticalDamage;
}
