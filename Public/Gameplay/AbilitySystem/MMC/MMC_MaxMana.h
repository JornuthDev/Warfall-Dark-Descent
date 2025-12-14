// Copyright Dark Script - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_MaxMana.generated.h"

/**
 * Modifier Magnitude Calculation pour MaxMana
 *
 * Formule : MaxMana = BaseMana + (Intelligence × 2.5)
 */
UCLASS()
class DARKSCRIPT_API UMMC_MaxMana : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_MaxMana();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
