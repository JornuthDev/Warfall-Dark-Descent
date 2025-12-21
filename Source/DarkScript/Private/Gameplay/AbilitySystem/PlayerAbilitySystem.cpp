#include "Gameplay/AbilitySystem/PlayerAbilitySystem.h"

#include "Gameplay/AbilitySystem/AttributeTypes.h"
#include "Utils/Helpers/SystemHelpers.h"

// ═══════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════

UPlayerAbilitySystem::UPlayerAbilitySystem()
{

}

void UPlayerAbilitySystem::Init(const FName& RowName)
{
	if (HasAuthority())
	{
		if (UpdatePlayerAttributeSets(RowName, true))
		{
			return;
		}
		RemoveAllSpawnedAttributes();
	}
}

bool UPlayerAbilitySystem::UpdatePlayerAttributeSets(const FName& RowName, const bool bIsFirstInit)
{
	
	if (const FAdvancedAttributeRow* Data = AbilitySystem::GetPlayerAttributes(RowName))
	{
		if (bIsFirstInit)
		{
			UAttributeSet* AttributeSet = NewObject<UAttributeSet>(GetOwner(), Data->GetProgressionAttributeSet());
			AddAttributeSetSubobject(AttributeSet);
		}
		InitPlayerSet(*Data, bIsFirstInit);
		
		if (const FDefaultAttributeRow* Default = &Data->Default; UpdateDefaultAttributeSets(Default))
		{
			return true;
		}
		return false;
	}
	return false;
}

void UPlayerAbilitySystem::InitPlayerSet(const FAdvancedAttributeRow& AttributeRow, const bool bInitLevel)
{
	UPlayerSet* AttributeSet = const_cast<UPlayerSet*>(GetSet<UPlayerSet>());
	if (!AttributeSet)
		return;
	
	if (bInitLevel)
	{
		SetNumericAttributeBase(UPlayerSet::GetLevelAttribute(), AttributeRow.Progression.Level); //Level Load Here
		SetNumericAttributeBase(UPlayerSet::GetExpForNextLevelAttribute(), GetNumericAttribute(UPlayerSet::GetExpForNextLevelAttribute()));
		SetNumericAttributeBase(UPlayerSet::GetExperienceAttribute(), AttributeRow.Progression.Experience); //Experience Load Here
	}
	
	SetNumericAttributeBase(UPlayerSet::GetVitalityAttribute(), AttributeRow.Progression.VitalityPerLevel);
	SetNumericAttributeBase(UPlayerSet::GetStrengthAttribute(), AttributeRow.Progression.StrengthPerLevel);
	SetNumericAttributeBase(UPlayerSet::GetAccuracyAttribute(), AttributeRow.Progression.AccuracyPerLevel);
	SetNumericAttributeBase(UPlayerSet::GetIntelligenceAttribute(), AttributeRow.Progression.IntelligencePerLevel);
	SetNumericAttributeBase(UPlayerSet::GetMentalityAttribute(), AttributeRow.Progression.MentalityPerLevel);
	
	AttributeSet->Init();
}

void UPlayerAbilitySystem::InitHealthSet(const FDefaultAttributeRow& AttributeRow)
{
	Super::InitHealthSet(AttributeRow);
	
	UHealthSet* AttributeSet = const_cast<UHealthSet*>(GetSet<UHealthSet>());
	if (!AttributeSet)
		return;
	RemoveActiveGameplayEffect(HealthPerPerkEffectHandle);
	AttributeSet->Init();
}

EResourceType UPlayerAbilitySystem::InitResourceSet(const FDefaultAttributeRow& AttributeRow)
{
	const EResourceType Type = Super::InitResourceSet(AttributeRow);
	UDefaultAttributeSet* AttributeSet = nullptr;
	
	switch (AttributeRow.Resource.Type)
	{
	case EResourceType::None:
		return EResourceType::None;
	case EResourceType::Mana:
		AttributeSet = const_cast<UManaSet*>(GetSet<UManaSet>());
		break;
	case EResourceType::Stamina:
		AttributeSet = const_cast<UStaminaSet*>(GetSet<UStaminaSet>());
		break;
	case EResourceType::Focus:
		AttributeSet = const_cast<UFocusSet*>(GetSet<UFocusSet>());
		break;
	default:;
	}
	RemoveActiveGameplayEffect(StaminaPerPerkEffectHandle);
	RemoveActiveGameplayEffect(ManaPerPerkEffectHandle);
	RemoveActiveGameplayEffect(FocusPerPerkEffectHandle);
	AttributeSet->Init();
	return Type;
}

void UPlayerAbilitySystem::InitCombatSet(const FDefaultAttributeRow& AttributeRow)
{
	Super::InitCombatSet(AttributeRow);
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Blueprint API)
// ═══════════════════════════════════════════════════════════════════════

void UPlayerAbilitySystem::GiveExperience(const float Amount)
{
	ApplySimpleEffect(UPlayerSet::GetExperienceAttribute(), Amount, EGameplayModOp::Additive);
}



