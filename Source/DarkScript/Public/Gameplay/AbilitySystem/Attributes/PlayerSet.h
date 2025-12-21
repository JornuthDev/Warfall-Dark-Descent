#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "DefaultAttributeSet.h"
#include "PlayerSet.generated.h"

#define UE_API DARKSCRIPT_API

/**
 * 
 */
UCLASS()
class UPlayerSet : public UDefaultAttributeSet
{
	GENERATED_BODY()
	
public:
	
	UE_API UPlayerSet();
	UE_API virtual void Init() override;
	
	ATTRIBUTE_ACCESSORS(UPlayerSet, Level);
	ATTRIBUTE_ACCESSORS(UPlayerSet, Experience);
	ATTRIBUTE_ACCESSORS(UPlayerSet, ExpForNextLevel);
	ATTRIBUTE_ACCESSORS(UPlayerSet, Vitality);
	ATTRIBUTE_ACCESSORS(UPlayerSet, Strength);
	ATTRIBUTE_ACCESSORS(UPlayerSet, Accuracy);
	ATTRIBUTE_ACCESSORS(UPlayerSet, Intelligence);
	ATTRIBUTE_ACCESSORS(UPlayerSet, Mentality);
	
	mutable FAttributeEvent OnLevelChanged;
	mutable FAttributeEvent OnExperienceChanged;
	mutable FAttributeEvent OnVitalityChanged;
	mutable FAttributeEvent OnStrengthChanged;
	mutable FAttributeEvent OnAccuracyChanged;
	mutable FAttributeEvent OnIntelligenceChanged;
	mutable FAttributeEvent OnMentalityChanged;
	
protected:
	
	UFUNCTION()
	UE_API void OnRep_Level(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_Experience(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_ExpForNextLevel(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_Vitality(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_Strength(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_Accuracy(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_Intelligence(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	UE_API void OnRep_Mentality(const FGameplayAttributeData& OldValue) const;
	
	UE_API virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	
	UE_API virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
	UE_API virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	UE_API virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	UE_API virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	
	UE_API virtual void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const override;
	UE_API float CalculateExpRequiredForLevel() const;
	
	UE_API void OnExperienceUpdating(const struct FGameplayEffectModCallbackData& Data);
	UE_API void OnLevelUp(const FGameplayEffectModCallbackData& Data);
	
private:
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Level, Category = "Attributes|Player|Progression", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Level;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Experience, Category = "Attributes|Player|Progression", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Experience;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ExpForNextLevel, Category = "Attributes|Player|Progression", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData ExpForNextLevel;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Vitality, Category = "Attributes|Player|Perks", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Vitality;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category = "Attributes|Player|Perks", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Strength;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Accuracy, Category = "Attributes|Player|Perks", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Accuracy;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intelligence, Category = "Attributes|Player|Perks", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Intelligence;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mentality, Category = "Attributes|Player|Perks", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Mentality;
	
	float LevelBeforeAttributeChange;
	float ExperienceBeforeAttributeChange;
	float VitalityBeforeAttributeChange;
	float StrengthBeforeAttributeChange;
	float AccuracyBeforeAttributeChange;
	float IntelligenceBeforeAttributeChange;
	float MentalityBeforeAttributeChange;
	
	FActiveGameplayEffectHandle PerksPerLevelEffectHandle;
};

#undef UE_API