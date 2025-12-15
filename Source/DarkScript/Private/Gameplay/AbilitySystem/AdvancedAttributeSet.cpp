#include "Gameplay/AbilitySystem/AdvancedAttributeSet.h"

#include "GameplayEffectExtension.h"
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
		return;
	AdvancedData = GetArchetypeData(Archetype);
	if (AdvancedData)
	{
		AttributeData = AdvancedData->GetAttributeData();
	}
	if (!AttributeData) return;
	
	Super::Init(Archetype, RowName);
	
	int32 Level = 1;
	
	UE_LOG(LogTemp, Warning, TEXT("Init from %s | OwningActor is %s"), GetOwningAbilitySystemComponent() ? *GetOwningAbilitySystemComponent()->GetName() : TEXT("not valid"),
		GetOwningActor() ? *GetOwningActor()->GetName() : TEXT("not valid"));
	
	auto CalculatePerkPoints = [](const int32 InLevel, const float PerkWeight) -> float
	{
		if (PerkWeight <= 0 || InLevel <= 0) return 0.0f;
		
		const float Result = static_cast<float>(InLevel) / PerkWeight;
		return FMath::FloorToInt(Result);
	};
	
	SetVitality(CalculatePerkPoints(Level, AdvancedData->Vitality));
	SetStrength(CalculatePerkPoints(Level, AdvancedData->Strength));
	SetAccuracy(CalculatePerkPoints(Level, AdvancedData->Accuracy));
	SetIntelligence(CalculatePerkPoints(Level, AdvancedData->Intelligence));
	SetMentality(CalculatePerkPoints(Level, AdvancedData->Mentality));
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