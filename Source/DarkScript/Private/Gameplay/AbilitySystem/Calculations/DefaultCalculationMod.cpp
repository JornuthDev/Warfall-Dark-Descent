#include "Gameplay/AbilitySystem/Calculations/DefaultCalculationMod.h"

void UDefaultCalculationMod::PostInitProperties()
{
	Super::PostInitProperties();
	
	// ✅ PostInitProperties is called AFTER construction is complete
	// Virtual functions work correctly here!
	const FGameplayAttribute AttributeToCapture = GetAttributeForCapture();
	
	if (AttributeToCapture.IsValid())
	{
		AttributeDef.AttributeToCapture = AttributeToCapture;
		AttributeDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
		AttributeDef.bSnapshot = false;
		
		RelevantAttributesToCapture.Add(AttributeDef);
	}
}

float UDefaultCalculationMod::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	return DoCalculation(Spec);
}