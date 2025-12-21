#include "Gameplay/AbilitySystem/Calculations/DefaultCalculationMod.h"

float UDefaultCalculationMod::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	return DoCalculation(Spec);
}