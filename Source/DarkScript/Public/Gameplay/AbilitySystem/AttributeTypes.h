#pragma once
#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "Attributes/Mutual/HealthSet.h"
#include "Attributes/Resources/FocusSet.h"
#include "Attributes/Resources/ManaSet.h"
#include "Attributes/Resources/StaminaSet.h"
#include "Attributes/Mutual/PropertyInstigator.h"
#include "Attributes/Mutual/ResistancesSet.h"
#include "Attributes/PlayerSet.h"
#include "DarkGameplayTags.h"
#include "AttributeTypes.generated.h"

class UDefaultAttributeSet;

UENUM(BlueprintType)
enum class EResourceType : uint8
{
	None UMETA(DisplayName = "None"),
	Stamina UMETA(DisplayName = "Stamina"),
	Mana UMETA(DisplayName = "Mana"),
	Focus UMETA(DisplayName = "Focus"),
};

USTRUCT(BlueprintType)
struct FHealthAttribute
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1"))
	float HealthMax = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
	float HealthRegenRate = 0.01;

	static bool GetAttributeSetClass(TSubclassOf<UAttributeSet>& OutClass)
	{
		OutClass = UHealthSet::StaticClass();
		return true;
	}
};

USTRUCT(BlueprintType)
struct FRessourceAttribute
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EResourceType Type = EResourceType::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHide, EditCondition = "Type != EResourceType::None"), meta = (ClampMin = "0"))
	float ResourceOnStart = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHide, EditCondition = "Type != EResourceType::None"), meta = (ClampMin = "1"))
	float ResourceMax = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHide, EditCondition = "Type != EResourceType::None"), meta = (ClampMin = "0"))
	float ResourceRegenRate = 0.01;
	
	bool GetAttributeSetClass(TSubclassOf<UAttributeSet>& OutClass) const
	{
		switch (Type)
		{
		case EResourceType::None:
			return false;
		case EResourceType::Stamina:
			OutClass = UStaminaSet::StaticClass();
			return true;
		case EResourceType::Mana:
			OutClass = UManaSet::StaticClass();
			return true;
		case EResourceType::Focus:
			OutClass = UFocusSet::StaticClass();
			return true;
		default:
			return false;
		}
	}
};

USTRUCT(BlueprintType)
struct FPropertyCombat
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physic", meta = (ClampMin = "0", DisplayName = "DamageMultiplier (Melee)"))
	float PhysicDamageMultiplier = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physic", meta = (ClampMin = "0", DisplayName = "DamageMultiplier (Ranged)"))
	float RangedDamageMultiplier = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physic", meta = (ClampMin = "0", ClampMax = "1", DisplayName = "CriticalChance"))
	float PhysicCriticalChance = 0.1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physic", meta = (ClampMin = "0", DisplayName = "CriticalPower"))
	float PhysicCriticalPower = 1.25;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic", meta = (ClampMin = "0", DisplayName = "DamageMultiplier (Magic)"))
	float MagicDamageMultiplier = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic", meta = (ClampMin = "0", ClampMax = "1", DisplayName = "CriticalChance"))
	float MagicCriticalChance = 0.1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic", meta = (ClampMin = "0", DisplayName = "CriticalPower"))
	float MagicCriticalPower = 1.25;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic", meta = (ClampMin = "0", DisplayName = "HealingMultiplier"))
	float HealingMultiplier = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic", meta = (ClampMin = "0", ClampMax = "1", DisplayName = "CriticalChance"))
	float HealCriticalChance = 0.1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic", meta = (ClampMin = "0", DisplayName = "CriticalPower"))
	float HealCriticalPower = 1.25;

	static bool GetAttributeSetClass(TSubclassOf<UAttributeSet>& OutClass)
	{
		OutClass = UPropertyInstigator::StaticClass();
		return true;
	}
};

USTRUCT (BlueprintType)
struct FResistanceAttribute
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Armor = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "-100", ClampMax = "100"))
	float IceResistance = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "-100", ClampMax = "100"))
	float FireResistance = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "-100", ClampMax = "100"))
	float LightningResistance = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "-100", ClampMax = "100"))
	float HolyResistance = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "-100", ClampMax = "100"))
	float PoisonResistance = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "-100", ClampMax = "100"))
	float BleedingResistance = 0;
	
	static bool GetAttributeSetClass(TSubclassOf<UAttributeSet>& OutClass)
	{
		OutClass = UResistancesSet::StaticClass();
		return true;
	}
};

USTRUCT(BlueprintType)
struct FDefaultAttributeRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHealthAttribute Health;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRessourceAttribute Resource;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPropertyCombat Combat;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FResistanceAttribute Resistances;
	
	FDefaultAttributeRow() = default;
	
	void GetAllAttributeSets(TArray<TSubclassOf<UAttributeSet>>& OutAttributeSets) const
	{
		TSubclassOf<UAttributeSet> AttributeSetClass;
		
		if (Health.GetAttributeSetClass(AttributeSetClass))
		{
			OutAttributeSets.AddUnique(AttributeSetClass);
		}
		if (Resource.GetAttributeSetClass(AttributeSetClass))
		{
			OutAttributeSets.AddUnique(AttributeSetClass);
		}
		if (Combat.GetAttributeSetClass(AttributeSetClass))
		{
			OutAttributeSets.AddUnique(AttributeSetClass);
		}
		if (Resistances.GetAttributeSetClass(AttributeSetClass))
		{
			OutAttributeSets.AddUnique(AttributeSetClass);
		}
	}
};

USTRUCT(BlueprintType)
struct FPlayerAttribute
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, SaveGame)
	float Level = 1;
	UPROPERTY(BlueprintReadOnly, SaveGame)
	float Experience = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
	float VitalityPerLevel = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
	float StrengthPerLevel = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
	float AccuracyPerLevel = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
	float IntelligencePerLevel = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
	float MentalityPerLevel = 0;
	
	static bool GetAttributeSetClass(TSubclassOf<UAttributeSet>& OutClass)
	{
		OutClass = UPlayerSet::StaticClass();
		return true;
	}
};

USTRUCT(BlueprintType)
struct FAdvancedAttributeRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDefaultAttributeRow Default;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
	FPlayerAttribute Progression;
	
	TArray<TSubclassOf<UAttributeSet>> GetAllAttributeSets() const
	{
		TArray<TSubclassOf<UAttributeSet>> OutAttributeSets;
		Default.GetAllAttributeSets(OutAttributeSets);
		return OutAttributeSets;
	}
	TSubclassOf<UAttributeSet> GetProgressionAttributeSet() const
	{
		TSubclassOf<UAttributeSet> OutAttributeSet;
		Progression.GetAttributeSetClass(OutAttributeSet);
		return OutAttributeSet;
	}
};

USTRUCT(BlueprintType)
struct FDynamicEffectBasic
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttribute Attribute;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Magnitude = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasDuration = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bHasDuration == true", EditConditionHide))
	float Duration = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer Tags;
	
	EGameplayEffectDurationType GetPolicy(const bool bByPassToInstant = false) const
	{
		if (!bByPassToInstant)
		{
			return bHasDuration ? EGameplayEffectDurationType::HasDuration : EGameplayEffectDurationType::Infinite;
		}
		return EGameplayEffectDurationType::Instant;
	}
	
	FDynamicEffectBasic() = default;
	
	FDynamicEffectBasic(const FGameplayAttribute& InAttribute, const float InMagnitude,
		const bool bInHasDuration = false, const float InDuration = 1, const FGameplayTagContainer& InTags = FGameplayTagContainer())
		: Attribute(InAttribute)
		, Magnitude(InMagnitude)
		, bHasDuration(bInHasDuration)
		, Duration(InDuration)
		, Tags(InTags)
	{}
	

};

USTRUCT(BlueprintType)
struct FDynamicEffectArray
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FGameplayAttribute, TSubclassOf<UGameplayEffectCalculation>> Attributes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasDuration = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bHasDuration == true", EditConditionHide))
	float Duration = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer Tags;
	
	FDynamicEffectArray() = default;
		
	explicit FDynamicEffectArray(const TMap<FGameplayAttribute, TSubclassOf<UGameplayEffectCalculation>>& InAttributes, const bool InHasDuration = false,
		const float InDuration = 1, const FGameplayTagContainer& InTags = FGameplayTagContainer())
		: Attributes(InAttributes)
		, bHasDuration(InHasDuration)
		, Duration(InDuration)
		, Tags(InTags)
		{}
	
	EGameplayEffectDurationType GetPolicy(const bool bByPassToInstant = false) const
	{
		if (!bByPassToInstant)
		{
			return bHasDuration ? EGameplayEffectDurationType::HasDuration : EGameplayEffectDurationType::Infinite;
		}
		return EGameplayEffectDurationType::Instant;
	}
};

struct FDynamicEffect
{
private:
	bool bUseCalculation = false;
	FGameplayAttribute Attribute;
	EGameplayModOp::Type OperationType = EGameplayModOp::Additive;
	TMap<FGameplayAttribute, TSubclassOf<UGameplayEffectCalculation>> Attributes;
	FString UniqueName = "";
	FGameplayTagContainer Tags;
	
public:
	float Magnitude = 0;
	float Duration = 0;
	EGameplayEffectDurationType Policy;
	
	bool HasMagnitude() const { return Magnitude != 0; }
	
	bool IsValid() const 
	{ 
		if (!Attributes.IsEmpty())
		{
			for (const auto& It : Attributes)
			{
				if (!It.Key.IsValid() || It.Value == nullptr)
				{
					return false;
				}
			}
			return true;
		}
		return Attribute.IsValid();
	}
	
	FDynamicEffect() = default;

	explicit FDynamicEffect(const FDynamicEffectBasic& InEffect, const EGameplayModOp::Type InOperation, const bool InByPasseToInstant = false, const FName& EffectName = NAME_None)
		: Attribute(InEffect.Attribute)
		, OperationType(InOperation)
		, Tags(InEffect.Tags)
		, Magnitude(InEffect.Magnitude)
		, Duration(InEffect.Duration)
		, Policy(InEffect.GetPolicy(InByPasseToInstant))
	{}
	
	explicit FDynamicEffect(const FDynamicEffectArray& InEffects, const bool InByPasseToInstant = false, const FName& EffectName = NAME_None)
		: bUseCalculation(true)
		, Attributes(InEffects.Attributes)
		, UniqueName(EffectName.ToString())
		, Tags(InEffects.Tags)
		, Policy(InEffects.GetPolicy(InByPasseToInstant))
	{}
	
    // ✅ Créer un NOUVEL effet à chaque application
    UGameplayEffect* CreateFreshEffect()
    {
        if (!IsValid())
        {
            return nullptr;
        }
        
        // Nom unique pour debug
		if (UniqueName.IsEmpty())
		{
			UniqueName = FString::Printf(TEXT("DynEffect_%lld"), FPlatformTime::Cycles64());
		}
        UGameplayEffect* Effect = NewObject<UGameplayEffect>(
            GetTransientPackage(), 
            FName(*UniqueName)
        );
        
        Effect->DurationPolicy = Policy;
        Effect->CachedAssetTags = Tags;
		
        // Configuration de la durée
        if (Policy == EGameplayEffectDurationType::HasDuration)
        {
            FSetByCallerFloat SetByCallerDuration;
            SetByCallerDuration.DataTag = DarkGameplayTags::SetByCaller_Duration;
            Effect->DurationMagnitude = FGameplayEffectModifierMagnitude(SetByCallerDuration);
        }
        
        if (bUseCalculation)
        {
            // ✅ Modifiers avec calcul custom
            for (auto& Pair : Attributes)
            {
            	FGameplayAttribute& Attr = Pair.Key;
                if (!Attr.IsValid()) continue;
                
                const int32 Idx = Effect->Modifiers.Num();
                Effect->Modifiers.SetNum(Idx + 1);
                
            	if (Pair.Value && Pair.Value->IsChildOf(UGameplayModMagnitudeCalculation::StaticClass()))
            	{
            		FGameplayModifierInfo& ModifierInfo = Effect->Modifiers[Idx];
            		ModifierInfo.Attribute = Attr;
            		ModifierInfo.ModifierOp = OperationType;
                
            		FCustomCalculationBasedFloat CalculationBased;
            		CalculationBased.CalculationClassMagnitude = Pair.Value;
            		ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(CalculationBased);
            		UE_LOG(LogTemp, Warning, TEXT("Create for %s new modifier to %s attribute"), *UniqueName, *Attr.AttributeName);
            	}
            }
        }
        else
        {
            // ✅ Modifier simple avec SetByCaller
            const int32 Idx = Effect->Modifiers.Num();
            Effect->Modifiers.SetNum(Idx + 1);
            
            FGameplayModifierInfo& ModifierInfo = Effect->Modifiers[Idx];
            ModifierInfo.Attribute = Attribute;
            ModifierInfo.ModifierOp = OperationType;
            
            FSetByCallerFloat SetByCallerMagnitude;
            SetByCallerMagnitude.DataTag = DarkGameplayTags::SetByCaller_Modifier;
            ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCallerMagnitude);
        }
        
        return Effect;
    }
    
    // ✅ Vérifier si l'effet peut être appliqué à la cible
    bool CanApplyTo(const UAbilitySystemComponent* TargetASC) const
    {
        if (!TargetASC || !IsValid())
        {
            return false;
        }
		if (bUseCalculation)
		{
			if (IsValid())
			{
				for (auto& Pair : Attributes)
				{
					if (!TargetASC->HasAttributeSetForAttribute(Pair.Key))
					{
						UE_LOG(LogTemp, Warning, TEXT("CanApplyTo:: %s HasNotAttributSetForAttribute %s"), *TargetASC->GetName(), *Pair.Key.AttributeName);
						return false;
					}
				}
				return true;
			}
			return false;
		}
        // Pour les modifiers simples
        return TargetASC->HasAttributeSetForAttribute(Attribute);
    }
};