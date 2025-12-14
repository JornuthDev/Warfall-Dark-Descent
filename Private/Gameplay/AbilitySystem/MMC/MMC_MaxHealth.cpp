// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_MaxHealth.h"
#include "Gameplay/AbilitySystem/ArchetypeAttributeSet.h"
#include "Utils/Helpers/SystemsHelpers.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	// Définir la capture pour BaseHealth
	FGameplayEffectAttributeCaptureDefinition BaseHealthDef(
		UArchetypeAttributeSet::GetBaseHealthAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	// Définir la capture pour Vitality
	FGameplayEffectAttributeCaptureDefinition VitalityDef(
		UArchetypeAttributeSet::GetVitalityAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	// Ajouter les captures
	RelevantAttributesToCapture.Add(BaseHealthDef);
	RelevantAttributesToCapture.Add(VitalityDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Récupérer les tags du Spec pour les vérifications
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	// Capturer BaseHealth
	float BaseHealth = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetBaseHealthAttribute()),
		Spec,
		EvaluateParameters,
		BaseHealth
	);

	// Capturer Vitality
	float Vitality = 0.f;
	GetCapturedAttributeMagnitude(
		GetCaptureDefinition(UArchetypeAttributeSet::GetVitalityAttribute()),
		Spec,
		EvaluateParameters,
		Vitality
	);

	// Formule : MaxHealth = BaseHealth + (Vitality × 5)
	const float MaxHealth = BaseHealth + (Vitality * 5.0f);

	return MaxHealth;
}
