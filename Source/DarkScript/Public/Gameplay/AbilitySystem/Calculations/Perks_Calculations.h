#pragma once

#include "CoreMinimal.h"
#include "Gameplay/AbilitySystem/Attributes/PlayerSet.h"
#include "Gameplay/AbilitySystem/Calculations/DefaultCalculationMod.h"
#include "Perks_Calculations.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class DARKSCRIPT_API UPerks_Calculations : public UDefaultCalculationMod
{
	GENERATED_BODY()
	
protected:
	virtual void Init() const override final;
	virtual float DoCalculation(const FGameplayEffectSpec& Spec) const override final;
	virtual FGameplayAttribute GetAttributeForCapture() const override final { return UPlayerSet::GetLevelAttribute(); }
	virtual FGameplayAttribute GetAttribute() const { return FGameplayAttribute(); }
	FGameplayAttribute Attribute;
};

UCLASS(Abstract)
class DARKSCRIPT_API UCalculation_Vitality : public UPerks_Calculations
{
	GENERATED_BODY()

	virtual FGameplayAttribute GetAttribute() const override { return UPlayerSet::GetVitalityAttribute(); }
};

UCLASS(Abstract)
class DARKSCRIPT_API UCalculation_Strength : public UPerks_Calculations
{
	GENERATED_BODY()
	
	virtual FGameplayAttribute GetAttribute() const override { return UPlayerSet::GetStrengthAttribute(); }
};

UCLASS(Abstract)
class DARKSCRIPT_API UCalculation_Accuracy : public UPerks_Calculations
{
	GENERATED_BODY()
	
	virtual FGameplayAttribute GetAttribute() const override { return UPlayerSet::GetAccuracyAttribute(); }
};

UCLASS(Abstract)
class DARKSCRIPT_API UCalculation_Intelligence : public UPerks_Calculations
{
	GENERATED_BODY()
	
	virtual FGameplayAttribute GetAttribute() const override { return UPlayerSet::GetIntelligenceAttribute(); }
};

UCLASS(Abstract)
class DARKSCRIPT_API UCalculation_Mentality : public UPerks_Calculations
{
	GENERATED_BODY()
	
	virtual FGameplayAttribute GetAttribute() const override { return UPlayerSet::GetMentalityAttribute(); }
};

