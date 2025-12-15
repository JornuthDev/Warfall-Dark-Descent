// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_FocusRegenRate.h"
#include "Gameplay/AbilitySystem/DefaultAttributeSet.h"
#include "GameplayEffectExecutionCalculation.h"
#include "Gameplay/AbilitySystem/AdvancedAttributeSet.h"

struct FFocusRegenRateStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxFocus);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseFocusRegenRate);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Accuracy);
	DECLARE_ATTRIBUTE_CAPTUREDEF(FocusRegenBonus);

	FFocusRegenRateStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, MaxFocus, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, BaseFocusRegenRate, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAdvancedAttributeSet, Accuracy, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, FocusRegenBonus, Target, false);
	}
};

static const FFocusRegenRateStatics& FocusRegenRateStatics()
{
	static FFocusRegenRateStatics Statics;
	return Statics;
}

UMMC_FocusRegenRate::UMMC_FocusRegenRate()
{
	RelevantAttributesToCapture.Add(FocusRegenRateStatics().MaxFocusDef);
	RelevantAttributesToCapture.Add(FocusRegenRateStatics().BaseFocusRegenRateDef);
	RelevantAttributesToCapture.Add(FocusRegenRateStatics().AccuracyDef);
	RelevantAttributesToCapture.Add(FocusRegenRateStatics().FocusRegenBonusDef);
}

float UMMC_FocusRegenRate::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float MaxFocus = 0.f;
	GetCapturedAttributeMagnitude(FocusRegenRateStatics().MaxFocusDef, Spec, EvaluateParameters, MaxFocus);

	float BaseFocusRegenRate = 0.f;
	GetCapturedAttributeMagnitude(FocusRegenRateStatics().BaseFocusRegenRateDef, Spec, EvaluateParameters, BaseFocusRegenRate);

	float Accuracy = 0.f;
	GetCapturedAttributeMagnitude(FocusRegenRateStatics().AccuracyDef, Spec, EvaluateParameters, Accuracy);

	float FocusRegenBonus = 0.f;
	GetCapturedAttributeMagnitude(FocusRegenRateStatics().FocusRegenBonusDef, Spec, EvaluateParameters, FocusRegenBonus);

	if (Spec.GetLevel() == 1)
	{
		return (MaxFocus * BaseFocusRegenRate) + FocusRegenBonus;
	}
	
	// Formule : FocusRegenRate = MaxFocus × (BaseFocusRegenRate + Accuracy × 0.001) + FocusRegenBonus (équipements/buffs)
	return (MaxFocus * (BaseFocusRegenRate + (Accuracy * 0.001f))) + FocusRegenBonus;
}
