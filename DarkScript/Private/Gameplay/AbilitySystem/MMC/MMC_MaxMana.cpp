// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_MaxMana.h"
#include "Gameplay/AbilitySystem/DefaultAttributeSet.h"
#include "GameplayEffectExecutionCalculation.h"
#include "Gameplay/AbilitySystem/AdvancedAttributeSet.h"

struct FMaxManaStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseMana);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Intelligence);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ManaBonus);

	FMaxManaStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, BaseMana, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAdvancedAttributeSet, Intelligence, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, ManaBonus, Target, false);
	}
};

static const FMaxManaStatics& MaxManaStatics()
{
	static FMaxManaStatics Statics;
	return Statics;
}

UMMC_MaxMana::UMMC_MaxMana()
{
	RelevantAttributesToCapture.Add(MaxManaStatics().BaseManaDef);
	RelevantAttributesToCapture.Add(MaxManaStatics().IntelligenceDef);
	RelevantAttributesToCapture.Add(MaxManaStatics().ManaBonusDef);
}

float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseMana = 0.f;
	GetCapturedAttributeMagnitude(MaxManaStatics().BaseManaDef, Spec, EvaluateParameters, BaseMana);

	float Intelligence = 0.f;
	GetCapturedAttributeMagnitude(MaxManaStatics().IntelligenceDef, Spec, EvaluateParameters, Intelligence);

	float ManaBonus = 0.f;
	GetCapturedAttributeMagnitude(MaxManaStatics().ManaBonusDef, Spec, EvaluateParameters, ManaBonus);

	if (Spec.GetLevel() == 1)
	{
		return FMath::Max(BaseMana + ManaBonus, 0.0f);
	}
	
	// Formule : MaxMana = BaseMana + (Intelligence × 2.5) + ManaBonus (équipements/buffs)
	return FMath::Max((BaseMana + (Intelligence * 2.5f)) + ManaBonus, 0.0f);
}
