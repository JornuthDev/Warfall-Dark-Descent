// Copyright Dark Script - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_MagicDamageMultiplier.generated.h"

/**
 * Modifier Magnitude Calculation pour MagicDamageMultiplier
 *
 * Formule : MagicDamageMultiplier = BaseMagicDamageMultiplier + (Intelligence × 0.005)
 */
UCLASS()
class DARKSCRIPT_API UMMC_MagicDamageMultiplier : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_MagicDamageMultiplier();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
