// Copyright Dark Script - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_FocusRegenRate.generated.h"

/**
 * Modifier Magnitude Calculation pour FocusRegenRate
 *
 * Formule : FocusRegenRate = MaxFocus × (BaseFocusRegenRate + (Accuracy × 0.001))
 */
UCLASS()
class DARKSCRIPT_API UMMC_FocusRegenRate : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_FocusRegenRate();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
