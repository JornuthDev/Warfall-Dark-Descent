#pragma once
#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Utils/Types/PlayerTypes.h"
#include "DefaultAttributeSet.generated.h"

// ═══════════════════════════════════════════════════════════════════════════
// STRUCTURES DES ATTRIBUTES COMMUNS
// ═══════════════════════════════════════════════════════════════════════════

USTRUCT(BlueprintType)
struct FAttributeDataRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes")
	float Health = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes", meta=(ClampMin = 0, ClampMax = 1))
	float HealthRegenRate = 0.01f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes")
	float Mana = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes", meta=(ClampMin = 0, ClampMax = 1))
	float ManaRegenRate = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes")
	float Stamina = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes", meta=(ClampMin = 0, ClampMax = 1))
	float StaminaRegenRate = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes")
	float Focus = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes", meta=(ClampMin = 0, ClampMax = 1))
	float FocusRegenRate = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes|Attack", meta=(ClampMin = 1))
	float PhysicDamageMultiplier = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes|Attack", meta=(ClampMin = 1))
	float MagicDamageMultiplier = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes|Attack", meta=(ClampMin = 1))
	float AccuracyDamageMultiplier = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes|Attack", meta=(ClampMin = 0, ClampMax = 1))
	float CriticalChance = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes|Attack", meta=(ClampMin = 0))
	float CriticalDamage = 1.25f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes|Attack", meta=(ClampMin = 0))
	float CriticalHeal = 1.25;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes|Resistance", meta=(ClampMin = 0))
	float Armor = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes|Resistance", meta=(ClampMin = -100, ClampMax = 100))
	float IceResistance = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes|Resistance", meta=(ClampMin = -100, ClampMax = 100))
	float FireResistance = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes|Resistance", meta=(ClampMin = -100, ClampMax = 100))
	float LightningResistance = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes|Resistance", meta=(ClampMin = -100, ClampMax = 100))
	float HolyResistance = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes|Resistance", meta=(ClampMin = -100, ClampMax = 100))
	float PoisonResistance = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes|Resistance", meta=(ClampMin = -100, ClampMax = 100))
	float BleedingResistance = 0.f;
};

// ═══════════════════════════════════════════════════════════════════════════
// MACROS HELPER POUR LES ATTRIBUTS
// ═══════════════════════════════════════════════════════════════════════════

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

// ═══════════════════════════════════════════════════════════════════════════
// CLASSE : DEFAULT ATTRIBUTE SET
// ═══════════════════════════════════════════════════════════════════════════

/**
 * AttributeSet pour les données SPÉCIFIQUES
 * 
 * Attaché au Character via DefaultAbilitySystem
 * Ces attributs sont RÉPLIQUÉS et UNIQUES pour chaque data
 */
UCLASS()
class DARKSCRIPT_API UDefaultAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	// ═══════════════════════════════════════════════════════════════════════
	// CONSTRUCTEUR
	// ═══════════════════════════════════════════════════════════════════════
	
	UDefaultAttributeSet();
	virtual void Init(const EClassArchetype Archetype = EClassArchetype::None, const FName RowName = NAME_None);
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Static)
	// ═══════════════════════════════════════════════════════════════════════
	
	static FAttributeDataRow* GetAttributeData(const FName RowName);
	
	// ═══════════════════════════════════════════════════════════════════════
	// CALCUL
	// ═══════════════════════════════════════════════════════════════════════
	
	float ConvertDamages(const FGameplayEffectModCallbackData& Data, float Amount) const;
	void EmpoweredMagnitude(FGameplayEffectSpec& Spec, const FGameplayTag& DamageTypeTag, bool bIsCritic) const;
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Helpers)
	// ═══════════════════════════════════════════════════════════════════════
	
	void SetAttribute(const FGameplayAttribute& Attribute, const float Value) const;
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Overrides)
	// ═══════════════════════════════════════════════════════════════════════
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════
	
	const FAttributeDataRow* AttributeData;
	
	// ═══════════════════════════════════════════════════════════════════════
	// BASE ATTRIBUTES (Valeurs de base depuis DataTable)
	// ═══════════════════════════════════════════════════════════════════════

	/** Valeur de base pour Health (depuis DataTable) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHealth, Category = "Base Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData BaseHealth;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, BaseHealth)

	UFUNCTION()
	virtual void OnRep_BaseHealth(const FGameplayAttributeData& OldBaseHealth);

	/** Valeur de base pour Mana (depuis DataTable) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseMana, Category = "Base Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData BaseMana;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, BaseMana)

	UFUNCTION()
	virtual void OnRep_BaseMana(const FGameplayAttributeData& OldBaseMana);

	/** Valeur de base pour Stamina (depuis DataTable) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseStamina, Category = "Base Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData BaseStamina;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, BaseStamina)

	UFUNCTION()
	virtual void OnRep_BaseStamina(const FGameplayAttributeData& OldBaseStamina);

	/** Valeur de base pour Focus (depuis DataTable) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseFocus, Category = "Base Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData BaseFocus;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, BaseFocus)

	UFUNCTION()
	virtual void OnRep_BaseFocus(const FGameplayAttributeData& OldBaseFocus);

	/** Taux de régénération de base pour Health */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHealthRegenRate, Category = "Base Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData BaseHealthRegenRate;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, BaseHealthRegenRate)

	UFUNCTION()
	virtual void OnRep_BaseHealthRegenRate(const FGameplayAttributeData& OldBaseHealthRegenRate);

	/** Taux de régénération de base pour Mana */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseManaRegenRate, Category = "Base Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData BaseManaRegenRate;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, BaseManaRegenRate)

	UFUNCTION()
	virtual void OnRep_BaseManaRegenRate(const FGameplayAttributeData& OldBaseManaRegenRate);

	/** Taux de régénération de base pour Stamina */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseStaminaRegenRate, Category = "Base Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData BaseStaminaRegenRate;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, BaseStaminaRegenRate)

	UFUNCTION()
	virtual void OnRep_BaseStaminaRegenRate(const FGameplayAttributeData& OldBaseStaminaRegenRate);

	/** Taux de régénération de base pour Focus */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseFocusRegenRate, Category = "Base Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData BaseFocusRegenRate;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, BaseFocusRegenRate)

	UFUNCTION()
	virtual void OnRep_BaseFocusRegenRate(const FGameplayAttributeData& OldBaseFocusRegenRate);

	/** Multiplicateur de dégâts physiques de base */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BasePhysicDamageMultiplier, Category = "Base Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData BasePhysicDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, BasePhysicDamageMultiplier)

	UFUNCTION()
	virtual void OnRep_BasePhysicDamageMultiplier(const FGameplayAttributeData& OldBasePhysicDamageMultiplier);

	/** Multiplicateur de dégâts magiques de base */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseMagicDamageMultiplier, Category = "Base Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData BaseMagicDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, BaseMagicDamageMultiplier)

	UFUNCTION()
	virtual void OnRep_BaseMagicDamageMultiplier(const FGameplayAttributeData& OldBaseMagicDamageMultiplier);

	/** Multiplicateur de dégâts de précision de base */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseAccuracyDamageMultiplier, Category = "Base Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData BaseAccuracyDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, BaseAccuracyDamageMultiplier)

	UFUNCTION()
	virtual void OnRep_BaseAccuracyDamageMultiplier(const FGameplayAttributeData& OldBaseAccuracyDamageMultiplier);

	/** Chance de coup critique de base */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseCriticalChance, Category = "Base Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData BaseCriticalChance;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, BaseCriticalChance)

	UFUNCTION()
	virtual void OnRep_BaseCriticalChance(const FGameplayAttributeData& OldBaseCriticalChance);

	/** Dégâts critiques de base */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseCriticalDamage, Category = "Base Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData BaseCriticalDamage;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, BaseCriticalDamage)

	UFUNCTION()
	virtual void OnRep_BaseCriticalDamage(const FGameplayAttributeData& OldBaseCriticalDamage);
	
	/** Heal critiques de base */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseCriticalHeal, Category = "Base Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData BaseCriticalHeal;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, BaseCriticalHeal)

	UFUNCTION()
	virtual void OnRep_BaseCriticalHeal(const FGameplayAttributeData& OldBaseCriticalHeal);

	// ═══════════════════════════════════════════════════════════════════════
	// METADATA : APPLY DAMAGES (Points de dégâts à appliquer)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de Dégâts à appliquer */
	UPROPERTY(EditDefaultsOnly, Category = "MetaData")
	FGameplayAttributeData ApplyDamages;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, ApplyDamages)
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS : HEALTH (Points de vie)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de vie actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, Health)

	/** Callback de réplication pour Health */
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	/** Points de vie rendu par seconde */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegenRate, Category = "Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData HealthRegenRate;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, HealthRegenRate)

	/** Callback de réplication pour HealthRegenRate */
	UFUNCTION()
	virtual void OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate);

	/** RÃ©gÃ©nÃ©ration de santÃ© bonus (Ã©quipement/buffs) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegenBonus, Category = "Attributes")
	FGameplayAttributeData HealthRegenBonus;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, HealthRegenBonus)

	/** Callback de rÃ©plication pour HealthRegenBonus */
	UFUNCTION()
	virtual void OnRep_HealthRegenBonus(const FGameplayAttributeData& OldHealthRegenBonus);
	
	/** Points de vie maximums (valeur finale calculée) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, MaxHealth)

	/** Callback de réplication pour MaxHealth */
	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	/** Points de vie bonus (équipement/buffs) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthBonus, Category = "Attributes")
	FGameplayAttributeData HealthBonus;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, HealthBonus)

	/** Callback de réplication pour HealthBonus */
	UFUNCTION()
	virtual void OnRep_HealthBonus(const FGameplayAttributeData& OldHealthBonus);

	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS : MANA (Points de mana)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de mana actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, Mana)

	/** Callback de réplication pour Mana */
	UFUNCTION()
	virtual void OnRep_Mana(const FGameplayAttributeData& OldMana);
	
	/** Points de mana rendu par seconde */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ManaRegenRate, Category = "Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData ManaRegenRate;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, ManaRegenRate)

	/** Callback de réplication pour ManaRegenRate */
	UFUNCTION()
	virtual void OnRep_ManaRegenRate(const FGameplayAttributeData& OldManaRegenRate);

	/** RÃ©gÃ©nÃ©ration de mana bonus (Ã©quipement/buffs) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ManaRegenBonus, Category = "Attributes")
	FGameplayAttributeData ManaRegenBonus;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, ManaRegenBonus)

	/** Callback de rÃ©plication pour ManaRegenBonus */
	UFUNCTION()
	virtual void OnRep_ManaRegenBonus(const FGameplayAttributeData& OldManaRegenBonus);
	
	/** Points de mana maximums */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, MaxMana)

	/** Callback de réplication pour MaxMana */
	UFUNCTION()
	virtual void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);

	/** Points de mana bonus (équipement/buffs) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ManaBonus, Category = "Attributes")
	FGameplayAttributeData ManaBonus;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, ManaBonus)

	/** Callback de réplication pour ManaBonus */
	UFUNCTION()
	virtual void OnRep_ManaBonus(const FGameplayAttributeData& OldManaBonus);

	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS : STAMINA (Points de stamina)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de stamina actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stamina, Category = "Attributes")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, Stamina)

	/** Callback de réplication pour Stamina */
	UFUNCTION()
	virtual void OnRep_Stamina(const FGameplayAttributeData& OldStamina);

	/** Points de stamina rendu par seconde */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_StaminaRegenRate, Category = "Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData StaminaRegenRate;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, StaminaRegenRate)

	/** Callback de réplication pour StaminaRegenRate */
	UFUNCTION()
	virtual void OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate);

	/** RÃ©gÃ©nÃ©ration de stamina bonus (Ã©quipement/buffs) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_StaminaRegenBonus, Category = "Attributes")
	FGameplayAttributeData StaminaRegenBonus;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, StaminaRegenBonus)

	/** Callback de rÃ©plication pour StaminaRegenBonus */
	UFUNCTION()
	virtual void OnRep_StaminaRegenBonus(const FGameplayAttributeData& OldStaminaRegenBonus);
	
	/** Points de Stamina maximums */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxStamina, Category = "Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, MaxStamina)

	/** Callback de réplication pour MaxStamina */
	UFUNCTION()
	virtual void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);

	/** Points de stamina bonus (équipement/buffs) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_StaminaBonus, Category = "Attributes")
	FGameplayAttributeData StaminaBonus;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, StaminaBonus)

	/** Callback de réplication pour StaminaBonus */
	UFUNCTION()
	virtual void OnRep_StaminaBonus(const FGameplayAttributeData& OldStaminaBonus);

	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS : FOCUS (Points de focus)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de Focus actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Focus, Category = "Attributes")
	FGameplayAttributeData Focus;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, Focus)

	/** Callback de réplication pour Focus */
	UFUNCTION()
	virtual void OnRep_Focus(const FGameplayAttributeData& OldFocus);

	/** Points de focus rendu par seconde */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FocusRegenRate, Category = "Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData FocusRegenRate;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, FocusRegenRate)

	/** Callback de réplication pour FocusRegenRate */
	UFUNCTION()
	virtual void OnRep_FocusRegenRate(const FGameplayAttributeData& OldFocusRegenRate);

	/** RÃ©gÃ©nÃ©ration de focus bonus (Ã©quipement/buffs) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FocusRegenBonus, Category = "Attributes")
	FGameplayAttributeData FocusRegenBonus;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, FocusRegenBonus)

	/** Callback de rÃ©plication pour FocusRegenBonus */
	UFUNCTION()
	virtual void OnRep_FocusRegenBonus(const FGameplayAttributeData& OldFocusRegenBonus);
	
	/** Points de focus maximums */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxFocus, Category = "Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData MaxFocus;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, MaxFocus)

	/** Callback de réplication pour MaxFocus */
	UFUNCTION()
	virtual void OnRep_MaxFocus(const FGameplayAttributeData& OldMaxFocus);

	/** Points de focus bonus (équipement/buffs) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FocusBonus, Category = "Attributes")
	FGameplayAttributeData FocusBonus;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, FocusBonus)

	/** Callback de réplication pour FocusBonus */
	UFUNCTION()
	virtual void OnRep_FocusBonus(const FGameplayAttributeData& OldFocusBonus);

	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|ATTACK : PHYSIC DAMAGE MULTIPLIER (Points de Physic Damage Multiplier)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de PhysicDamageMultiplier actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicDamageMultiplier, Category = "Attributes|Attack", meta=(HideInDetailsView))
	FGameplayAttributeData PhysicDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, PhysicDamageMultiplier)

	/** Callback de réplication pour PhysicDamageMultiplier */
	UFUNCTION()
	virtual void OnRep_PhysicDamageMultiplier(const FGameplayAttributeData& OldPhysicDamageMultiplier);

	/** DÃ©gÃ¢ts physiques bonus (Ã©quipement/buffs) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicDamageBonus, Category = "Attributes|Attack")
	FGameplayAttributeData PhysicDamageBonus;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, PhysicDamageBonus)

	/** Callback de rÃ©plication pour PhysicDamageBonus */
	UFUNCTION()
	virtual void OnRep_PhysicDamageBonus(const FGameplayAttributeData& OldPhysicDamageBonus);
		
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|ATTACK : MAGIC DAMAGE MULTIPLIER (Points de Magic Damage Multiplier)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de MagicDamageMultiplier actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicDamageMultiplier, Category = "Attributes|Attack", meta=(HideInDetailsView))
	FGameplayAttributeData MagicDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, MagicDamageMultiplier)

	/** Callback de réplication pour MagicDamageMultiplier */
	UFUNCTION()
	virtual void OnRep_MagicDamageMultiplier(const FGameplayAttributeData& OldMagicDamageMultiplier);

	/** DÃ©gÃ¢ts magiques bonus (Ã©quipement/buffs) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicDamageBonus, Category = "Attributes|Attack")
	FGameplayAttributeData MagicDamageBonus;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, MagicDamageBonus)

	/** Callback de rÃ©plication pour MagicDamageBonus */
	UFUNCTION()
	virtual void OnRep_MagicDamageBonus(const FGameplayAttributeData& OldMagicDamageBonus);
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|ATTACK : ACCURACY DAMAGE MULTIPLIER (Points de Accuracy Damage Multiplier)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de AccuracyDamageMultiplier actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AccuracyDamageMultiplier, Category = "Attributes|Attack", meta=(HideInDetailsView))
	FGameplayAttributeData AccuracyDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, AccuracyDamageMultiplier)

	/** Callback de réplication pour AccuracyDamageMultiplier */
	UFUNCTION()
	virtual void OnRep_AccuracyDamageMultiplier(const FGameplayAttributeData& OldAccuracyDamageMultiplier);

	/** DÃ©gÃ¢ts de prÃ©cision bonus (Ã©quipement/buffs) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AccuracyDamageBonus, Category = "Attributes|Attack")
	FGameplayAttributeData AccuracyDamageBonus;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, AccuracyDamageBonus)

	/** Callback de rÃ©plication pour AccuracyDamageBonus */
	UFUNCTION()
	virtual void OnRep_AccuracyDamageBonus(const FGameplayAttributeData& OldAccuracyDamageBonus);
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|ATTACK : CRITICAL CHANCE (Points de Critical Chance)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de CriticalChance actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalChance, Category = "Attributes|Attack", meta=(HideInDetailsView))
	FGameplayAttributeData CriticalChance;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, CriticalChance)

	/** Callback de réplication pour CriticalChance */
	UFUNCTION()
	virtual void OnRep_CriticalChance(const FGameplayAttributeData& OldCriticalChance);

	/** Chance critique bonus (Ã©quipement/buffs) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalChanceBonus, Category = "Attributes|Attack")
	FGameplayAttributeData CriticalChanceBonus;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, CriticalChanceBonus)

	/** Callback de rÃ©plication pour CriticalChanceBonus */
	UFUNCTION()
	virtual void OnRep_CriticalChanceBonus(const FGameplayAttributeData& OldCriticalChanceBonus);
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|ATTACK : CRITICAL DAMAGE (Points de Critical Damage)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de CriticalDamage actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalDamage, Category = "Attributes|Attack", meta=(HideInDetailsView))
	FGameplayAttributeData CriticalDamage;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, CriticalDamage)

	/** Callback de réplication pour CriticalDamage */
	UFUNCTION()
	virtual void OnRep_CriticalDamage(const FGameplayAttributeData& OldCriticalDamage);

	/** DÃ©gÃ¢ts critiques bonus (Ã©quipement/buffs) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalDamageBonus, Category = "Attributes|Attack")
	FGameplayAttributeData CriticalDamageBonus;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, CriticalDamageBonus)

	/** Callback de rÃ©plication pour CriticalDamageBonus */
	UFUNCTION()
	virtual void OnRep_CriticalDamageBonus(const FGameplayAttributeData& OldCriticalDamageBonus);
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|ATTACK : CRITICAL HEAL (Points de Critical Heal)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de CriticalHeal actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHeal, Category = "Attributes|Attack", meta=(HideInDetailsView))
	FGameplayAttributeData CriticalHeal;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, CriticalHeal)

	/** Callback de réplication pour CriticalHeal */
	UFUNCTION()
	virtual void OnRep_CriticalHeal(const FGameplayAttributeData& OldCriticalHeal);

	/** DÃ©gÃ¢ts critiques bonus (Ã©quipement/buffs) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHealBonus, Category = "Attributes|Attack")
	FGameplayAttributeData CriticalHealBonus;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, CriticalHealBonus)

	/** Callback de rÃ©plication pour CriticalDamageBonus */
	UFUNCTION()
	virtual void OnRep_CriticalHealBonus(const FGameplayAttributeData& OldCriticalHealBonus);
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|RESISTANCE : ARMOR (Points de Armor)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de IceResistance actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Attributes|Resistance")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, Armor)

	/** Callback de réplication pour IceResistance */
	UFUNCTION()
	virtual void OnRep_Armor(const FGameplayAttributeData& OldArmor);
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|RESISTANCE : ICE RESISTANCE (Points de Ice Resistance)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de IceResistance actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IceResistance, Category = "Attributes|Resistance")
	FGameplayAttributeData IceResistance;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, IceResistance)

	/** Callback de réplication pour IceResistance */
	UFUNCTION()
	virtual void OnRep_IceResistance(const FGameplayAttributeData& OldIceResistance);
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|RESISTANCE : FIRE RESISTANCE (Points de Fire Resistance)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de FireResistance actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FireResistance, Category = "Attributes|Resistance")
	FGameplayAttributeData FireResistance;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, FireResistance)

	/** Callback de réplication pour FireResistance */
	UFUNCTION()
	virtual void OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance);
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|RESISTANCE : LIGHTNING RESISTANCE (Points de Lightning Resistance)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de LightningResistance actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LightningResistance, Category = "Attributes|Resistance")
	FGameplayAttributeData LightningResistance;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, LightningResistance)

	/** Callback de réplication pour LightningResistance */
	UFUNCTION()
	virtual void OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance);
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|RESISTANCE : HOLY RESISTANCE (Points de Holy Resistance)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de HolyResistance actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HolyResistance, Category = "Attributes|Resistance")
	FGameplayAttributeData HolyResistance;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, HolyResistance)

	/** Callback de réplication pour HolyResistance */
	UFUNCTION()
	virtual void OnRep_HolyResistance(const FGameplayAttributeData& OldHolyResistance);
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|RESISTANCE : POISON RESISTANCE (Points de Poison Resistance)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de PoisonResistance actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PoisonResistance, Category = "Attributes|Resistance")
	FGameplayAttributeData PoisonResistance;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, PoisonResistance)

	/** Callback de réplication pour PoisonResistance */
	UFUNCTION()
	virtual void OnRep_PoisonResistance(const FGameplayAttributeData& OldPoisonResistance);
	
	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTS|RESISTANCE : BLEEDING RESISTANCE (Points de Bleeding Resistance)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de BleedingResistance actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BleedingResistance, Category = "Attributes|Resistance")
	FGameplayAttributeData BleedingResistance;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, BleedingResistance)

	/** Callback de réplication pour BleedingResistance */
	UFUNCTION()
	virtual void OnRep_BleedingResistance(const FGameplayAttributeData& OldBleedingResistance);
	
};
