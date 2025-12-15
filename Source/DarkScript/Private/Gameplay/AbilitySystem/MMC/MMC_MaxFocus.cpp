// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_MaxFocus.h"
#include "Gameplay/AbilitySystem/DefaultAttributeSet.h"
#include "GameplayEffectExecutionCalculation.h"
#include "Gameplay/AbilitySystem/AdvancedAttributeSet.h"

struct FMaxFocusStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseFocus);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Accuracy);
	DECLARE_ATTRIBUTE_CAPTUREDEF(FocusBonus);

	FMaxFocusStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, BaseFocus, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAdvancedAttributeSet, Accuracy, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, FocusBonus, Target, false);
	}
};

static const FMaxFocusStatics& MaxFocusStatics()
{
	static FMaxFocusStatics Statics;
	return Statics;
}

UMMC_MaxFocus::UMMC_MaxFocus()
{
	RelevantAttributesToCapture.Add(MaxFocusStatics().BaseFocusDef);
	RelevantAttributesToCapture.Add(MaxFocusStatics().AccuracyDef);
	RelevantAttributesToCapture.Add(MaxFocusStatics().FocusBonusDef);
}

float UMMC_MaxFocus::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseFocus = 0.f;
	GetCapturedAttributeMagnitude(MaxFocusStatics().BaseFocusDef, Spec, EvaluateParameters, BaseFocus);

	float Accuracy = 0.f;
	GetCapturedAttributeMagnitude(MaxFocusStatics().AccuracyDef, Spec, EvaluateParameters, Accuracy);

	float FocusBonus = 0.f;
	GetCapturedAttributeMagnitude(MaxFocusStatics().FocusBonusDef, Spec, EvaluateParameters, FocusBonus);

	if (Spec.GetLevel() == 1)
	{
		return FMath::Max(BaseFocus + FocusBonus, 0.0f);
	}
	
	// Formule : MaxFocus = BaseFocus + (Accuracy × 2.5) + FocusBonus (équipements/buffs)
	return FMath::Max((BaseFocus + (Accuracy * 2.5f)) + FocusBonus, 0.0f);
}
