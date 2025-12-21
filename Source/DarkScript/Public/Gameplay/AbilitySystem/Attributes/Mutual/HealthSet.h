#pragma once

#include "AbilitySystemComponent.h"
#include "Gameplay/AbilitySystem/Attributes/DefaultAttributeSet.h"
#include "HealthSet.generated.h"

#define UE_API DARKSCRIPT_API

/**
 * 
 */
UCLASS()
class UHealthSet : public UDefaultAttributeSet
{
	GENERATED_BODY()
	
public:
	
	UE_API UHealthSet();
	
	UE_API virtual void Init() override;
	
	ATTRIBUTE_ACCESSORS(UHealthSet, Health);
	ATTRIBUTE_ACCESSORS(UHealthSet, HealthRegenRate);
	ATTRIBUTE_ACCESSORS(UHealthSet, HealthMax);
	ATTRIBUTE_ACCESSORS(UHealthSet, Healing);
	ATTRIBUTE_ACCESSORS(UHealthSet, Damage);
	
	mutable FAttributeEvent OnHealthChanged;
	mutable FAttributeEvent OnHealthRegenRateChanged;
	mutable FAttributeEvent OnHealthMaxChanged;
	mutable FAttributeEvent OnOutOfHealth;
	
protected:
	
	UFUNCTION()
	UE_API void OnRep_Health(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	UE_API void OnRep_HealthRegenRate(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_HealthMax(const FGameplayAttributeData& OldValue) const;
		
	UE_API virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	UE_API virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
	UE_API virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	UE_API virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	UE_API virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	
	UE_API virtual void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const override;
	
private:
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Attributes|Health", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegenRate, Category = "Attributes|Health", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData HealthRegenRate;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthMax, Category = "Attributes|Health", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData HealthMax;
		
	bool bOutOfHealth;
	
	float HealthBeforeAttributeChange;
	float HealthRegenRateBeforeAttributeChange;
	float HealthMaxBeforeAttributeChange;
	
	
	UPROPERTY(meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Healing;
	
	UPROPERTY(meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Damage;
};

#undef UE_API
