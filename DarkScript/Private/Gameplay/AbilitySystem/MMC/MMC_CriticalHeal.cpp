// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_CriticalHeal.h"
#include "Gameplay/AbilitySystem/DefaultAttributeSet.h"
#include "GameplayEffectExecutionCalculation.h"
#include "Gameplay/AbilitySystem/AdvancedAttributeSet.h"

struct FCriticalHealStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseCriticalHeal);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Mentality);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHealBonus);

	FCriticalHealStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, BaseCriticalHeal, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAdvancedAttributeSet, Mentality, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, CriticalHealBonus, Target, false);
	}
};

static const FCriticalHealStatics& CriticalHealStatics()
{
	static FCriticalHealStatics Statics;
	return Statics;
}

UMMC_CriticalHeal::UMMC_CriticalHeal()
{
	RelevantAttributesToCapture.Add(CriticalHealStatics().BaseCriticalHealDef);
	RelevantAttributesToCapture.Add(CriticalHealStatics().MentalityDef);
	RelevantAttributesToCapture.Add(CriticalHealStatics().CriticalHealBonusDef);
}

float UMMC_CriticalHeal::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseCriticalHeal = 0.f;
	GetCapturedAttributeMagnitude(CriticalHealStatics().BaseCriticalHealDef, Spec, EvaluateParameters, BaseCriticalHeal);

	float Mentality = 0.f;
	GetCapturedAttributeMagnitude(CriticalHealStatics().MentalityDef, Spec, EvaluateParameters, Mentality);

	float CriticalHealBonus = 0.f;
	GetCapturedAttributeMagnitude(CriticalHealStatics().CriticalHealBonusDef, Spec, EvaluateParameters, CriticalHealBonus);

	if (Spec.GetLevel() == 1)
	{
		return FMath::Max(BaseCriticalHeal + CriticalHealBonus, 0.0f);
	}
	
	// Formule : CriticalHeal = BaseCriticalHeal + (Mentality × 0.002) + CriticalHealBonus (équipements/buffs)
	return FMath::Max((BaseCriticalHeal + (Mentality * 0.002f)) + CriticalHealBonus, 0.0f);
}
