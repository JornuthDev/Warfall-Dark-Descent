// DarkAttributeSet.h
#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Utils/Types/PlayerTypes.h"
#include "DarkAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * AttributeSet pour tous les personnages (joueurs + ennemis)
 * Contient TOUTES les stats gérées par GAS
 */
UCLASS()
class DARKSCRIPT_API UDarkAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    UDarkAttributeSet();

    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // ═══════════════════════════════════════════════════════════════
    // HEALTH
    // ═══════════════════════════════════════════════════════════════

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Attributes|Health")
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UDarkAttributeSet, Health)

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Attributes|Health")
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UDarkAttributeSet, MaxHealth)

    UFUNCTION()
    virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

    UFUNCTION()
    virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

    // ═══════════════════════════════════════════════════════════════
    // COMBAT STATS
    // ═══════════════════════════════════════════════════════════════

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Damage, Category = "Attributes|Combat")
    FGameplayAttributeData Damage;
    ATTRIBUTE_ACCESSORS(UDarkAttributeSet, Damage)

    UFUNCTION()
    virtual void OnRep_Damage(const FGameplayAttributeData& OldDamage);

    // ═══════════════════════════════════════════════════════════════
    // RESISTANCES (Armor + Élémentaires)
    // ═══════════════════════════════════════════════════════════════

    /** Armor (réduit dégâts Flesh) */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Attributes|Resistances")
    FGameplayAttributeData Armor;
    ATTRIBUTE_ACCESSORS(UDarkAttributeSet, Armor)

    /** Ice Resistance (-100 à +100, négatif = faiblesse) */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IceResistance, Category = "Attributes|Resistances")
    FGameplayAttributeData IceResistance;
    ATTRIBUTE_ACCESSORS(UDarkAttributeSet, IceResistance)

    /** Fire Resistance */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FireResistance, Category = "Attributes|Resistances")
    FGameplayAttributeData FireResistance;
    ATTRIBUTE_ACCESSORS(UDarkAttributeSet, FireResistance)

    /** Lightning Resistance */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LightningResistance, Category = "Attributes|Resistances")
    FGameplayAttributeData LightningResistance;
    ATTRIBUTE_ACCESSORS(UDarkAttributeSet, LightningResistance)

    /** Holy Resistance */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HolyResistance, Category = "Attributes|Resistances")
    FGameplayAttributeData HolyResistance;
    ATTRIBUTE_ACCESSORS(UDarkAttributeSet, HolyResistance)

    UFUNCTION()
    virtual void OnRep_Armor(const FGameplayAttributeData& OldArmor);

    UFUNCTION()
    virtual void OnRep_IceResistance(const FGameplayAttributeData& OldIceResistance);

    UFUNCTION()
    virtual void OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance);

    UFUNCTION()
    virtual void OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance);

    UFUNCTION()
    virtual void OnRep_HolyResistance(const FGameplayAttributeData& OldHolyResistance);

    // ═══════════════════════════════════════════════════════════════
    // MOVEMENT
    // ═══════════════════════════════════════════════════════════════

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MoveSpeed, Category = "Attributes|Movement")
    FGameplayAttributeData MoveSpeed;
    ATTRIBUTE_ACCESSORS(UDarkAttributeSet, MoveSpeed)

    UFUNCTION()
    virtual void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);

    // ═══════════════════════════════════════════════════════════════
    // RESOURCE SYSTEM
    // ═══════════════════════════════════════════════════════════════

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Resource, Category = "Attributes|Resource")
    FGameplayAttributeData Resource;
    ATTRIBUTE_ACCESSORS(UDarkAttributeSet, Resource)

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxResource, Category = "Attributes|Resource")
    FGameplayAttributeData MaxResource;
    ATTRIBUTE_ACCESSORS(UDarkAttributeSet, MaxResource)

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ResourceRegenRate, Category = "Attributes|Resource")
    FGameplayAttributeData ResourceRegenRate;
    ATTRIBUTE_ACCESSORS(UDarkAttributeSet, ResourceRegenRate)

    UFUNCTION()
    virtual void OnRep_Resource(const FGameplayAttributeData& OldResource);

    UFUNCTION()
    virtual void OnRep_MaxResource(const FGameplayAttributeData& OldMaxResource);

    UFUNCTION()
    virtual void OnRep_ResourceRegenRate(const FGameplayAttributeData& OldResourceRegenRate);

    // ═══════════════════════════════════════════════════════════════
    // META ATTRIBUTES (pour les calculs, ne sont PAS repliquées)
    // ═══════════════════════════════════════════════════════════════

    /** Incoming damage (temporary, used in PostGameplayEffectExecute) */
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Meta")
    FGameplayAttributeData IncomingDamage;
    ATTRIBUTE_ACCESSORS(UDarkAttributeSet, IncomingDamage)

    /** Incoming healing (temporary, used in PostGameplayEffectExecute) */
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Meta")
    FGameplayAttributeData IncomingHealing;
    ATTRIBUTE_ACCESSORS(UDarkAttributeSet, IncomingHealing)

    /** Pending damage type (stocké temporairement pour le calcul) */
    EDamageType PendingDamageType = EDamageType::None;

protected:
    /** Helper: Clamp attribute between min and max */
    void ClampAttribute(const FGameplayAttribute& Attribute, float MinValue, float MaxValue) const;
};