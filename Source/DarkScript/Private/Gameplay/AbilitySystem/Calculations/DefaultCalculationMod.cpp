#include "Gameplay/AbilitySystem/Calculations/DefaultCalculationMod.h"

float UDefaultCalculationMod::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	if (!bIsInitialized)
	{
		Init();
	}
	return DoCalculation(Spec);
}

void UDefaultCalculationMod::Init() const
{
	AttributeDef.AttributeToCapture = GetAttributeForCapture();
	AttributeDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	AttributeDef.bSnapshot = false;
	
	const_cast<UDefaultCalculationMod*>(this)->RelevantAttributesToCapture.Add(AttributeDef);
}
