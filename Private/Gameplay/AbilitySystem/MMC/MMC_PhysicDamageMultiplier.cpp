// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_PhysicDamageMultiplier.h"
#include "Gameplay/AbilitySystem/ArchetypeAttributeSet.h"
#include "Utils/Helpers/SystemsHelpers.h"

UMMC_PhysicDamageMultiplier::UMMC_PhysicDamageMultiplier()
{
	FGameplayEffectAttributeCaptureDefinition BasePhysicDamageMultiplierDef(
		UArchetypeAttributeSet::GetBasePhysicDamageMultiplierAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	FGameplayEffectAttributeCaptureDefinition StrengthDef(
		UArchetypeAttributeSet::GetStrengthAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	RelevantAttributesToCapture.Add(BasePhysicDamageMultiplierDef);
	RelevantAttributesToCapture.Add(StrengthDef);
}

float UMMC_PhysicDamageMultiplier::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BasePhysicDamageMultiplier = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetBasePhysicDamageMultiplierAttribute()),
		Spec,
		EvaluateParameters,
		BasePhysicDamageMultiplier
	);

	float Strength = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetStrengthAttribute()),
		Spec,
		EvaluateParameters,
		Strength
	);

	// Formule : PhysicDamageMultiplier = BasePhysicDamageMultiplier + (Strength × 0.005)
	const float PhysicDamageMultiplier = BasePhysicDamageMultiplier + (Strength * 0.005f);

	return PhysicDamageMultiplier;
}
