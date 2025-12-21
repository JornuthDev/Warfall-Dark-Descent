// Calculations/Attributes_Calculations.cpp
#include "Gameplay/AbilitySystem/Calculations/Attributes_Calculations.h"
#include "AbilitySystemComponent.h"

void UAttributes_Calculations::PostInitProperties()
{
	Super::PostInitProperties();
	
	// ✅ Virtual functions work correctly here!
	AttributeToBaseValue = GetAttributeToBaseValue();
	MultiplyValue = GetMultiplier();
}

float UAttributes_Calculations::DoCalculation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	
	float CurrentPerk = 0.f;
	GetCapturedAttributeMagnitude(AttributeDef, Spec, EvaluationParameters, CurrentPerk);
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
	
	const float AttributeMaxBase = SourceASC->GetNumericAttributeBase(AttributeToBaseValue);
	const float DesiredValue = AttributeMaxBase + (CurrentPerk * MultiplyValue);
	
	return DesiredValue - AttributeMaxBase;
}