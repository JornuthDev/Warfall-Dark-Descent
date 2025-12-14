// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_MaxStamina.h"
#include "Gameplay/AbilitySystem/ArchetypeAttributeSet.h"
#include "Utils/Helpers/SystemsHelpers.h"

UMMC_MaxStamina::UMMC_MaxStamina()
{
	FGameplayEffectAttributeCaptureDefinition BaseStaminaDef(
		UArchetypeAttributeSet::GetBaseStaminaAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	FGameplayEffectAttributeCaptureDefinition StrengthDef(
		UArchetypeAttributeSet::GetStrengthAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	RelevantAttributesToCapture.Add(BaseStaminaDef);
	RelevantAttributesToCapture.Add(StrengthDef);
}

float UMMC_MaxStamina::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseStamina = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetBaseStaminaAttribute()),
		Spec,
		EvaluateParameters,
		BaseStamina
	);

	float Strength = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetStrengthAttribute()),
		Spec,
		EvaluateParameters,
		Strength
	);

	// Formule : MaxStamina = BaseStamina + (Strength × 2.5)
	const float MaxStamina = BaseStamina + (Strength * 2.5f);

	return MaxStamina;
}
