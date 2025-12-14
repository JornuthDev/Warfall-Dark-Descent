// Copyright Dark Script - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_AccuracyDamageMultiplier.generated.h"

/**
 * Modifier Magnitude Calculation pour AccuracyDamageMultiplier
 *
 * Formule : AccuracyDamageMultiplier = BaseAccuracyDamageMultiplier + (Accuracy × 0.005)
 */
UCLASS()
class DARKSCRIPT_API UMMC_AccuracyDamageMultiplier : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_AccuracyDamageMultiplier();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
