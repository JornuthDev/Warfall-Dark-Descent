#pragma once
#include "CoreMinimal.h"
#include "OnlineTypes.h"
#include "AbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct FInternalAttributeData
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes|Resistance")
	float Armor = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes|Resistance")
	float IceResistance = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes|Resistance")
	float FireResistance = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes|Resistance")
	float LightningResistance = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes|Resistance")
	float HolyResistance = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes|Resistance")
	float PoisonResistance = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes|Resistance")
	float BleedingResistance = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perks")
	float Vitality = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perks")
	float Strength = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perks")
	float Accuracy = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perks")
	float Intelligence = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perks")
	float Mentality = 0.f;
	
	// ═══ TODO PHASE 8+ : ÉQUIPEMENTS ET TALENTS ═══
	
	// UPROPERTY(SaveGame)
	// TArray<FEquipmentEntry> EquippedItems;
	
	// UPROPERTY(SaveGame)
	// TArray<FTalentEntry> ActiveTalents;
	
	// UPROPERTY(SaveGame)
	// TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;
	
};

USTRUCT(BlueprintType)
struct FDefaultArchetypeData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EClassArchetype Archetype;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInternalAttributeData Data;
};