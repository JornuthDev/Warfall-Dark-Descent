// Copyright Dark Script - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_PhysicDamageMultiplier.generated.h"

/**
 * Modifier Magnitude Calculation pour PhysicDamageMultiplier
 *
 * Formule : PhysicDamageMultiplier = BasePhysicDamageMultiplier + (Strength × 0.005)
 */
UCLASS(NotBlueprintType, NotBlueprintable)
class DARKSCRIPT_API UMMC_PhysicDamageMultiplier : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_PhysicDamageMultiplier();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
