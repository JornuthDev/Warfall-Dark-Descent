// Copyright Dark Script - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_CriticalChance.generated.h"

/**
 * Modifier Magnitude Calculation pour CriticalChance
 *
 * Formule : CriticalChance = BaseCriticalChance + (Accuracy × 0.002)
 */
UCLASS(NotBlueprintType, NotBlueprintable)
class DARKSCRIPT_API UMMC_CriticalChance : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_CriticalChance();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
