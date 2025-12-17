// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_MagicDamageMultiplier.h"
#include "Gameplay/AbilitySystem/DefaultAttributeSet.h"
#include "GameplayEffectExecutionCalculation.h"
#include "Gameplay/AbilitySystem/AdvancedAttributeSet.h"

struct FMagicDamageMultiplierStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseMagicDamageMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Intelligence);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicDamageBonus);

	FMagicDamageMultiplierStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, BaseMagicDamageMultiplier, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAdvancedAttributeSet, Intelligence, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, MagicDamageBonus, Target, false);
	}
};

static const FMagicDamageMultiplierStatics& MagicDamageMultiplierStatics()
{
	static FMagicDamageMultiplierStatics Statics;
	return Statics;
}

UMMC_MagicDamageMultiplier::UMMC_MagicDamageMultiplier()
{
	RelevantAttributesToCapture.Add(MagicDamageMultiplierStatics().BaseMagicDamageMultiplierDef);
	RelevantAttributesToCapture.Add(MagicDamageMultiplierStatics().IntelligenceDef);
	RelevantAttributesToCapture.Add(MagicDamageMultiplierStatics().MagicDamageBonusDef);
}

float UMMC_MagicDamageMultiplier::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseMagicDamageMultiplier = 0.f;
	GetCapturedAttributeMagnitude(MagicDamageMultiplierStatics().BaseMagicDamageMultiplierDef, Spec, EvaluateParameters, BaseMagicDamageMultiplier);

	float Intelligence = 0.f;
	GetCapturedAttributeMagnitude(MagicDamageMultiplierStatics().IntelligenceDef, Spec, EvaluateParameters, Intelligence);

	float MagicDamageBonus = 0.f;
	GetCapturedAttributeMagnitude(MagicDamageMultiplierStatics().MagicDamageBonusDef, Spec, EvaluateParameters, MagicDamageBonus);

	if (Spec.GetLevel() == 1)
	{
		return FMath::Max(BaseMagicDamageMultiplier + MagicDamageBonus, 0.1f);
	}
	
	// Formule : MagicDamageMultiplier = BaseMagicDamageMultiplier + (Intelligence × 0.005) + MagicDamageBonus (équipements/buffs)
	return FMath::Max((BaseMagicDamageMultiplier + (Intelligence * 0.005f)) + MagicDamageBonus, 0.1f);
}
