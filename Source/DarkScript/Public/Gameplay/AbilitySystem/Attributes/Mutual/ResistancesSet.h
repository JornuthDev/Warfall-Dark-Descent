#pragma once

#include "AbilitySystemComponent.h"
#include "Gameplay/AbilitySystem/Attributes/DefaultAttributeSet.h"
#include "NativeGameplayTags.h"
#include "ResistancesSet.generated.h"

#define UE_API DARKSCRIPT_API

/**
 * 
 */
UCLASS()
class UResistancesSet : public UDefaultAttributeSet
{
	GENERATED_BODY()
	
public:
	
	UE_API UResistancesSet();
	
	ATTRIBUTE_ACCESSORS(UResistancesSet, Armor);
	ATTRIBUTE_ACCESSORS(UResistancesSet, Ice);
	ATTRIBUTE_ACCESSORS(UResistancesSet, Fire);
	ATTRIBUTE_ACCESSORS(UResistancesSet, Lightning);
	ATTRIBUTE_ACCESSORS(UResistancesSet, Holy);
	ATTRIBUTE_ACCESSORS(UResistancesSet, Poison);
	ATTRIBUTE_ACCESSORS(UResistancesSet, Bleeding);
	
	mutable FAttributeEvent OnArmorChanged;
	mutable FAttributeEvent OnIceChanged;
	mutable FAttributeEvent OnFireChanged;
	mutable FAttributeEvent OnLightningChanged;
	mutable FAttributeEvent OnHolyChanged;
	mutable FAttributeEvent OnPoisonChanged;
	mutable FAttributeEvent OnBleedingChanged;
	
	UE_API float CalculateReducedDamage(const FGameplayTag& Tag, const float Magnitude, const float EffectiveArmor = 1.f) const;
	
protected:
	
	UFUNCTION()
	UE_API void OnRep_Armor(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_Ice(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_Fire(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_Lightning(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_Holy(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_Poison(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_Bleeding(const FGameplayAttributeData& OldValue) const;
	
	UE_API virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	UE_API virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
	UE_API virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	UE_API virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	UE_API virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	
	UE_API virtual void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const override;
	
private:
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Attributes|Resistances", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Armor;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Ice, Category = "Attributes|Health", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Ice;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Fire, Category = "Attributes|Health", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Fire;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Lightning, Category = "Attributes|Health", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Lightning;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Holy, Category = "Attributes|Health", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Holy;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Poison, Category = "Attributes|Health", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Poison;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Bleeding, Category = "Attributes|Health", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Bleeding;
	
	float ArmorBeforeAttributeChange;
	float IceBeforeAttributeChange;
	float FireBeforeAttributeChange;
	float LightningBeforeAttributeChange;
	float HolyBeforeAttributeChange;
	float PoisonBeforeAttributeChange;
	float BleedingBeforeAttributeChange;
};

#undef UE_API