#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "DefaultCalculationMod.generated.h"

/**
 * 
 */
UCLASS()
class DARKSCRIPT_API UDefaultCalculationMod : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override final;
	
protected:
	
	virtual float DoCalculation(const FGameplayEffectSpec& Spec) const { return 0.f; }
};