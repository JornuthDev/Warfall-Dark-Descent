// Copyright Dark Script - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_MaxFocus.generated.h"

/**
 * Modifier Magnitude Calculation pour MaxFocus
 *
 * Formule : MaxFocus = BaseFocus + (Accuracy × 2.5)
 */
UCLASS()
class DARKSCRIPT_API UMMC_MaxFocus : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_MaxFocus();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
