#include "Gameplay/AbilitySystem/DefaultAbilitySystem.h"

#include "DarkGameplayTags.h"
#include "Gameplay/AbilitySystem/DynamicEffectConfig.h"
#include "Gameplay/AbilitySystem/Attributes/PropertyCauser.h"
#include "Gameplay/AbilitySystem/Attributes/Mutual/ResistancesSet.h"
#include "Gameplay/AbilitySystem/Effects/DynamicGameplayEffect.h"
#include "Utils/Helpers/SystemHelpers.h"

// ═══════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════
UDefaultAbilitySystem::UDefaultAbilitySystem()
{

}

void UDefaultAbilitySystem::Init(const FName& RowName)
{
	if (HasAuthority())
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

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Internal)
// ═══════════════════════════════════════════════════════════════════════

FActiveGameplayEffectHandle UDefaultAbilitySystem::ApplyDynamicEffect(
	const FDynamicEffectConfig& Config,
	AActor* Causer,
	UAbilitySystemComponent* Target)
{
	// ✅ Vérifier l'autorité - Les effets doivent être appliqués côté serveur
	if (!HasAuthority())
	{
		Server_ApplyDynamicEffect(Config, Causer, Target);
		return FActiveGameplayEffectHandle();
	}

	UAbilitySystemComponent* TargetASC = Target ? Target : this;
	if (!TargetASC || !IsValid(TargetASC))
	{
		UE_LOG(LogTemp, Error, TEXT("ApplyDynamicEffect: Invalid target ASC"));
		return FActiveGameplayEffectHandle();
	}

	// ✅ Créer l'effet dynamique - Cette classe PEUT être répliquée
	UDynamicGameplayEffect* Effect = UDynamicGameplayEffect::StaticClass()->GetDefaultObject<UDynamicGameplayEffect>();
	if (!Effect)
	{
		return FActiveGameplayEffectHandle();
	}
	Effect->ConfigureFromStruct(Config);

	// Créer le contexte
	FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	EffectContext.AddInstigator(GetOwnerActor(), Causer);

	// Créer la spec
	const FGameplayEffectSpec Spec(Effect, EffectContext, 1.f);

	// ✅ Appliquer l'effet - GAS gère la réplication automatiquement
	return TargetASC->ApplyGameplayEffectSpecToSelf(Spec);
}

FActiveGameplayEffectHandle UDefaultAbilitySystem::ApplyDynamicEffectWithValues(
	const FDynamicEffectConfig& Config,
	const float MagnitudeValue,
	const float DurationValue,
	AActor* Causer,
	UAbilitySystemComponent* Target)
{
	if (!HasAuthority())
	{
		Server_ApplyDynamicEffectWithValues(Config, MagnitudeValue, DurationValue, Causer, Target);
		return FActiveGameplayEffectHandle();
	}

	UAbilitySystemComponent* TargetASC = Target ? Target : this;
	if (!TargetASC || !IsValid(TargetASC))
	{
		return FActiveGameplayEffectHandle();
	}

	UDynamicGameplayEffect* Effect = UDynamicGameplayEffect::StaticClass()->GetDefaultObject<UDynamicGameplayEffect>();
	if (!Effect)
	{
		return FActiveGameplayEffectHandle();
	}
	Effect->ConfigureFromStruct(Config);

	FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	EffectContext.AddInstigator(GetOwnerActor(), Causer);

	FGameplayEffectSpec Spec(Effect, EffectContext, 1.f);
	// ✅ Définir les valeurs SetByCaller
	if (Config.Modifiers.Num() > 0 && Config.Modifiers[0].bUseSetByCaller)
	{
		Spec.SetSetByCallerMagnitude(Config.Modifiers[0].SetByCallerTag, MagnitudeValue);
	}

	if (Config.bUseDurationSetByCaller && DurationValue > 0.f)
	{
		Spec.SetSetByCallerMagnitude(Config.DurationSetByCallerTag, DurationValue);
	}

	return TargetASC->ApplyGameplayEffectSpecToSelf(Spec);
}

FActiveGameplayEffectHandle UDefaultAbilitySystem::ApplySimpleEffect(
	const FGameplayAttribute& Attribute,
	const float Magnitude,
	const EGameplayModOp::Type Operation,
	const EGameplayEffectDurationType DurationType,
	const float Duration,
	AActor* Causer,
	UAbilitySystemComponent* Target)
{
	// ✅ Créer une config simple
	FDynamicEffectConfig Config;
	Config.DurationPolicy = DurationType;
	Config.Duration = Duration;
	Config.Modifiers.Add(FDynamicModifierConfig(Attribute, Magnitude, Operation));

	return ApplyDynamicEffect(Config, Causer, Target);
}

// ═══════════════════════════════════════════════════════════════════════
// BLUEPRINT API (backward compatibility)
// ═══════════════════════════════════════════════════════════════════════

FActiveGameplayEffectHandle UDefaultAbilitySystem::ApplyPropertyToCharacter(const FGameplayTag PropertyTag, const EGameplayModOp::Type Operation,
	const float Magnitude,  AActor* Causer, UAbilitySystemComponent* Target, const bool bHasDuration,
	const float Duration)
{
	const FGameplayAttribute Attribute = DarkGameplayTags::FindAttributeByTag(PropertyTag);
	if (!Attribute.IsValid()) return FActiveGameplayEffectHandle();

	const EGameplayEffectDurationType DurationType = bHasDuration
	? EGameplayEffectDurationType::HasDuration
	: EGameplayEffectDurationType::Infinite;
	
	return ApplySimpleEffect(Attribute, Magnitude, Operation, DurationType, Duration, Causer, Target);
}

FActiveGameplayEffectHandle UDefaultAbilitySystem::ApplyPropertyToCauser(const FGameplayTag PropertyTag, const EGameplayModOp::Type Operation,
                                                                         const float Magnitude, const bool bHasDuration, const float Duration)
{
	const FGameplayAttribute Attribute = DarkGameplayTags::FindAttributeByTag(PropertyTag);
	if (!Attribute.IsValid()) return FActiveGameplayEffectHandle();

	const EGameplayEffectDurationType DurationType = bHasDuration
	? EGameplayEffectDurationType::HasDuration
	: EGameplayEffectDurationType::Infinite;
	
	return ApplySimpleEffect(Attribute, Magnitude, Operation, DurationType, Duration);
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Replication)
// ═══════════════════════════════════════════════════════════════════════

void UDefaultAbilitySystem::Server_ApplyDynamicEffect_Implementation(const FDynamicEffectConfig& Config, AActor* Causer,
	UAbilitySystemComponent* Target)
{
	ApplyDynamicEffect(Config, Causer, Target);
}

void UDefaultAbilitySystem::Server_ApplyDynamicEffectWithValues_Implementation(const FDynamicEffectConfig& Config,
                                                                               const float MagnitudeValue, const float DurationValue, AActor* Causer, UAbilitySystemComponent* Target)
{
	ApplyDynamicEffectWithValues(Config, MagnitudeValue, DurationValue, Causer, Target);
}
