// Copyright Dark Script - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_StaminaRegenRate.generated.h"

/**
 * Modifier Magnitude Calculation pour StaminaRegenRate
 *
 * Formule : StaminaRegenRate = MaxStamina × (BaseStaminaRegenRate + (Strength × 0.001))
 */
UCLASS(NotBlueprintType, NotBlueprintable)
class DARKSCRIPT_API UMMC_StaminaRegenRate : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_StaminaRegenRate();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
