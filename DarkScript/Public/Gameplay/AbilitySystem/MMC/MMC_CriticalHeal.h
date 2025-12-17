#pragma once
#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_CriticalHeal.generated.h"

/**
 * Modifier Magnitude Calculation pour CriticalHeal
 *
 * Formule : CriticalHeal = BaseCriticalHeal + (Accuracy × 0.002)
 */
UCLASS(NotBlueprintType, NotBlueprintable)
class DARKSCRIPT_API UMMC_CriticalHeal : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_CriticalHeal();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

