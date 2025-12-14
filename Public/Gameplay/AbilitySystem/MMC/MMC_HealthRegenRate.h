// Copyright Dark Script - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_HealthRegenRate.generated.h"

/**
 * Modifier Magnitude Calculation pour HealthRegenRate
 *
 * Formule : HealthRegenRate = MaxHealth × BaseHealthRegenRate
 */
UCLASS()
class DARKSCRIPT_API UMMC_HealthRegenRate : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_HealthRegenRate();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
