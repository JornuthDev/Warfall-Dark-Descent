// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_AccuracyDamageMultiplier.h"
#include "Gameplay/AbilitySystem/DefaultAttributeSet.h"
#include "GameplayEffectExecutionCalculation.h"
#include "Gameplay/AbilitySystem/AdvancedAttributeSet.h"

struct FAccuracyDamageMultiplierStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseAccuracyDamageMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Accuracy);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AccuracyDamageBonus);

	FAccuracyDamageMultiplierStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, BaseAccuracyDamageMultiplier, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAdvancedAttributeSet, Accuracy, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, AccuracyDamageBonus, Target, false);
	}
};

static const FAccuracyDamageMultiplierStatics& AccuracyDamageMultiplierStatics()
{
	static FAccuracyDamageMultiplierStatics Statics;
	return Statics;
}

UMMC_AccuracyDamageMultiplier::UMMC_AccuracyDamageMultiplier()
{
	RelevantAttributesToCapture.Add(AccuracyDamageMultiplierStatics().BaseAccuracyDamageMultiplierDef);
	RelevantAttributesToCapture.Add(AccuracyDamageMultiplierStatics().AccuracyDef);
	RelevantAttributesToCapture.Add(AccuracyDamageMultiplierStatics().AccuracyDamageBonusDef);
}

float UMMC_AccuracyDamageMultiplier::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseAccuracyDamageMultiplier = 0.f;
	GetCapturedAttributeMagnitude(AccuracyDamageMultiplierStatics().BaseAccuracyDamageMultiplierDef, Spec, EvaluateParameters, BaseAccuracyDamageMultiplier);

	float Accuracy = 0.f;
	GetCapturedAttributeMagnitude(AccuracyDamageMultiplierStatics().AccuracyDef, Spec, EvaluateParameters, Accuracy);

	float AccuracyDamageBonus = 0.f;
	GetCapturedAttributeMagnitude(AccuracyDamageMultiplierStatics().AccuracyDamageBonusDef, Spec, EvaluateParameters, AccuracyDamageBonus);

	if (Spec.GetLevel() == 1)
	{
		return FMath::Max((BaseAccuracyDamageMultiplier + AccuracyDamageBonus), 0.1f);
	}
	
	// Formule : AccuracyDamageMultiplier = BaseAccuracyDamageMultiplier + (Accuracy × 0.005) + AccuracyDamageBonus (équipements/buffs)
	return FMath::Max((BaseAccuracyDamageMultiplier + (Accuracy * 0.005f)) + AccuracyDamageBonus, 0.1f);
}
