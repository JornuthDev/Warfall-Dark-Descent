#pragma once
#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Gameplay/AbilitySystem/Attributes/DefaultAttributeSet.h"
#include "ManaSet.generated.h"

#define UE_API DARKSCRIPT_API

/**
 * 
 */
UCLASS()
class UManaSet : public UDefaultAttributeSet
{
	GENERATED_BODY()
	
	public:
	
	UE_API UManaSet();
	
	UE_API virtual void Init() override;
	
	ATTRIBUTE_ACCESSORS(UManaSet, Mana);
	ATTRIBUTE_ACCESSORS(UManaSet, ManaRegenRate);
	ATTRIBUTE_ACCESSORS(UManaSet, ManaMax);
	
	mutable FAttributeEvent OnManaChanged;
	mutable FAttributeEvent OnManaRegenRateChanged;
	mutable FAttributeEvent OnManaMaxChanged;

	
protected:
	
	UFUNCTION()
	UE_API void OnRep_Mana(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_ManaRegenRate(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_ManaMax(const FGameplayAttributeData& OldValue) const;
		
	UE_API virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	UE_API virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
	UE_API virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	UE_API virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	UE_API virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	
	UE_API virtual void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const override;
	
private:
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Attributes|Mana", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Mana;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ManaRegenRate, Category = "Attributes|Mana", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ManaRegenRate;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ManaMax, Category = "Attributes|Mana", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ManaMax;
		
	float ManaBeforeAttributeChange;
	float ManaRegenRateBeforeAttributeChange;
	float ManaMaxBeforeAttributeChange;
	
};

#undef UE_API