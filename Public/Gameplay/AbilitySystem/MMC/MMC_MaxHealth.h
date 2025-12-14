// Copyright Dark Script - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_MaxHealth.generated.h"

/**
 * Modifier Magnitude Calculation pour MaxHealth
 *
 * Formule : MaxHealth = BaseHealth + (Vitality × 5)
 *
 * Capture les attributs :
 * - BaseHealth (de l'ArchetypeAttributeSet)
 * - Vitality (de l'ArchetypeAttributeSet)
 */
UCLASS()
class DARKSCRIPT_API UMMC_MaxHealth : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_MaxHealth();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
