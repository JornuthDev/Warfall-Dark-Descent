#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Gameplay/AbilitySystem/Attributes/DefaultAttributeSet.h"
#include "StaminaSet.generated.h"

#define UE_API DARKSCRIPT_API

/**
 * 
 */
UCLASS()
class UStaminaSet : public UDefaultAttributeSet
{
	GENERATED_BODY()
	
	public:
	
	UE_API UStaminaSet();
	
	UE_API virtual void Init() override;
	
	ATTRIBUTE_ACCESSORS(UStaminaSet, Stamina);
	ATTRIBUTE_ACCESSORS(UStaminaSet, StaminaRegenRate);
	ATTRIBUTE_ACCESSORS(UStaminaSet, StaminaMax);
	
	mutable FAttributeEvent OnStaminaChanged;
	mutable FAttributeEvent OnStaminaRegenRateChanged;
	mutable FAttributeEvent OnStaminaMaxChanged;
	
protected:
	
	UFUNCTION()
	UE_API void OnRep_Stamina(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_StaminaRegenRate(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_StaminaMax(const FGameplayAttributeData& OldValue) const;
		
	UE_API virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	UE_API virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
	UE_API virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	UE_API virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	UE_API virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	
	UE_API virtual void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const override;
	
private:
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stamina, Category = "Attributes|Stamina", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Stamina;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_StaminaRegenRate, Category = "Attributes|Stamina", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData StaminaRegenRate;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_StaminaMax, Category = "Attributes|Stamina", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData StaminaMax;
	
	float StaminaBeforeAttributeChange;
	float StaminaRegenRateBeforeAttributeChange;
	float StaminaMaxBeforeAttributeChange;
	
};

#undef UE_API