// Calculations/Perks_Calculations.h
#pragma once

#include "CoreMinimal.h"
#include "Gameplay/AbilitySystem/Attributes/PlayerSet.h"
#include "Gameplay/AbilitySystem/Calculations/DefaultCalculationMod.h"
#include "Perks_Calculations.generated.h"

/**
 * 
 */
UCLASS()
class DARKSCRIPT_API UPerks_Calculations : public UDefaultCalculationMod
{
	GENERATED_BODY()
	
public:
	virtual void PostInitProperties() override;
	
protected:
	virtual float DoCalculation(const FGameplayEffectSpec& Spec) const override final;
	virtual FGameplayAttribute GetAttributeForCapture() const override final { return UPlayerSet::GetLevelAttribute(); }
	virtual FGameplayAttribute GetAttribute() const { return FGameplayAttribute(); }
	
	FGameplayAttribute Attribute;
};

UCLASS()
class DARKSCRIPT_API UCalculation_Vitality : public UPerks_Calculations
{
	GENERATED_BODY()

	virtual FGameplayAttribute GetAttribute() const override { return UPlayerSet::GetVitalityAttribute(); }
};

UCLASS()
class DARKSCRIPT_API UCalculation_Strength : public UPerks_Calculations
{
	GENERATED_BODY()
	
	virtual FGameplayAttribute GetAttribute() const override { return UPlayerSet::GetStrengthAttribute(); }
};

UCLASS()
class DARKSCRIPT_API UCalculation_Accuracy : public UPerks_Calculations
{
	GENERATED_BODY()
	
	virtual FGameplayAttribute GetAttribute() const override { return UPlayerSet::GetAccuracyAttribute(); }
};

UCLASS()
class DARKSCRIPT_API UCalculation_Intelligence : public UPerks_Calculations
{
	GENERATED_BODY()
	
	virtual FGameplayAttribute GetAttribute() const override { return UPlayerSet::GetIntelligenceAttribute(); }
};

UCLASS()
class DARKSCRIPT_API UCalculation_Mentality : public UPerks_Calculations
{
	GENERATED_BODY()
	
	virtual FGameplayAttribute GetAttribute() const override { return UPlayerSet::GetMentalityAttribute(); }
};