#pragma once
#include "CoreMinimal.h"
#include "DefaultAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Utils/Helpers/SystemsHelper.h"
#include "AdvancedAttributeSet.generated.h"

// ═══════════════════════════════════════════════════════════════════════════
// STRUCTURES DES ATTRIBUTES AVANCEES
// ═══════════════════════════════════════════════════════════════════════════

USTRUCT(BlueprintType)
struct FArchetypeAttributeRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EClassArchetype Archetype = EClassArchetype::Tank;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes")
	FName AttributesRow;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perks")
	float Vitality;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perks")
	float Strength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perks")
	float Accuracy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perks")
	float Intelligence;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perks")
	float Mentality;
	
	FArchetypeAttributeRow()
		:AttributesRow(NAME_None)
		,Vitality(0.f)
		,Strength(0.f)
		,Accuracy(0.f)
		,Intelligence(0.f)
		,Mentality(0.f)
	{}
	
	FAttributeDataRow* GetAttributeData() const
	{
		const UDataTable* Table = Content::GetTable(Tables::Attributes);
		if (!Table) return nullptr;
		const FString Ctx(TEXT("Looking_For_Data"));
	
		if (FAttributeDataRow* Direct = Table->FindRow<FAttributeDataRow>(AttributesRow, Ctx))
		{
			return Direct;
		}
		return nullptr;
	}
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
 * 
 */
UCLASS()
class DARKSCRIPT_API UAdvancedAttributeSet : public UDefaultAttributeSet
{
	GENERATED_BODY()
	
public:
	// ═══════════════════════════════════════════════════════════════════════
	// CONSTRUCTEUR
	// ═══════════════════════════════════════════════════════════════════════
	
	UAdvancedAttributeSet();
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Static)
	// ═══════════════════════════════════════════════════════════════════════
	
	static FArchetypeAttributeRow* GetArchetypeData(const EClassArchetype Archetype);
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Overrides)
	// ═══════════════════════════════════════════════════════════════════════
	
	virtual void Init(const EClassArchetype Archetype = EClassArchetype::None, const FName RowName = NAME_None) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════
	
	const FArchetypeAttributeRow* AdvancedData;
	
	// ═══════════════════════════════════════════════════════════════════════
	// PERKS : VITALITY (Points de Vitality)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de Vitality actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Vitality, Category = "Perks")
	FGameplayAttributeData Vitality;
	ATTRIBUTE_ACCESSORS(UAdvancedAttributeSet, Vitality)

	/** Callback de réplication pour Vitality */
	UFUNCTION()
	virtual void OnRep_Vitality(const FGameplayAttributeData& OldVitality);
	
	// ═══════════════════════════════════════════════════════════════════════
	// PERKS : STRENGTH (Points de Strength)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de Strength actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category = "Perks")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UAdvancedAttributeSet, Strength)

	/** Callback de réplication pour Strength */
	UFUNCTION()
	virtual void OnRep_Strength(const FGameplayAttributeData& OldStrength);
	
	// ═══════════════════════════════════════════════════════════════════════
	// PERKS : ACCURACY (Points de Accuracy)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de Accuracy actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Accuracy, Category = "Perks")
	FGameplayAttributeData Accuracy;
	ATTRIBUTE_ACCESSORS(UAdvancedAttributeSet, Accuracy)

	/** Callback de réplication pour Accuracy */
	UFUNCTION()
	virtual void OnRep_Accuracy(const FGameplayAttributeData& OldAccuracy);
	
	// ═══════════════════════════════════════════════════════════════════════
	// PERKS : INTELLIGENCE (Points de Intelligence)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de Intelligence actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intelligence, Category = "Perks")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UAdvancedAttributeSet, Intelligence)

	/** Callback de réplication pour Intelligence */
	UFUNCTION()
	virtual void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence);
	
	// ═══════════════════════════════════════════════════════════════════════
	// PERKS : MENTALITY (Points de Mentality)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Points de Mentality actuels */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mentality, Category = "Perks")
	FGameplayAttributeData Mentality;
	ATTRIBUTE_ACCESSORS(UAdvancedAttributeSet, Mentality)

	/** Callback de réplication pour Mentality */
	UFUNCTION()
	virtual void OnRep_Mentality(const FGameplayAttributeData& OldMentality);
};
