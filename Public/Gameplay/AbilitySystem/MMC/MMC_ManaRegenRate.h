// Copyright Dark Script - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_ManaRegenRate.generated.h"

/**
 * Modifier Magnitude Calculation pour ManaRegenRate
 *
 * Formule : ManaRegenRate = MaxMana × (BaseManaRegenRate + (Mentality × 0.001))
 */
UCLASS()
class DARKSCRIPT_API UMMC_ManaRegenRate : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_ManaRegenRate();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
