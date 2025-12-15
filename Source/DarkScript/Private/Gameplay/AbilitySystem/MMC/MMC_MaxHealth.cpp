// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_MaxHealth.h"
#include "Gameplay/AbilitySystem/DefaultAttributeSet.h"
#include "GameplayEffectExecutionCalculation.h"
#include "Gameplay/AbilitySystem/AdvancedAttributeSet.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Capture Definitions (Pattern GAS standard) ───────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

struct FMaxHealthStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseHealth);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Vitality);
	DECLARE_ATTRIBUTE_CAPTUREDEF(HealthBonus);

	FMaxHealthStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, BaseHealth, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAdvancedAttributeSet, Vitality, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, HealthBonus, Target, false);
	}
};

static const FMaxHealthStatics& MaxHealthStatics()
{
	static FMaxHealthStatics Statics;
	return Statics;
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Constructeur ──────────────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

UMMC_MaxHealth::UMMC_MaxHealth()
{
	RelevantAttributesToCapture.Add(MaxHealthStatics().BaseHealthDef);
	RelevantAttributesToCapture.Add(MaxHealthStatics().VitalityDef);
	RelevantAttributesToCapture.Add(MaxHealthStatics().HealthBonusDef);
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Calcul ────────────────────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseHealth = 0.f;
	GetCapturedAttributeMagnitude(MaxHealthStatics().BaseHealthDef, Spec, EvaluateParameters, BaseHealth);

	float Vitality = 0.f;
	GetCapturedAttributeMagnitude(MaxHealthStatics().VitalityDef, Spec, EvaluateParameters, Vitality);

	float HealthBonus = 0.f;
	GetCapturedAttributeMagnitude(MaxHealthStatics().HealthBonusDef, Spec, EvaluateParameters, HealthBonus);

	if (Spec.GetLevel() == 1)
	{
		return FMath::Max(BaseHealth + HealthBonus, 1);
	}
	
	// Formule : MaxHealth = BaseHealth + (Vitality × 5) + HealthBonus (équipements/buffs)
	return FMath::Max((BaseHealth + (Vitality * 5.0f)) + HealthBonus, 1);
}
