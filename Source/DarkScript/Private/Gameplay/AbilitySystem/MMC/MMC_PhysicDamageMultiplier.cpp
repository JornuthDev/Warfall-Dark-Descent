// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/MMC/MMC_PhysicDamageMultiplier.h"
#include "Gameplay/AbilitySystem/DefaultAttributeSet.h"
#include "GameplayEffectExecutionCalculation.h"
#include "Gameplay/AbilitySystem/AdvancedAttributeSet.h"

struct FPhysicDamageMultiplierStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(BasePhysicDamageMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Strength);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicDamageBonus);

	FPhysicDamageMultiplierStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, BasePhysicDamageMultiplier, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAdvancedAttributeSet, Strength, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDefaultAttributeSet, PhysicDamageBonus, Target, false);
	}
};

static const FPhysicDamageMultiplierStatics& PhysicDamageMultiplierStatics()
{
	static FPhysicDamageMultiplierStatics Statics;
	return Statics;
}

UMMC_PhysicDamageMultiplier::UMMC_PhysicDamageMultiplier()
{
	RelevantAttributesToCapture.Add(PhysicDamageMultiplierStatics().BasePhysicDamageMultiplierDef);
	RelevantAttributesToCapture.Add(PhysicDamageMultiplierStatics().StrengthDef);
	RelevantAttributesToCapture.Add(PhysicDamageMultiplierStatics().PhysicDamageBonusDef);
}

float UMMC_PhysicDamageMultiplier::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BasePhysicDamageMultiplier = 0.f;
	GetCapturedAttributeMagnitude(PhysicDamageMultiplierStatics().BasePhysicDamageMultiplierDef, Spec, EvaluateParameters, BasePhysicDamageMultiplier);

	float Strength = 0.f;
	GetCapturedAttributeMagnitude(PhysicDamageMultiplierStatics().StrengthDef, Spec, EvaluateParameters, Strength);

	float PhysicDamageBonus = 0.f;
	GetCapturedAttributeMagnitude(PhysicDamageMultiplierStatics().PhysicDamageBonusDef, Spec, EvaluateParameters, PhysicDamageBonus);

	if (Spec.GetLevel() == 1)
	{
		return FMath::Max(BasePhysicDamageMultiplier + PhysicDamageBonus, 0.1f);
	}
	
	// Formule : PhysicDamageMultiplier = BasePhysicDamageMultiplier + (Strength × 0.005) + PhysicDamageBonus (équipements/buffs)
	return FMath::Max((BasePhysicDamageMultiplier + (Strength * 0.005f)) + PhysicDamageBonus, 0.1f);
}
