#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Gameplay/AbilitySystem/Attributes/DefaultAttributeSet.h"
#include "FocusSet.generated.h"

#define UE_API DARKSCRIPT_API

/**
 * 
 */
UCLASS()
class UFocusSet : public UDefaultAttributeSet
{
	GENERATED_BODY()
	
	public:
	
	UE_API UFocusSet();
	
	UE_API virtual void Init() override;
	
	ATTRIBUTE_ACCESSORS(UFocusSet, Focus);
	ATTRIBUTE_ACCESSORS(UFocusSet, FocusRegenRate);
	ATTRIBUTE_ACCESSORS(UFocusSet, FocusMax);
	
	mutable FAttributeEvent OnFocusChanged;
	mutable FAttributeEvent OnFocusRegenRateChanged;
	mutable FAttributeEvent OnFocusMaxChanged;

	
protected:
	
	UFUNCTION()
	UE_API void OnRep_Focus(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_FocusRegenRate(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_FocusMax(const FGameplayAttributeData& OldValue) const;
		
	UE_API virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	UE_API virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
	UE_API virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	UE_API virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	UE_API virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	
	UE_API virtual void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const override;
	
private:
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Focus, Category = "Attributes|Focus", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Focus;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FocusRegenRate, Category = "Attributes|Focus", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData FocusRegenRate;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FocusMax, Category = "Attributes|Focus", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData FocusMax;
	
	float FocusBeforeAttributeChange;
	float FocusRegenRateBeforeAttributeChange;
	float FocusMaxBeforeAttributeChange;
	
};

#undef UE_API