// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_ManaRegenRate.h"
#include "Gameplay/AbilitySystem/DefaultAttributeSet.h"
#include "GameplayEffectExecutionCalculation.h"
#include "Gameplay/AbilitySystem/AdvancedAttributeSet.h"

struct FManaRegenRateStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxMana);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseManaRegenRate);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Mentality);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ManaRegenBonus);

	FManaRegenRateStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, MaxMana, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, BaseManaRegenRate, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAdvancedAttributeSet, Mentality, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, ManaRegenBonus, Target, false);
	}
};

static const FManaRegenRateStatics& ManaRegenRateStatics()
{
	static FManaRegenRateStatics Statics;
	return Statics;
}

UMMC_ManaRegenRate::UMMC_ManaRegenRate()
{
	RelevantAttributesToCapture.Add(ManaRegenRateStatics().MaxManaDef);
	RelevantAttributesToCapture.Add(ManaRegenRateStatics().BaseManaRegenRateDef);
	RelevantAttributesToCapture.Add(ManaRegenRateStatics().MentalityDef);
	RelevantAttributesToCapture.Add(ManaRegenRateStatics().ManaRegenBonusDef);
}

float UMMC_ManaRegenRate::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float MaxMana = 0.f;
	GetCapturedAttributeMagnitude(ManaRegenRateStatics().MaxManaDef, Spec, EvaluateParameters, MaxMana);

	float BaseManaRegenRate = 0.f;
	GetCapturedAttributeMagnitude(ManaRegenRateStatics().BaseManaRegenRateDef, Spec, EvaluateParameters, BaseManaRegenRate);

	float Mentality = 0.f;
	GetCapturedAttributeMagnitude(ManaRegenRateStatics().MentalityDef, Spec, EvaluateParameters, Mentality);

	float ManaRegenBonus = 0.f;
	GetCapturedAttributeMagnitude(ManaRegenRateStatics().ManaRegenBonusDef, Spec, EvaluateParameters, ManaRegenBonus);

	if (Spec.GetLevel() == 1)
	{
		return (MaxMana * BaseManaRegenRate) + ManaRegenBonus;
	}
	
	// Formule : ManaRegenRate = MaxMana × (BaseManaRegenRate + Mentality × 0.001) + ManaRegenBonus (équipements/buffs)
	return (MaxMana * (BaseManaRegenRate + (Mentality * 0.001f))) + ManaRegenBonus;
}
