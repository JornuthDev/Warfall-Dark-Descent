// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_MaxStamina.h"
#include "Gameplay/AbilitySystem/DefaultAttributeSet.h"
#include "GameplayEffectExecutionCalculation.h"
#include "Gameplay/AbilitySystem/AdvancedAttributeSet.h"

struct FMaxStaminaStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseStamina);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Strength);
	DECLARE_ATTRIBUTE_CAPTUREDEF(StaminaBonus);

	FMaxStaminaStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, BaseStamina, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAdvancedAttributeSet, Strength, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, StaminaBonus, Target, false);
	}
};

static const FMaxStaminaStatics& MaxStaminaStatics()
{
	static FMaxStaminaStatics Statics;
	return Statics;
}

UMMC_MaxStamina::UMMC_MaxStamina()
{
	RelevantAttributesToCapture.Add(MaxStaminaStatics().BaseStaminaDef);
	RelevantAttributesToCapture.Add(MaxStaminaStatics().StrengthDef);
	RelevantAttributesToCapture.Add(MaxStaminaStatics().StaminaBonusDef);
}

float UMMC_MaxStamina::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseStamina = 0.f;
	GetCapturedAttributeMagnitude(MaxStaminaStatics().BaseStaminaDef, Spec, EvaluateParameters, BaseStamina);

	float Strength = 0.f;
	GetCapturedAttributeMagnitude(MaxStaminaStatics().StrengthDef, Spec, EvaluateParameters, Strength);

	float StaminaBonus = 0.f;
	GetCapturedAttributeMagnitude(MaxStaminaStatics().StaminaBonusDef, Spec, EvaluateParameters, StaminaBonus);

	if (Spec.GetLevel() == 1)
	{
		return FMath::Max(BaseStamina + StaminaBonus, 0.0f);
	}
	
	// Formule : MaxStamina = BaseStamina + (Strength × 2.5) + StaminaBonus (équipements/buffs)
	return FMath::Max((BaseStamina + (Strength * 2.5f)) + StaminaBonus, 0.0f);
}
