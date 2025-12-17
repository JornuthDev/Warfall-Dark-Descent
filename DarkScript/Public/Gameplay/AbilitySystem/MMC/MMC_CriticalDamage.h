// Copyright Dark Script - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_CriticalDamage.generated.h"

/**
 * Modifier Magnitude Calculation pour CriticalDamage
 *
 * Formule : CriticalDamage = BaseCriticalDamage + (Accuracy × 0.002)
 */
UCLASS(NotBlueprintType, NotBlueprintable)
class DARKSCRIPT_API UMMC_CriticalDamage : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_CriticalDamage();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
