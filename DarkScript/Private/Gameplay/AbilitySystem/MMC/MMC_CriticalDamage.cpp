// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_CriticalDamage.h"
#include "Gameplay/AbilitySystem/DefaultAttributeSet.h"
#include "GameplayEffectExecutionCalculation.h"
#include "Gameplay/AbilitySystem/AdvancedAttributeSet.h"

struct FCriticalDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseCriticalDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Accuracy);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalDamageBonus);

	FCriticalDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, BaseCriticalDamage, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAdvancedAttributeSet, Accuracy, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, CriticalDamageBonus, Target, false);
	}
};

static const FCriticalDamageStatics& CriticalDamageStatics()
{
	static FCriticalDamageStatics Statics;
	return Statics;
}

UMMC_CriticalDamage::UMMC_CriticalDamage()
{
	RelevantAttributesToCapture.Add(CriticalDamageStatics().BaseCriticalDamageDef);
	RelevantAttributesToCapture.Add(CriticalDamageStatics().AccuracyDef);
	RelevantAttributesToCapture.Add(CriticalDamageStatics().CriticalDamageBonusDef);
}

float UMMC_CriticalDamage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseCriticalDamage = 0.f;
	GetCapturedAttributeMagnitude(CriticalDamageStatics().BaseCriticalDamageDef, Spec, EvaluateParameters, BaseCriticalDamage);

	float Accuracy = 0.f;
	GetCapturedAttributeMagnitude(CriticalDamageStatics().AccuracyDef, Spec, EvaluateParameters, Accuracy);

	float CriticalDamageBonus = 0.f;
	GetCapturedAttributeMagnitude(CriticalDamageStatics().CriticalDamageBonusDef, Spec, EvaluateParameters, CriticalDamageBonus);

	if (Spec.GetLevel() == 1)
	{
		return FMath::Max(BaseCriticalDamage + CriticalDamageBonus, 0.0f);
	}
	
	// Formule : CriticalDamage = BaseCriticalDamage + (Accuracy × 0.002) + CriticalDamageBonus (équipements/buffs)
	return FMath::Max((BaseCriticalDamage + (Accuracy * 0.002f)) + CriticalDamageBonus, 0.0f);
}
