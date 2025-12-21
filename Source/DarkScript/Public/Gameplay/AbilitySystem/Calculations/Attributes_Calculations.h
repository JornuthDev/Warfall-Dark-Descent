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

protected:
	virtual float DoCalculation(const FGameplayEffectSpec& Spec) const override final;
	
	virtual FGameplayAttribute GetAttributeToCapture() const { return FGameplayAttribute(); }
	virtual FGameplayAttribute GetAttributeToBaseValue() const { return FGameplayAttribute(); }
	virtual float GetMultiplier() const { return 1.0f; }
};

UCLASS()
class DARKSCRIPT_API UCalculation_HealthMax : public UAttributes_Calculations
{
	GENERATED_BODY()
	
public:
	UCalculation_HealthMax();
	
protected:
	virtual FGameplayAttribute GetAttributeToCapture() const override { return UPlayerSet::GetVitalityAttribute(); }
	virtual FGameplayAttribute GetAttributeToBaseValue() const override { return UHealthSet::GetHealthMaxAttribute(); }
	virtual float GetMultiplier() const override { return AttributeRules::HealthMaxPerPerk; }
	
	FGameplayEffectAttributeCaptureDefinition VitalityDef;
};

UCLASS()
class DARKSCRIPT_API UCalculation_HealthRegenRate : public UAttributes_Calculations
{
	GENERATED_BODY()
	
public:
	UCalculation_HealthRegenRate();
	
protected:
	virtual FGameplayAttribute GetAttributeToCapture() const override { return UPlayerSet::GetVitalityAttribute(); }
	virtual FGameplayAttribute GetAttributeToBaseValue() const override { return UHealthSet::GetHealthRegenRateAttribute(); }
	virtual float GetMultiplier() const override { return AttributeRules::HealthRegenRatePerPerk; }
	
	FGameplayEffectAttributeCaptureDefinition VitalityDef;
};

UCLASS()
class DARKSCRIPT_API UCalculation_ManaMax : public UAttributes_Calculations
{
	GENERATED_BODY()
	
public:
	UCalculation_ManaMax();
	
protected:
	virtual FGameplayAttribute GetAttributeToCapture() const override { return UPlayerSet::GetIntelligenceAttribute(); }
	virtual FGameplayAttribute GetAttributeToBaseValue() const override { return UManaSet::GetManaMaxAttribute(); }
	virtual float GetMultiplier() const override { return AttributeRules::ManaMaxPerPerk; }
	
	FGameplayEffectAttributeCaptureDefinition IntelligenceDef;
};

UCLASS()
class DARKSCRIPT_API UCalculation_ManaRegenRate : public UAttributes_Calculations
{
	GENERATED_BODY()
	
public:
	UCalculation_ManaRegenRate();
	
protected:
	virtual FGameplayAttribute GetAttributeToCapture() const override { return UPlayerSet::GetMentalityAttribute(); }
	virtual FGameplayAttribute GetAttributeToBaseValue() const override { return UManaSet::GetManaRegenRateAttribute(); }
	virtual float GetMultiplier() const override { return AttributeRules::ManaRegenRatePerPerk; }
	
	FGameplayEffectAttributeCaptureDefinition MentalityDef;
};

UCLASS()
class DARKSCRIPT_API UCalculation_StaminaMax : public UAttributes_Calculations
{
	GENERATED_BODY()
	
public:
	UCalculation_StaminaMax();
	
protected:
	virtual FGameplayAttribute GetAttributeToCapture() const override { return UPlayerSet::GetStrengthAttribute(); }
	virtual FGameplayAttribute GetAttributeToBaseValue() const override { return UStaminaSet::GetStaminaMaxAttribute(); }
	virtual float GetMultiplier() const override { return AttributeRules::StaminaMaxPerPerk; }
	
	FGameplayEffectAttributeCaptureDefinition StrengthDef;
};

UCLASS()
class DARKSCRIPT_API UCalculation_StaminaRegenRate : public UAttributes_Calculations
{
	GENERATED_BODY()
	
public:
	UCalculation_StaminaRegenRate();
	
protected:
	virtual FGameplayAttribute GetAttributeToCapture() const override { return UPlayerSet::GetStrengthAttribute(); }
	virtual FGameplayAttribute GetAttributeToBaseValue() const override { return UStaminaSet::GetStaminaRegenRateAttribute(); }
	virtual float GetMultiplier() const override { return AttributeRules::StaminaRegenRatePerPerk; }
	
	FGameplayEffectAttributeCaptureDefinition StrengthDef;
};

UCLASS()
class DARKSCRIPT_API UCalculation_FocusMax : public UAttributes_Calculations
{
	GENERATED_BODY()
	
public:
	UCalculation_FocusMax();
	
protected:
	virtual FGameplayAttribute GetAttributeToCapture() const override { return UPlayerSet::GetAccuracyAttribute(); }
	virtual FGameplayAttribute GetAttributeToBaseValue() const override { return UFocusSet::GetFocusMaxAttribute(); }
	virtual float GetMultiplier() const override { return AttributeRules::FocusMaxPerPerk; }
	
	FGameplayEffectAttributeCaptureDefinition AccuracyDef;
};

UCLASS()
class DARKSCRIPT_API UCalculation_FocusRegenRate : public UAttributes_Calculations
{
	GENERATED_BODY()
	
public:
	UCalculation_FocusRegenRate();
	
protected:
	virtual FGameplayAttribute GetAttributeToCapture() const override { return UPlayerSet::GetAccuracyAttribute(); }
	virtual FGameplayAttribute GetAttributeToBaseValue() const override { return UFocusSet::GetFocusRegenRateAttribute(); }
	virtual float GetMultiplier() const override { return AttributeRules::FocusRegenRatePerPerk; }
	
	FGameplayEffectAttributeCaptureDefinition AccuracyDef;
};