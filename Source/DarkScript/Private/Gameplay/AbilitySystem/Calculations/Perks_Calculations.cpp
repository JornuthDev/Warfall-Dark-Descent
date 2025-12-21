
#include "Gameplay/AbilitySystem/Calculations/Perks_Calculations.h"

#include "AbilitySystemComponent.h"

void UPerks_Calculations::Init() const
{
	Super::Init();
	
	const_cast<UPerks_Calculations*>(this)->Attribute = GetAttribute();
}

float UPerks_Calculations::DoCalculation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	
	float CurrentLevel = 0.f;
	GetCapturedAttributeMagnitude(AttributeDef, Spec, EvaluationParameters, CurrentLevel);
	
	if (CurrentLevel <= 0.f)
	{
		return 0.f;
	}
	
	const UAbilitySystemComponent* SourceASC = Spec.GetContext().GetInstigatorAbilitySystemComponent();
	if (!SourceASC)
	{
		return 0.f;
	}
	
	const float PerkPerLevel = SourceASC->GetNumericAttributeBase(Attribute);
	const float DesiredValue = FMath::Floor(CurrentLevel * PerkPerLevel);
	return DesiredValue - PerkPerLevel;
}
