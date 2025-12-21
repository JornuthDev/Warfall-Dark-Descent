#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Gameplay/AbilitySystem/Attributes/DefaultAttributeSet.h"
#include "PropertyInstigator.generated.h"

#define UE_API DARKSCRIPT_API

/**
 *
 */
UCLASS()
class UPropertyInstigator : public UDefaultAttributeSet
{
	GENERATED_BODY()

public:

	UE_API UPropertyInstigator();

	ATTRIBUTE_ACCESSORS(UPropertyInstigator, PhysicDamageMultiplier); // Strength
	ATTRIBUTE_ACCESSORS(UPropertyInstigator, RangedDamageMultiplier); // Accuracy
	ATTRIBUTE_ACCESSORS(UPropertyInstigator, MagicDamageMultiplier); // Intelligence
	ATTRIBUTE_ACCESSORS(UPropertyInstigator, HealingMultiplier); // Mentality
	ATTRIBUTE_ACCESSORS(UPropertyInstigator, PhysicCriticalChance); // Accuracy
	ATTRIBUTE_ACCESSORS(UPropertyInstigator, PhysicCriticalPower); // Strength
	ATTRIBUTE_ACCESSORS(UPropertyInstigator, MagicCriticalChance); // Mentality
	ATTRIBUTE_ACCESSORS(UPropertyInstigator, MagicCriticalPower); // Intelligence
	ATTRIBUTE_ACCESSORS(UPropertyInstigator, HealCriticalChance); // Mentality
	ATTRIBUTE_ACCESSORS(UPropertyInstigator, HealCriticalPower); // Intelligence

	mutable FAttributeEvent OnPhysicDamageMultiplierChanged;
	mutable FAttributeEvent OnRangedDamageMultiplierChanged;
	mutable FAttributeEvent OnMagicDamageMultiplierChanged;
	mutable FAttributeEvent OnHealingMultiplierChanged;
	mutable FAttributeEvent OnPhysicCriticalChanceChanged;
	mutable FAttributeEvent OnPhysicCriticalPowerChanged;
	mutable FAttributeEvent OnMagicCriticalChanceChanged;
	mutable FAttributeEvent OnMagicCriticalPowerChanged;
	mutable FAttributeEvent OnHealCriticalChanceChanged;
	mutable FAttributeEvent OnHealCriticalPowerChanged;

protected:

	UFUNCTION()
	UE_API void OnRep_PhysicDamageMultiplier(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	UE_API void OnRep_RangedDamageMultiplier(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	UE_API void OnRep_MagicDamageMultiplier(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	UE_API void OnRep_HealingMultiplier(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	UE_API void OnRep_PhysicCriticalChance(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	UE_API void OnRep_PhysicCriticalPower(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	UE_API void OnRep_MagicCriticalChance(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	UE_API void OnRep_MagicCriticalPower(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	UE_API void OnRep_HealCriticalChance(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	UE_API void OnRep_HealCriticalPower(const FGameplayAttributeData& OldValue) const;

	UE_API virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	UE_API virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	UE_API virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	UE_API virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	UE_API virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	UE_API virtual void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const override;

private:

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicDamageMultiplier, Category = "Attributes|Character", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData PhysicDamageMultiplier;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RangedDamageMultiplier, Category = "Attributes|Character", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData RangedDamageMultiplier;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicDamageMultiplier, Category = "Attributes|Character", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData MagicDamageMultiplier;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealingMultiplier, Category = "Attributes|Character", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData HealingMultiplier;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicCriticalChance, Category = "Attributes|Character", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData PhysicCriticalChance;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicCriticalPower, Category = "Attributes|Character", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData PhysicCriticalPower;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicCriticalChance, Category = "Attributes|Character", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData MagicCriticalChance;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicCriticalPower, Category = "Attributes|Character", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData MagicCriticalPower;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealCriticalChance, Category = "Attributes|Character", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData HealCriticalChance;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealCriticalPower, Category = "Attributes|Character", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData HealCriticalPower;
		
	float PhysicDamageMultiplierBeforeAttributeChange;
	float RangedDamageMultiplierBeforeAttributeChange;
	float MagicDamageMultiplierBeforeAttributeChange;
	float HealingMultiplierBeforeAttributeChange;
	float PhysicCriticalChanceBeforeAttributeChange;
	float PhysicCriticalPowerBeforeAttributeChange;
	float MagicCriticalChanceBeforeAttributeChange;
	float MagicCriticalPowerBeforeAttributeChange;
	float HealCriticalChanceBeforeAttributeChange;
	float HealCriticalPowerBeforeAttributeChange;
};

#undef UE_API