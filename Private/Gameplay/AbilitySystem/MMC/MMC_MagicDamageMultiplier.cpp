// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_MagicDamageMultiplier.h"
#include "Gameplay/AbilitySystem/ArchetypeAttributeSet.h"
#include "Utils/Helpers/SystemsHelpers.h"

UMMC_MagicDamageMultiplier::UMMC_MagicDamageMultiplier()
{
	FGameplayEffectAttributeCaptureDefinition BaseMagicDamageMultiplierDef(
		UArchetypeAttributeSet::GetBaseMagicDamageMultiplierAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	FGameplayEffectAttributeCaptureDefinition IntelligenceDef(
		UArchetypeAttributeSet::GetIntelligenceAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	RelevantAttributesToCapture.Add(BaseMagicDamageMultiplierDef);
	RelevantAttributesToCapture.Add(IntelligenceDef);
}

float UMMC_MagicDamageMultiplier::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseMagicDamageMultiplier = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetBaseMagicDamageMultiplierAttribute()),
		Spec,
		EvaluateParameters,
		BaseMagicDamageMultiplier
	);

	float Intelligence = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetIntelligenceAttribute()),
		Spec,
		EvaluateParameters,
		Intelligence
	);

	// Formule : MagicDamageMultiplier = BaseMagicDamageMultiplier + (Intelligence × 0.005)
	const float MagicDamageMultiplier = BaseMagicDamageMultiplier + (Intelligence * 0.005f);

	return MagicDamageMultiplier;
}
