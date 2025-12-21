// Calculations/Attributes_Calculations.cpp
#include "Gameplay/AbilitySystem/Calculations/Attributes_Calculations.h"
#include "AbilitySystemComponent.h"

// Macro helper pour éviter la répétition
#define SETUP_ATTRIBUTE_CAPTURE(CaptureDef, Attribute) \
	CaptureDef.AttributeToCapture = Attribute; \
	CaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source; \
	CaptureDef.bSnapshot = false; \
	RelevantAttributesToCapture.Add(CaptureDef);

UCalculation_HealthMax::UCalculation_HealthMax()
{
	SETUP_ATTRIBUTE_CAPTURE(VitalityDef, UPlayerSet::GetVitalityAttribute());
}

UCalculation_HealthRegenRate::UCalculation_HealthRegenRate()
{
	SETUP_ATTRIBUTE_CAPTURE(VitalityDef, UPlayerSet::GetVitalityAttribute());
}

UCalculation_ManaMax::UCalculation_ManaMax()
{
	SETUP_ATTRIBUTE_CAPTURE(IntelligenceDef, UPlayerSet::GetIntelligenceAttribute());
}

UCalculation_ManaRegenRate::UCalculation_ManaRegenRate()
{
	SETUP_ATTRIBUTE_CAPTURE(MentalityDef, UPlayerSet::GetMentalityAttribute());
}

UCalculation_StaminaMax::UCalculation_StaminaMax()
{
	SETUP_ATTRIBUTE_CAPTURE(StrengthDef, UPlayerSet::GetStrengthAttribute());
}

UCalculation_StaminaRegenRate::UCalculation_StaminaRegenRate()
{
	SETUP_ATTRIBUTE_CAPTURE(StrengthDef, UPlayerSet::GetStrengthAttribute());
}

UCalculation_FocusMax::UCalculation_FocusMax()
{
	SETUP_ATTRIBUTE_CAPTURE(AccuracyDef, UPlayerSet::GetAccuracyAttribute());
}

UCalculation_FocusRegenRate::UCalculation_FocusRegenRate()
{
	SETUP_ATTRIBUTE_CAPTURE(AccuracyDef, UPlayerSet::GetAccuracyAttribute());
}

float UAttributes_Calculations::DoCalculation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	
	float CurrentPerk = 0.f;
	
	// S'assurer qu'on a bien enregistré l'attribut
	if (RelevantAttributesToCapture.Num() > 0)
	{
		GetCapturedAttributeMagnitude(RelevantAttributesToCapture[0], Spec, EvaluationParameters, CurrentPerk);
	}
	else
	{
		// Fallback si pas d'attribut capturé
		return 0.f;
	}
	
	CurrentPerk = FMath::Floor(CurrentPerk);
	
	if (CurrentPerk == 0.f)
	{
		return 0.f;
	}
	
	const UAbilitySystemComponent* SourceASC = Spec.GetContext().GetInstigatorAbilitySystemComponent();
	if (!SourceASC)
	{
		return 0.f;
	}
	
	const float AttributeMaxBase = SourceASC->GetNumericAttributeBase(GetAttributeToBaseValue());
	const float DesiredValue = AttributeMaxBase + (CurrentPerk * GetMultiplier());
	
	return DesiredValue - AttributeMaxBase;
}