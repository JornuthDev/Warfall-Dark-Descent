#pragma once
#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DamagesAttributSet.generated.h"

// ═══════════════════════════════════════════════════════════════════════════
// MACROS HELPER POUR LES ATTRIBUTS
// ═══════════════════════════════════════════════════════════════════════════

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class DARKSCRIPT_API UDamagesAttributSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	// ═══════════════════════════════════════════════════════════════════════
	// CONSTRUCTEUR
	// ═══════════════════════════════════════════════════════════════════════
	
	UDamagesAttributSet();
	void Init(const FName RowName);

protected:
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Overrides)
	// ═══════════════════════════════════════════════════════════════════════
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Helpers)
	// ═══════════════════════════════════════════════════════════════════════
	
	void SetAttribute(const FGameplayAttribute& Attribute, const float NewValue) const;
	
public:
	float GetFinalDamage(const FGameplayTag& IncomingDamage) const;
	
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════
	
	// Data des attributes de l'arme
		

	// ═══════════════════════════════════════════════════════════════════════
	// BASE ATTRIBUTES (Valeurs de base depuis DataTable)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Valeur pour Damage (Depuis DataTable) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Damage, Category = " Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UDamagesAttributSet, Damage)
	
	UFUNCTION()
	virtual void OnRep_Damage(FGameplayAttributeData& OldDamage);
	
	/** Valeur pour FleshMultiplier (Depuis DataTable) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FleshDamageMultiplier, Category = " Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData FleshDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UDamagesAttributSet, FleshDamageMultiplier)
	
	UFUNCTION()
	virtual void OnRep_FleshDamageMultiplier(FGameplayAttributeData& OldFleshDamageMultiplier);
	
	/** Valeur pour PenetrationDamageMultiplier (Depuis DataTable) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PenetrationDamageMultiplier, Category = " Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData PenetrationDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UDamagesAttributSet, PenetrationDamageMultiplier)
	
	UFUNCTION()
	virtual void OnRep_PenetrationDamageMultiplier(FGameplayAttributeData& OldPenetrationDamageMultiplier);
	
	/** Valeur pour IceDamageMultiplier (Depuis DataTable) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IceDamageMultiplier, Category = " Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData IceDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UDamagesAttributSet, IceDamageMultiplier)
	
	UFUNCTION()
	virtual void OnRep_IceDamageMultiplier(FGameplayAttributeData& OldIceDamageMultiplier);
	
	/** Valeur pour FireDamageMultiplier (Depuis DataTable) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FireDamageMultiplier, Category = " Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData FireDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UDamagesAttributSet, FireDamageMultiplier)
	
	UFUNCTION()
	virtual void OnRep_FireDamageMultiplier(FGameplayAttributeData& OldFireDamageMultiplier);
	
	/** Valeur pour LightningDamageMultiplier (Depuis DataTable) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LightningDamageMultiplier, Category = " Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData LightningDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UDamagesAttributSet, LightningDamageMultiplier)
	
	UFUNCTION()
	virtual void OnRep_LightningDamageMultiplier(FGameplayAttributeData& OldLightningDamageMultiplier);
	
	/** Valeur pour HolyDamageMultiplier (Depuis DataTable) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HolyDamageMultiplier, Category = " Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData HolyDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UDamagesAttributSet, HolyDamageMultiplier)
	
	UFUNCTION()
	virtual void OnRep_HolyDamageMultiplier(FGameplayAttributeData& OldHolyDamageMultiplier);
	
	/** Valeur pour PoisonDamageMultiplier (Depuis DataTable) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PoisonDamageMultiplier, Category = " Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData PoisonDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UDamagesAttributSet, PoisonDamageMultiplier)
	
	UFUNCTION()
	virtual void OnRep_PoisonDamageMultiplier(FGameplayAttributeData& OldPoisonDamageMultiplier);
	
	/** Valeur pour BleedingDamageMultiplier (Depuis DataTable) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BleedingDamageMultiplier, Category = " Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData BleedingDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UDamagesAttributSet, BleedingDamageMultiplier)
	
	UFUNCTION()
	virtual void OnRep_BleedingDamageMultiplier(FGameplayAttributeData& OldBleedingDamageMultiplier);
	
	/** Valeur pour RangedDamageMultiplier (Depuis DataTable) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RangedDamageMultiplier, Category = " Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData RangedDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UDamagesAttributSet, RangedDamageMultiplier)
	
	UFUNCTION()
	virtual void OnRep_RangedDamageMultiplier(FGameplayAttributeData& OldRangedDamageMultiplier);
	
	/** Valeur pour ArmorPenetration (Depuis DataTable) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorPenetration, Category = " Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData ArmorPenetration;
	ATTRIBUTE_ACCESSORS(UDamagesAttributSet, ArmorPenetration)
	
	UFUNCTION()
	virtual void OnRep_ArmorPenetration(FGameplayAttributeData& OldArmorPenetration);
};
