#include "Gameplay/AbilitySystem/DefaultAbilitySystem.h"

#include "DarkGameplayTags.h"
#include "Gameplay/AbilitySystem/Attributes/PropertyCauser.h"
#include "Gameplay/AbilitySystem/Attributes/Mutual/ResistancesSet.h"
#include "Utils/Helpers/SystemHelpers.h"

// ═══════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════
UDefaultAbilitySystem::UDefaultAbilitySystem()
{
	
}

void UDefaultAbilitySystem::Init(const FName& RowName)
{
	if (GetOwner()->HasAuthority())
	{
		if (const FDefaultAttributeRow* Data = AbilitySystem::GetDefaultAttributes(RowName))
		{
			if (UpdateDefaultAttributeSets(Data))
			{
				return;
			}
		}
		RemoveAllSpawnedAttributes();
	}
}

bool UDefaultAbilitySystem::UpdateDefaultAttributeSets(const FDefaultAttributeRow*& AttributeRow)
{
	if (AttributeRow)
	{
		TArray<TSubclassOf<UAttributeSet>> AttributeSets;
		AttributeRow->GetAllAttributeSets(AttributeSets);
		UAttributeSet* AttributeSet = nullptr;
		
		switch (ActualAttributeData.Resource.Type)
		{
			case EResourceType::None:
				break;
			case EResourceType::Mana:
				AttributeSet = const_cast<UManaSet*>(GetSet<UManaSet>());
				break;
			case EResourceType::Stamina:
				AttributeSet = const_cast<UStaminaSet*>(GetSet<UStaminaSet>());
				break;
			case EResourceType::Focus:
				AttributeSet = const_cast<UFocusSet*>(GetSet<UFocusSet>());
		default:;
		}
		
		if (AttributeSet)
		{
			RemoveSpawnedAttribute(AttributeSet);
			UE_LOG(LogTemp, Warning, TEXT("UpdateDefaultAttributeSets:: Remove %s "), *AttributeSet->GetName());
		}
		
		for (const TSubclassOf<UAttributeSet>& SetClass : AttributeSets)
		{
			if (SetClass)
			{
				if (AttributeSet = const_cast<UAttributeSet*>(GetAttributeSet(SetClass)); AttributeSet)
				{
					UE_LOG(LogTemp, Warning, TEXT("UpdateDefaultAttributeSets:: Remove %s "), *AttributeSet->GetName());
					RemoveSpawnedAttribute(AttributeSet);
				}
				AttributeSet = NewObject<UAttributeSet>(GetOwner(), SetClass);
				AddAttributeSetSubobject(AttributeSet);
			}
		}
		
		ActualAttributeData = *AttributeRow;
		InitDefaultAttributeSets(*AttributeRow);
		return true;
	}
	return false;
}

void UDefaultAbilitySystem::InitDefaultAttributeSets(const FDefaultAttributeRow& AttributeRow)
{
	InitHealthSet(AttributeRow);
	InitResourceSet(AttributeRow);
	InitCombatSet(AttributeRow);
	InitResistancesSet(AttributeRow);
}

void UDefaultAbilitySystem::InitHealthSet(const FDefaultAttributeRow& AttributeRow)
{
	SetNumericAttributeBase(UHealthSet::GetHealthAttribute(), AttributeRow.Health.HealthMax);
	SetNumericAttributeBase(UHealthSet::GetHealthMaxAttribute(), AttributeRow.Health.HealthMax);
	SetNumericAttributeBase(UHealthSet::GetHealthRegenRateAttribute(), AttributeRow.Health.HealthRegenRate);
}

EResourceType UDefaultAbilitySystem::InitResourceSet(const FDefaultAttributeRow& AttributeRow)
{
	switch (AttributeRow.Resource.Type)
	{
	case EResourceType::None:
		return EResourceType::None;
	case EResourceType::Mana:
		SetNumericAttributeBase(UManaSet::GetManaAttribute(), AttributeRow.Resource.ResourceOnStart);
		SetNumericAttributeBase(UManaSet::GetManaMaxAttribute(), AttributeRow.Resource.ResourceMax);
		SetNumericAttributeBase(UManaSet::GetManaRegenRateAttribute(), AttributeRow.Resource.ResourceRegenRate);
		return EResourceType::Mana;
	case EResourceType::Stamina:
		SetNumericAttributeBase(UStaminaSet::GetStaminaAttribute(), AttributeRow.Resource.ResourceOnStart);
		SetNumericAttributeBase(UStaminaSet::GetStaminaMaxAttribute(), AttributeRow.Resource.ResourceMax);
		SetNumericAttributeBase(UStaminaSet::GetStaminaRegenRateAttribute(), AttributeRow.Resource.ResourceRegenRate);
		return EResourceType::Stamina;
	case EResourceType::Focus:
		SetNumericAttributeBase(UFocusSet::GetFocusAttribute(), AttributeRow.Resource.ResourceOnStart);
		SetNumericAttributeBase(UFocusSet::GetFocusMaxAttribute(), AttributeRow.Resource.ResourceMax);
		SetNumericAttributeBase(UFocusSet::GetFocusRegenRateAttribute(), AttributeRow.Resource.ResourceRegenRate);
		return EResourceType::Focus;
	default:;
	}
	return EResourceType::None;
}

void UDefaultAbilitySystem::InitCombatSet(const FDefaultAttributeRow& AttributeRow)
{
	SetNumericAttributeBase(UPropertyInstigator::GetPhysicDamageMultiplierAttribute(), AttributeRow.Combat.PhysicDamageMultiplier);
	SetNumericAttributeBase(UPropertyInstigator::GetRangedDamageMultiplierAttribute(), AttributeRow.Combat.RangedDamageMultiplier);
	SetNumericAttributeBase(UPropertyInstigator::GetMagicDamageMultiplierAttribute(), AttributeRow.Combat.MagicDamageMultiplier);
	SetNumericAttributeBase(UPropertyInstigator::GetHealingMultiplierAttribute(), AttributeRow.Combat.HealingMultiplier);
	SetNumericAttributeBase(UPropertyInstigator::GetPhysicCriticalChanceAttribute(), AttributeRow.Combat.PhysicCriticalChance);
	SetNumericAttributeBase(UPropertyInstigator::GetPhysicCriticalPowerAttribute(), AttributeRow.Combat.PhysicCriticalPower);
	SetNumericAttributeBase(UPropertyInstigator::GetMagicCriticalChanceAttribute(), AttributeRow.Combat.MagicCriticalChance);
	SetNumericAttributeBase(UPropertyInstigator::GetMagicCriticalPowerAttribute(), AttributeRow.Combat.MagicCriticalPower);
	SetNumericAttributeBase(UPropertyInstigator::GetHealCriticalChanceAttribute(), AttributeRow.Combat.HealCriticalChance);
	SetNumericAttributeBase(UPropertyInstigator::GetHealCriticalPowerAttribute(), AttributeRow.Combat.HealCriticalPower);
}

void UDefaultAbilitySystem::InitResistancesSet(const FDefaultAttributeRow& AttributeRow)
{
	SetNumericAttributeBase(UResistancesSet::GetArmorAttribute(), AttributeRow.Resistances.Armor);
	SetNumericAttributeBase(UResistancesSet::GetIceAttribute(), AttributeRow.Resistances.IceResistance);
	SetNumericAttributeBase(UResistancesSet::GetFireAttribute(), AttributeRow.Resistances.FireResistance);
	SetNumericAttributeBase(UResistancesSet::GetLightningAttribute(), AttributeRow.Resistances.LightningResistance);
	SetNumericAttributeBase(UResistancesSet::GetHolyAttribute(), AttributeRow.Resistances.HolyResistance);
	SetNumericAttributeBase(UResistancesSet::GetPoisonAttribute(), AttributeRow.Resistances.PoisonResistance);
	SetNumericAttributeBase(UResistancesSet::GetBleedingAttribute(), AttributeRow.Resistances.BleedingResistance);
}

FActiveGameplayEffectHandle UDefaultAbilitySystem::ApplyDynamicGameplayEffect(
    FDynamicEffect& Params, 
    AActor* Causer, 
    UAbilitySystemComponent* Target)
{
    UAbilitySystemComponent* TargetASC = Target ? Target : this;
	
    if (!TargetASC || !IsValid(TargetASC))
    {
        UE_LOG(LogTemp, Error, TEXT("ApplyDynamicGameplayEffect: Invalid TargetASC!"));
        return FActiveGameplayEffectHandle();
    }
	
    if (!Params.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("ApplyDynamicGameplayEffect: Invalid Params!"));
        return FActiveGameplayEffectHandle();
    }
	
    if (!Params.CanApplyTo(TargetASC))
    {
        UE_LOG(LogTemp, Warning, 
            TEXT("ApplyDynamicGameplayEffect: Target %s doesn't have required AttributeSets"), 
            *TargetASC->GetOwner()->GetName());
        return FActiveGameplayEffectHandle();
    }
	
    const UGameplayEffect* FreshEffect = Params.CreateFreshEffect();
    if (!FreshEffect)
    {
        UE_LOG(LogTemp, Error, TEXT("ApplyDynamicGameplayEffect: Failed to create fresh effect!"));
        return FActiveGameplayEffectHandle();
    }
	
    FGameplayEffectContextHandle EffectContext = MakeEffectContext();
    if (!EffectContext.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("ApplyDynamicGameplayEffect: Invalid EffectContext!"));
        return FActiveGameplayEffectHandle();
    }
    
    EffectContext.AddSourceObject(this);
    EffectContext.AddInstigator(GetOwnerActor(), Causer);
	
    FGameplayEffectSpec Spec(FreshEffect, EffectContext, 1.f);
	
    if (Params.HasMagnitude())
    {
        Spec.SetSetByCallerMagnitude(DarkGameplayTags::SetByCaller_Modifier, Params.Magnitude);
    }
    if (Params.Policy == EGameplayEffectDurationType::HasDuration)
    {
        Spec.SetSetByCallerMagnitude(DarkGameplayTags::SetByCaller_Duration, Params.Duration);
    }
    return TargetASC->ApplyGameplayEffectSpecToSelf(Spec);
}

FActiveGameplayEffectHandle UDefaultAbilitySystem::ApplyPropertyToCharacter(const FGameplayTag PropertyTag, const EGameplayModOp::Type Operation,
	const float Magnitude,  AActor* Causer, UAbilitySystemComponent* Target, const bool bHasDuration,
	const float Duration)
{
	struct FTagAttribute
	{
		FGameplayTag Tag;
		FGameplayAttribute Attribute;
	};
	TArray<FTagAttribute> TagAttributes{
		// Health
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_HealthMax,
			UHealthSet::GetHealthMaxAttribute()},
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_HealthRegenRate,
			UHealthSet::GetHealthRegenRateAttribute()},

		// Resources - Mana
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Resource_ManaMax,
			UManaSet::GetManaMaxAttribute()},
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Resource_ManaRegenRate,
			UManaSet::GetManaRegenRateAttribute()},

		// Resources - Stamina
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Resource_StaminaMax,
			UStaminaSet::GetStaminaMaxAttribute()},
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Resource_StaminaRegenRate,
			UStaminaSet::GetStaminaRegenRateAttribute()},

		// Resources - Focus
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Resource_FocusMax,
			UFocusSet::GetFocusMaxAttribute()},
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Resource_FocusRegenRate,
			UFocusSet::GetFocusRegenRateAttribute()},

		// Armor
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Armor,
			UResistancesSet::GetArmorAttribute()},

		// Perks
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Perk_Vitality,
			UPlayerSet::GetVitalityAttribute()},
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Perk_Strength,
			UPlayerSet::GetStrengthAttribute()},
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Perk_Accuracy,
			UPlayerSet::GetAccuracyAttribute()},
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Perk_Intelligence,
			UPlayerSet::GetIntelligenceAttribute()},
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Perk_Mentality,
			UPlayerSet::GetMentalityAttribute()},

		// Resistances
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Resistance_Armor,
			UResistancesSet::GetArmorAttribute()},
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Resistance_Ice,
			UResistancesSet::GetIceAttribute()},
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Resistance_Fire,
			UResistancesSet::GetFireAttribute()},
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Resistance_Lightning,
			UResistancesSet::GetLightningAttribute()},
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Resistance_Holy,
			UResistancesSet::GetHolyAttribute()},
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Resistance_Poison,
			UResistancesSet::GetPoisonAttribute()},
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Resistance_Bleeding,
			UResistancesSet::GetBleedingAttribute()},

		// Damage
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Damage_Physic,
			UPropertyInstigator::GetPhysicDamageMultiplierAttribute()},
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Damage_Ranged,
			UPropertyInstigator::GetRangedDamageMultiplierAttribute()},
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Damage_Magic,
			UPropertyInstigator::GetMagicDamageMultiplierAttribute()},
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Damage_Physic_CriticalChance,
			UPropertyInstigator::GetPhysicCriticalChanceAttribute()},
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Damage_Physic_CriticalPower,
			UPropertyInstigator::GetPhysicCriticalPowerAttribute()},
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Damage_Magic_CriticalChance,
			UPropertyInstigator::GetMagicCriticalChanceAttribute()},
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Damage_Magic_CriticalPower,
			UPropertyInstigator::GetMagicCriticalPowerAttribute()},

		// Heal
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Heal,
			UPropertyInstigator::GetHealingMultiplierAttribute()},
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Heal_CriticalChance,
			UPropertyInstigator::GetHealCriticalChanceAttribute()},
		{DarkGameplayTags::TAG_Ability_Character_PropertyType_Heal_CriticalPower,
			UPropertyInstigator::GetHealCriticalPowerAttribute()},
	};

	const FTagAttribute* FoundAttr = TagAttributes.FindByPredicate([PropertyTag] (const FTagAttribute& Attribute)
	{
		return Attribute.Tag == PropertyTag;
	});
	if (!FoundAttr) return FActiveGameplayEffectHandle();
	
	FDynamicEffect Effect(FDynamicEffectBasic(FoundAttr->Attribute, Magnitude, bHasDuration, Duration), Operation);
	return ApplyDynamicGameplayEffect(Effect, Causer, Target);
}

FActiveGameplayEffectHandle UDefaultAbilitySystem::ApplyPropertyToCauser(const FGameplayTag PropertyTag, const EGameplayModOp::Type Operation,
                                                                         const float Magnitude, const bool bHasDuration, const float Duration)
{
	struct FTagAttribute
	{
		FGameplayTag Tag;
		FGameplayAttribute Attribute;
	};
	TArray<FTagAttribute> TagAttributes{
		{DarkGameplayTags::TAG_Ability_Causer_PropertyType_Damage, UPropertyCauser::GetDamageAttribute()},
		{DarkGameplayTags::TAG_Ability_Causer_PropertyType_ArmorPenetration, UPropertyCauser::GetArmorPenetrationAttribute()},
		{DarkGameplayTags::TAG_Ability_Causer_PropertyType_Multiplier_FleshMultiplier, UPropertyCauser::GetFleshMultiplierAttribute()},
		{DarkGameplayTags::TAG_Ability_Causer_PropertyType_Multiplier_PenetrationMultiplier, UPropertyCauser::GetPenetrationMultiplierAttribute()},
		{DarkGameplayTags::TAG_Ability_Causer_PropertyType_Multiplier_IceMultiplier, UPropertyCauser::GetIceMultiplierAttribute()},
		{DarkGameplayTags::TAG_Ability_Causer_PropertyType_Multiplier_FireMultiplier, UPropertyCauser::GetFireMultiplierAttribute()},
		{DarkGameplayTags::TAG_Ability_Causer_PropertyType_Multiplier_LightningMultiplier, UPropertyCauser::GetLightningMultiplierAttribute()},
		{DarkGameplayTags::TAG_Ability_Causer_PropertyType_Multiplier_HolyMultiplier, UPropertyCauser::GetHolyMultiplierAttribute()},
		{DarkGameplayTags::TAG_Ability_Causer_PropertyType_Multiplier_PoisonMultiplier, UPropertyCauser::GetPoisonMultiplierAttribute()},
		{DarkGameplayTags::TAG_Ability_Causer_PropertyType_Multiplier_BleedingMultiplier, UPropertyCauser::GetBleedingMultiplierAttribute()},
		{DarkGameplayTags::TAG_Ability_Causer_PropertyType_Multiplier_HealingMultiplier, UPropertyCauser::GetHealingMultiplierAttribute()},
		{DarkGameplayTags::TAG_Ability_Causer_PropertyType_Multiplier_SpeedMultiplier, UPropertyCauser::GetSpeedMultiplierAttribute()},
	};
	
	const FTagAttribute* FoundAttr = TagAttributes.FindByPredicate([PropertyTag](const FTagAttribute& Attribute)
	{
	   return Attribute.Tag == PropertyTag;
	});
	
	if (!FoundAttr) return FActiveGameplayEffectHandle();

	FDynamicEffect Effect(FDynamicEffectBasic(FoundAttr->Attribute, Magnitude, bHasDuration, Duration), Operation);
	return ApplyDynamicGameplayEffect(Effect);
}
