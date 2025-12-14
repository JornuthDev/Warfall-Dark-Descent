// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_MaxMana.h"
#include "Gameplay/AbilitySystem/ArchetypeAttributeSet.h"
#include "Utils/Helpers/SystemsHelpers.h"

UMMC_MaxMana::UMMC_MaxMana()
{
	FGameplayEffectAttributeCaptureDefinition BaseManaDef(
		UArchetypeAttributeSet::GetBaseManaAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	FGameplayEffectAttributeCaptureDefinition IntelligenceDef(
		UArchetypeAttributeSet::GetIntelligenceAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	RelevantAttributesToCapture.Add(BaseManaDef);
	RelevantAttributesToCapture.Add(IntelligenceDef);
}

float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseMana = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetBaseManaAttribute()),
		Spec,
		EvaluateParameters,
		BaseMana
	);

	float Intelligence = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetIntelligenceAttribute()),
		Spec,
		EvaluateParameters,
		Intelligence
	);

	// Formule : MaxMana = BaseMana + (Intelligence × 2.5)
	const float MaxMana = BaseMana + (Intelligence * 2.5f);

	return MaxMana;
}
