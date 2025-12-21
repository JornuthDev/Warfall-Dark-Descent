#pragma once
#include "CoreMinimal.h"
#include "Attributes/Mutual/HealthSet.h"
#include "Attributes/Resources/FocusSet.h"
#include "Attributes/Resources/ManaSet.h"
#include "Attributes/Resources/StaminaSet.h"
#include "Attributes/Mutual/PropertyInstigator.h"
#include "Attributes/Mutual/ResistancesSet.h"
#include "Attributes/PlayerSet.h"
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