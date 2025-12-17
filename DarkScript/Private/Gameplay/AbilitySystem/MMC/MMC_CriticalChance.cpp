// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_CriticalChance.h"
#include "Gameplay/AbilitySystem/DefaultAttributeSet.h"
#include "GameplayEffectExecutionCalculation.h"
#include "Gameplay/AbilitySystem/AdvancedAttributeSet.h"

struct FCriticalChanceStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseCriticalChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Accuracy);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalChanceBonus);

	FCriticalChanceStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, BaseCriticalChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAdvancedAttributeSet, Accuracy, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, CriticalChanceBonus, Target, false);
	}
};

static const FCriticalChanceStatics& CriticalChanceStatics()
{
	static FCriticalChanceStatics Statics;
	return Statics;
}

UMMC_CriticalChance::UMMC_CriticalChance()
{
	RelevantAttributesToCapture.Add(CriticalChanceStatics().BaseCriticalChanceDef);
	RelevantAttributesToCapture.Add(CriticalChanceStatics().AccuracyDef);
	RelevantAttributesToCapture.Add(CriticalChanceStatics().CriticalChanceBonusDef);
}

float UMMC_CriticalChance::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseCriticalChance = 0.f;
	GetCapturedAttributeMagnitude(CriticalChanceStatics().BaseCriticalChanceDef, Spec, EvaluateParameters, BaseCriticalChance);

	float Accuracy = 0.f;
	GetCapturedAttributeMagnitude(CriticalChanceStatics().AccuracyDef, Spec, EvaluateParameters, Accuracy);

	float CriticalChanceBonus = 0.f;
	GetCapturedAttributeMagnitude(CriticalChanceStatics().CriticalChanceBonusDef, Spec, EvaluateParameters, CriticalChanceBonus);

	if (Spec.GetLevel() == 1)
	{
		return FMath::Max((BaseCriticalChance + CriticalChanceBonus), 0.0f);
	}
	
	// Formule : CriticalChance = BaseCriticalChance + (Accuracy × 0.002) + CriticalChanceBonus (équipements/buffs)
	return FMath::Max((BaseCriticalChance + (Accuracy * 0.002f)) + CriticalChanceBonus, 0.0f);
}
