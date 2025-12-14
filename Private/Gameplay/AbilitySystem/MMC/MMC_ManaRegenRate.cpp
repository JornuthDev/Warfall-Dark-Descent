// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_ManaRegenRate.h"
#include "Gameplay/AbilitySystem/ArchetypeAttributeSet.h"
#include "Utils/Helpers/SystemsHelpers.h"

UMMC_ManaRegenRate::UMMC_ManaRegenRate()
{
	FGameplayEffectAttributeCaptureDefinition MaxManaDef(
		UArchetypeAttributeSet::GetMaxManaAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	FGameplayEffectAttributeCaptureDefinition BaseManaRegenRateDef(
		UArchetypeAttributeSet::GetBaseManaRegenRateAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	FGameplayEffectAttributeCaptureDefinition MentalityDef(
		UArchetypeAttributeSet::GetMentalityAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	RelevantAttributesToCapture.Add(MaxManaDef);
	RelevantAttributesToCapture.Add(BaseManaRegenRateDef);
	RelevantAttributesToCapture.Add(MentalityDef);
}

float UMMC_ManaRegenRate::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float MaxMana = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetMaxManaAttribute()),
		Spec,
		EvaluateParameters,
		MaxMana
	);

	float BaseManaRegenRate = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetBaseManaRegenRateAttribute()),
		Spec,
		EvaluateParameters,
		BaseManaRegenRate
	);

	float Mentality = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetMentalityAttribute()),
		Spec,
		EvaluateParameters,
		Mentality
	);

	// Formule : ManaRegenRate = MaxMana × (BaseManaRegenRate + (Mentality × 0.001))
	const float ManaRegenRate = MaxMana * (BaseManaRegenRate + (Mentality * 0.001f));

	return ManaRegenRate;
}
