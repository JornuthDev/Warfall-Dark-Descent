// Calculations/Perks_Calculations.cpp
#include "Gameplay/AbilitySystem/Calculations/Perks_Calculations.h"
#include "AbilitySystemComponent.h"

UPerks_Calculations::UPerks_Calculations()
{
	LevelDef.AttributeToCapture = UPlayerSet::GetLevelAttribute();
	LevelDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	LevelDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(LevelDef);
}

float UPerks_Calculations::DoCalculation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	
	float CurrentLevel = 0.f;
	
	// S'assurer qu'on a bien enregistré l'attribut
	if (RelevantAttributesToCapture.Num() > 0)
	{
		GetCapturedAttributeMagnitude(RelevantAttributesToCapture[0], Spec, EvaluationParameters, CurrentLevel);
	}
	else
	{
		// Fallback si pas d'attribut capturé
		return 0.f;
	}
	
	if (CurrentLevel <= 0.f)
	{
		return 0.f;
	}
	
	const UAbilitySystemComponent* SourceASC = Spec.GetContext().GetInstigatorAbilitySystemComponent();
	if (!SourceASC)
	{
		return 0.f;
	}
	
	const float PerkPerLevel = SourceASC->GetNumericAttributeBase(GetAttribute());
	const float DesiredValue = FMath::Floor(CurrentLevel * PerkPerLevel);
	return DesiredValue - PerkPerLevel;
}