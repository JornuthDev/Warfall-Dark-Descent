#include "Gameplay/AbilitySystem/AdvancedAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Gameplay/AbilitySystem/GlobalAbilitySystem.h"
#include "Net/UnrealNetwork.h"

// ═══════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════

UAdvancedAttributeSet::UAdvancedAttributeSet()
{
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Static)
// ═══════════════════════════════════════════════════════════════════════

FArchetypeAttributeRow* UAdvancedAttributeSet::GetArchetypeData(const EClassArchetype Archetype)
{
	const UDataTable* Table = Content::GetTable(Tables::ArchetypeData);
	if (!Table) return nullptr;
	const FString Ctx(TEXT("Looking_For_Data"));
	
	TArray<FArchetypeAttributeRow*> Rows;
	Table->GetAllRows(Ctx, Rows);
	
	FArchetypeAttributeRow* const* FoundRowPtr = Rows.FindByPredicate([&Archetype](const FArchetypeAttributeRow* Row)
	{
		return Row && Row->Archetype == Archetype;
	});
	
	if (FoundRowPtr && *FoundRowPtr)
	{
		return *FoundRowPtr;
	}
	return nullptr;
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Overrides)
// ═══════════════════════════════════════════════════════════════════════

void UAdvancedAttributeSet::Init(const EClassArchetype Archetype, const FName RowName)
{
	if (Archetype == EClassArchetype::None)
	{
		Super::Init(Archetype, RowName);
		return;
	}
	
	AdvancedData = GetArchetypeData(Archetype);
	if (AdvancedData)
	{
		AttributeData = AdvancedData->GetAttributeData();
	}
	if (!AttributeData || !GetOwningAbilitySystemComponent()) return;
	
	Super::Init(Archetype, RowName);
	
	auto CalculatePerkPoints = [](const int32 InLevel, const float PerkWeight) -> float
	{
		if (PerkWeight <= 0 || InLevel - 1 <= 0) return 0.0f;
		
		const float Result = static_cast<float>(InLevel - 1) / PerkWeight;
		return FMath::FloorToInt(Result);
	};
	
	SetAttribute(GetVitalityAttribute(), CalculatePerkPoints(CurrentLevel, AdvancedData->Vitality));
	SetAttribute(GetStrengthAttribute(), CalculatePerkPoints(CurrentLevel, AdvancedData->Strength));
	SetAttribute(GetAccuracyAttribute(), CalculatePerkPoints(CurrentLevel, AdvancedData->Accuracy));
	SetAttribute(GetIntelligenceAttribute(), CalculatePerkPoints(CurrentLevel, AdvancedData->Intelligence));
	SetAttribute(GetMentalityAttribute(), CalculatePerkPoints(CurrentLevel, AdvancedData->Mentality));
}

void UAdvancedAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// ───────────────────────────────────────────────────────────────────────
	// PERKS
	// ───────────────────────────────────────────────────────────────────────
	DOREPLIFETIME_CONDITION_NOTIFY(UAdvancedAttributeSet, Vitality, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAdvancedAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAdvancedAttributeSet, Accuracy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAdvancedAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAdvancedAttributeSet, Mentality, COND_None, REPNOTIFY_Always);
}

void UAdvancedAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	// Récupérer l'attribut qui a été modifié
	FGameplayAttribute Attribute = Data.EvaluatedData.Attribute;
	
	// ───────────────────────────────────────────────────────────────────────
	// VITALITY : Min 0
	// ───────────────────────────────────────────────────────────────────────
	if (Attribute == GetVitalityAttribute())
	{
		SetVitality(FMath::Max(GetVitality(), 0.f));
	}
	
	// ───────────────────────────────────────────────────────────────────────
	// STRENGTH : Min 0
	// ───────────────────────────────────────────────────────────────────────
	else if (Attribute == GetStrengthAttribute())
	{
		SetStrength(FMath::Max(GetStrength(), 0.f));
	}
	
	// ───────────────────────────────────────────────────────────────────────
	// ACCURACY : Min 0
	// ───────────────────────────────────────────────────────────────────────
	else if (Attribute == GetAccuracyAttribute())
	{
		SetAccuracy(FMath::Max(GetAccuracy(), 0.f));
	}
	
	// ───────────────────────────────────────────────────────────────────────
	// INTELLIGENCE : Min 0
	// ───────────────────────────────────────────────────────────────────────
	else if (Attribute == GetIntelligenceAttribute())
	{
		SetIntelligence(FMath::Max(GetIntelligence(), 0.f));
	}
	
	// ───────────────────────────────────────────────────────────────────────
	// MENTALITY : Min 0
	// ───────────────────────────────────────────────────────────────────────
	else if (Attribute == GetMentalityAttribute())
	{
		SetMentality(FMath::Max(GetMentality(), 0.f));
	}
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Internal)
// ═══════════════════════════════════════════════════════════════════════

void UAdvancedAttributeSet::OnLevelUp(const FOnAttributeChangeData& ChangeData)
{
	if (!AdvancedData)
	{
		UE_LOG(DarkScript, Error, TEXT("OnLevelUp: AdvancedData is null!"));
		return;
	}
	const int32 NewValue = static_cast<int32>(ChangeData.NewValue);
	const int32 LevelDifference = NewValue - ChangeData.OldValue;
	
	if (LevelDifference == 0)
	{
		return;
	}
	
	// Lambda pour calculer les points de perk gagnés/perdus
	auto CalculatePerkPoints = [](const int32 LevelCount, const float PerkWeight) -> float
	{
		if (PerkWeight <= 0 || LevelCount - 1 == 0) return 0.0f;
		return FMath::FloorToInt(static_cast<float>(LevelCount - 1) / PerkWeight);
	};
	
	// Structure pour mapper chaque perk à son attribut et son poids
	struct FPerkMapping
	{
		FGameplayAttribute Attribute;
		float Weight;
	};
	
	// Table de mapping de tous les perks
	TArray<FPerkMapping> PerkMappings = {
		{ GetVitalityAttribute(),     AdvancedData->Vitality },
		{ GetStrengthAttribute(),     AdvancedData->Strength },
		{ GetAccuracyAttribute(),     AdvancedData->Accuracy },
		{ GetIntelligenceAttribute(), AdvancedData->Intelligence },
		{ GetMentalityAttribute(),    AdvancedData->Mentality }
	};
	
	// Déterminer si on monte ou descend de niveau
	const bool bLevelUp = LevelDifference > 0;
	
	// Mettre à jour tous les perks en une seule boucle
	for (const auto& [Attribute, Weight] : PerkMappings)
	{
		if (Attribute.IsValid() && Weight > 0)
		{
			const float CurrentPerkValue = GetOwningAbilitySystemComponent()->GetNumericAttribute(Attribute);
			const float PerkChange = CalculatePerkPoints(NewValue, Weight);
			const float AfterChange = PerkChange - CurrentPerkValue;
			
			// Si level up : ajouter les points, si level down : retirer les points
			const float NewPerkValue = bLevelUp ? 
				CurrentPerkValue + AfterChange : 
				FMath::Max(0.f, CurrentPerkValue - AfterChange); // Ne pas descendre en dessous de 0.
			SetAttribute(Attribute, NewPerkValue);
		}
	}
	CurrentLevel = NewValue;
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - VITALITY
// ═══════════════════════════════════════════════════════════════════════════

void UAdvancedAttributeSet::OnRep_Vitality(const FGameplayAttributeData& OldVitality)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAdvancedAttributeSet, Vitality, OldVitality);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - STRENGTH
// ═══════════════════════════════════════════════════════════════════════════

void UAdvancedAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAdvancedAttributeSet, Strength, OldStrength);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - ACCURACY
// ═══════════════════════════════════════════════════════════════════════════

void UAdvancedAttributeSet::OnRep_Accuracy(const FGameplayAttributeData& OldAccuracy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAdvancedAttributeSet, Accuracy, OldAccuracy);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - INTELLIGENCE
// ═══════════════════════════════════════════════════════════════════════════

void UAdvancedAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAdvancedAttributeSet, Intelligence, OldIntelligence);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - MENTALITY
// ═══════════════════════════════════════════════════════════════════════════

void UAdvancedAttributeSet::OnRep_Mentality(const FGameplayAttributeData& OldMentality)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAdvancedAttributeSet, Mentality, OldMentality);
}