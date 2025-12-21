// Calculations/Attributes_Calculations.h
#pragma once

#include "CoreMinimal.h"
#include "Gameplay/AbilitySystem/Calculations/DefaultCalculationMod.h"
#include "Gameplay/AbilitySystem/Attributes/PlayerSet.h"
#include "GameplayRules.h"
#include "Gameplay/AbilitySystem/Attributes/Mutual/HealthSet.h"
#include "Gameplay/AbilitySystem/Attributes/Resources/FocusSet.h"
#include "Gameplay/AbilitySystem/Attributes/Resources/ManaSet.h"
#include "Gameplay/AbilitySystem/Attributes/Resources/StaminaSet.h"
#include "Attributes_Calculations.generated.h"

/**
 * 
 */
UCLASS()
class DARKSCRIPT_API UAttributes_Calculations : public UDefaultCalculationMod
{
	GENERATED_BODY()
	
public:
	virtual void PostInitProperties() override;
	
protected:
	virtual float DoCalculation(const FGameplayEffectSpec& Spec) const override final;
	
	virtual FGameplayAttribute GetAttributeToBaseValue() const { return FGameplayAttribute(); }
	virtual float GetMultiplier() const { return 1.0f; }
	
	FGameplayAttribute AttributeToBaseValue;
	float MultiplyValue;
};

UCLASS()
class DARKSCRIPT_API UCalculation_HealthMax : public UAttributes_Calculations
{
	GENERATED_BODY()
	
	virtual FGameplayAttribute GetAttributeForCapture() const override { return UPlayerSet::GetVitalityAttribute(); }
	virtual FGameplayAttribute GetAttributeToBaseValue() const override { return UHealthSet::GetHealthMaxAttribute(); }
	virtual float GetMultiplier() const override { return AttributeRules::HealthMaxPerPerk; }
};

UCLASS()
class DARKSCRIPT_API UCalculation_HealthRegenRate : public UAttributes_Calculations
{
	GENERATED_BODY()
		
	virtual FGameplayAttribute GetAttributeForCapture() const override { return UPlayerSet::GetVitalityAttribute(); }
	virtual FGameplayAttribute GetAttributeToBaseValue() const override { return UHealthSet::GetHealthRegenRateAttribute(); }
	virtual float GetMultiplier() const override { return AttributeRules::HealthRegenRatePerPerk; }
};

UCLASS()
class DARKSCRIPT_API UCalculation_ManaMax : public UAttributes_Calculations
{
	GENERATED_BODY()
		
	virtual FGameplayAttribute GetAttributeForCapture() const override { return UPlayerSet::GetIntelligenceAttribute(); }
	virtual FGameplayAttribute GetAttributeToBaseValue() const override { return UManaSet::GetManaMaxAttribute(); }
	virtual float GetMultiplier() const override { return AttributeRules::ManaMaxPerPerk; }
};

UCLASS()
class DARKSCRIPT_API UCalculation_ManaRegenRate : public UAttributes_Calculations
{
	GENERATED_BODY()
		
	virtual FGameplayAttribute GetAttributeForCapture() const override { return UPlayerSet::GetMentalityAttribute(); }
	virtual FGameplayAttribute GetAttributeToBaseValue() const override { return UManaSet::GetManaRegenRateAttribute(); }
	virtual float GetMultiplier() const override { return AttributeRules::ManaRegenRatePerPerk; }
};

UCLASS()
class DARKSCRIPT_API UCalculation_StaminaMax : public UAttributes_Calculations
{
	GENERATED_BODY()
		
	virtual FGameplayAttribute GetAttributeForCapture() const override { return UPlayerSet::GetStrengthAttribute(); }
	virtual FGameplayAttribute GetAttributeToBaseValue() const override { return UStaminaSet::GetStaminaMaxAttribute(); }
	virtual float GetMultiplier() const override { return AttributeRules::StaminaMaxPerPerk; }
};

UCLASS()
class DARKSCRIPT_API UCalculation_StaminaRegenRate : public UAttributes_Calculations
{
	GENERATED_BODY()
		
	virtual FGameplayAttribute GetAttributeForCapture() const override { return UPlayerSet::GetStrengthAttribute(); }
	virtual FGameplayAttribute GetAttributeToBaseValue() const override { return UStaminaSet::GetStaminaRegenRateAttribute(); }
	virtual float GetMultiplier() const override { return AttributeRules::StaminaRegenRatePerPerk; }
};

UCLASS()
class DARKSCRIPT_API UCalculation_FocusMax : public UAttributes_Calculations
{
	GENERATED_BODY()
	
	virtual FGameplayAttribute GetAttributeForCapture() const override { return UPlayerSet::GetAccuracyAttribute(); }
	virtual FGameplayAttribute GetAttributeToBaseValue() const override { return UFocusSet::GetFocusMaxAttribute(); }
	virtual float GetMultiplier() const override { return AttributeRules::FocusMaxPerPerk; }
};

UCLASS()
class DARKSCRIPT_API UCalculation_FocusRegenRate : public UAttributes_Calculations
{
	GENERATED_BODY()
		
	virtual FGameplayAttribute GetAttributeForCapture() const override { return UPlayerSet::GetAccuracyAttribute(); }
	virtual FGameplayAttribute GetAttributeToBaseValue() const override { return UFocusSet::GetFocusRegenRateAttribute(); }
	virtual float GetMultiplier() const override { return AttributeRules::FocusRegenRatePerPerk; }
};