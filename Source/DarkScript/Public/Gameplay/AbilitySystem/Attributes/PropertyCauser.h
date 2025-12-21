#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "DefaultAttributeSet.h"
#include "PropertyCauser.generated.h"

#define UE_API DARKSCRIPT_API

/**
 * 
 */
UCLASS()
class UPropertyCauser : public UDefaultAttributeSet
{
	GENERATED_BODY()
	
public:
	
	UE_API UPropertyCauser();
	
	ATTRIBUTE_ACCESSORS(UPropertyCauser, Damage);
	ATTRIBUTE_ACCESSORS(UPropertyCauser, ArmorPenetration);
	ATTRIBUTE_ACCESSORS(UPropertyCauser, FleshMultiplier);
	ATTRIBUTE_ACCESSORS(UPropertyCauser, PenetrationMultiplier);
	ATTRIBUTE_ACCESSORS(UPropertyCauser, IceMultiplier);
	ATTRIBUTE_ACCESSORS(UPropertyCauser, FireMultiplier);
	ATTRIBUTE_ACCESSORS(UPropertyCauser, LightningMultiplier);
	ATTRIBUTE_ACCESSORS(UPropertyCauser, HolyMultiplier);
	ATTRIBUTE_ACCESSORS(UPropertyCauser, PoisonMultiplier);
	ATTRIBUTE_ACCESSORS(UPropertyCauser, BleedingMultiplier);
	ATTRIBUTE_ACCESSORS(UPropertyCauser, HealingMultiplier);
	ATTRIBUTE_ACCESSORS(UPropertyCauser, SpeedMultiplier)
	
	mutable FAttributeEvent OnDamageChanged;
	mutable FAttributeEvent OnArmorPenetrationChanged;
	mutable FAttributeEvent OnFleshMultiplierChanged;
	mutable FAttributeEvent OnPenetrationMultiplierChanged;
	mutable FAttributeEvent OnIceMultiplierChanged;
	mutable FAttributeEvent OnFireMultiplierChanged;
	mutable FAttributeEvent OnLightningMultiplierChanged;
	mutable FAttributeEvent OnHolyMultiplierChanged;
	mutable FAttributeEvent OnPoisonMultiplierChanged;
	mutable FAttributeEvent OnBleedingMultiplierChanged;
	mutable FAttributeEvent OnHealingMultiplierChanged;
	mutable FAttributeEvent OnSpeedMultiplierChanged;

protected:
	
	UFUNCTION()
	UE_API void OnRep_Damage(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_ArmorPenetration(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_FleshMultiplier(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_PenetrationMultiplier(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_IceMultiplier(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_FireMultiplier(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_LightningMultiplier(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_HolyMultiplier(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_PoisonMultiplier(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_BleedingMultiplier(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	UE_API void OnRep_HealingMultiplier(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_SpeedMultiplier(const FGameplayAttributeData& OldValue) const;
	
	UE_API virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	UE_API virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
	UE_API virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	UE_API virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	UE_API virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;	
	
	UE_API virtual void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const override;
	
private:
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Damage, Category = "Attributes|Object", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Damage;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorPenetration, Category = "Attributes|Object", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ArmorPenetration;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FleshMultiplier, Category = "Attributes|Object", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData FleshMultiplier;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PenetrationMultiplier, Category = "Attributes|Object", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData PenetrationMultiplier;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IceMultiplier, Category = "Attributes|Object", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData IceMultiplier;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FireMultiplier, Category = "Attributes|Object", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData FireMultiplier;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LightningMultiplier, Category = "Attributes|Object", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData LightningMultiplier;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HolyMultiplier, Category = "Attributes|Object", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData HolyMultiplier;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PoisonMultiplier, Category = "Attributes|Object", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData PoisonMultiplier;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BleedingMultiplier, Category = "Attributes|Object", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData BleedingMultiplier;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealingMultiplier, Category = "Attributes|Object", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData HealingMultiplier;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SpeedMultiplier, Category = "Attributes|Object", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData SpeedMultiplier;
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Properties", meta = (Categories = "Ability.Causer.Type.", AllowPrivateAccess = true))
	FGameplayTag CauserType;
	
	float DamageBeforeAttributeChange;
	float ArmorPenetrationBeforeAttributeChange;
	float FleshMultiplierBeforeAttributeChange;
	float PenetrationMultiplierBeforeAttributeChange;
	float IceMultiplierBeforeAttributeChange;
	float FireMultiplierBeforeAttributeChange;
	float LightningMultiplierBeforeAttributeChange;
	float HolyMultiplierBeforeAttributeChange;
	float PoisonMultiplierBeforeAttributeChange;
	float BleedingMultiplierBeforeAttributeChange;
	float HealingMultiplierBeforeAttributeChange;
	float SpeedMultiplierBeforeAttributeChange;
};

#undef UE_API