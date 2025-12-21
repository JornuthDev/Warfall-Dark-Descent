#include "Gameplay/AbilitySystem/Attributes/Mutual/ResistancesSet.h"
#include "Gameplay/AbilitySystem/DefaultAbilitySystem.h"
#include "Engine/World.h"
#include "DarkGameplayTags.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"



UResistancesSet::UResistancesSet()
	: Armor(0.f)
	, Ice(0.f)
	, Fire(0.f)
	, Lightning(0.f)
	, Holy(0.f)
	, Poison(0.f)
	, Bleeding(0.f)
{
	ArmorBeforeAttributeChange = 0.f;
	IceBeforeAttributeChange = 0.f;
	FireBeforeAttributeChange = 0.f;
	LightningBeforeAttributeChange = 0.f;
	HolyBeforeAttributeChange = 0.f;
	PoisonBeforeAttributeChange = 0.f;
	BleedingBeforeAttributeChange = 0.f;
}

void UResistancesSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UResistancesSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResistancesSet, Ice, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResistancesSet, Fire, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResistancesSet, Lightning, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResistancesSet, Holy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResistancesSet, Poison, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResistancesSet, Bleeding, COND_None, REPNOTIFY_Always);
}

void UResistancesSet::OnRep_Armor(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResistancesSet, Armor, OldValue);
	
	OnArmorChanged.Broadcast(nullptr, nullptr, nullptr, GetArmor() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetArmor());
}

void UResistancesSet::OnRep_Ice(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResistancesSet, Ice, OldValue);
	
	OnIceChanged.Broadcast(nullptr, nullptr, nullptr, GetIce() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetIce());
}

void UResistancesSet::OnRep_Fire(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResistancesSet, Fire, OldValue);
	
	OnFireChanged.Broadcast(nullptr, nullptr, nullptr, GetFire() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetFire());
}

void UResistancesSet::OnRep_Lightning(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResistancesSet, Lightning, OldValue);
	
	OnLightningChanged.Broadcast(nullptr, nullptr, nullptr, GetLightning() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetLightning());
}

void UResistancesSet::OnRep_Holy(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResistancesSet, Holy, OldValue);
	
	OnHolyChanged.Broadcast(nullptr, nullptr, nullptr, GetHoly() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetHoly());
}

void UResistancesSet::OnRep_Poison(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResistancesSet, Poison, OldValue);
	
	OnPoisonChanged.Broadcast(nullptr, nullptr, nullptr, GetPoison() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetPoison());
}

void UResistancesSet::OnRep_Bleeding(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResistancesSet, Bleeding, OldValue);
	
	OnBleedingChanged.Broadcast(nullptr, nullptr, nullptr, GetBleeding() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetBleeding());
}

bool UResistancesSet::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}
	
	ArmorBeforeAttributeChange = GetArmor();
	IceBeforeAttributeChange = GetIce();
	FireBeforeAttributeChange = GetFire();
	LightningBeforeAttributeChange = GetLightning();
	HolyBeforeAttributeChange = GetHoly();
	PoisonBeforeAttributeChange = GetPoison();
	BleedingBeforeAttributeChange = GetBleeding();
	return true;
}

void UResistancesSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();
	
	if (GetArmor() != ArmorBeforeAttributeChange)
	{
		OnArmorChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, ArmorBeforeAttributeChange, GetArmor());
	}
	else if (GetIce() != IceBeforeAttributeChange)
	{
		OnIceChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, IceBeforeAttributeChange, GetIce());
	}
	else if (GetFire() != FireBeforeAttributeChange)
	{
		OnFireChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, FireBeforeAttributeChange, GetFire());
	}
	else if (GetLightning() != LightningBeforeAttributeChange)
	{
		OnLightningChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, LightningBeforeAttributeChange, GetLightning());
	}
	else if (GetHoly() != HolyBeforeAttributeChange)
	{
		OnHolyChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HolyBeforeAttributeChange, GetHoly());
	}
	else if (GetPoison() != PoisonBeforeAttributeChange)
	{
		OnPoisonChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, PoisonBeforeAttributeChange, GetPoison());
	}
	else if (GetBleeding() != BleedingBeforeAttributeChange)
	{
		OnBleedingChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, BleedingBeforeAttributeChange, GetBleeding());
	}
}

void UResistancesSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void UResistancesSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void UResistancesSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
}

void UResistancesSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::ClampAttribute(Attribute, NewValue);
	
	if (Attribute == GetArmorAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
	else
	{
		NewValue = FMath::Clamp(NewValue, -100.f, 100.f);
	}
}

float UResistancesSet::CalculateReducedDamage(const FGameplayTag& Tag, const float Magnitude, const float EffectiveArmor) const
{
	auto CalculateArmorReduction = [](const float Armor, const float ArmorMultiplier = 1.f) -> float
	{
		const float StillArmor = Armor * ArmorMultiplier;
		return StillArmor / (StillArmor + 1000.f);
	};
	
	auto ApplyResistance = [Magnitude](const float Resistance) -> float
	{
		return FMath::Max(0.f, Magnitude * (1.f - (Resistance) / 100.f));
	};
	
	struct FDamageTypeHandler
	{
		FGameplayTag Type;
		TFunction<float()> CalculateReduction;
	};
	
	const TArray<FDamageTypeHandler> DamageHandlers = {
		{
			DarkGameplayTags::TAG_Ability_Damage_Type_Flesh_Melee,
			[&]() -> float
			{
				const float Reduction = CalculateArmorReduction(GetArmor());
				return FMath::Max(0.f, Magnitude * (1.f - (Reduction)));
			}
		},
		{
			DarkGameplayTags::TAG_Ability_Damage_Type_Flesh_Ranged,
			[&]() -> float
			{
				const float Reduction = CalculateArmorReduction(GetArmor());
				return FMath::Max(0.f, Magnitude * (1.f - (Reduction)));
			}
		},
		{
			DarkGameplayTags::TAG_Ability_Damage_Type_Penetration_Melee,
			[&]() -> float
			{
				const float Reduction = CalculateArmorReduction(GetArmor(), EffectiveArmor);
				return FMath::Max(0.f, Magnitude * (1.f - (Reduction)));
			}
		},
		{
			DarkGameplayTags::TAG_Ability_Damage_Type_Penetration_Ranged,
			[&]() -> float
			{
				const float Reduction = CalculateArmorReduction(GetArmor(), EffectiveArmor);
				return FMath::Max(0.f, Magnitude * (1.f - (Reduction)));
			}
		},
		{DarkGameplayTags::TAG_Ability_Damage_Type_Ice, [&]() { return ApplyResistance(GetIce()); }},
		{DarkGameplayTags::TAG_Ability_Damage_Type_Fire, [&]() { return ApplyResistance(GetFire()); }},
		{DarkGameplayTags::TAG_Ability_Damage_Type_Lightning, [&]() { return ApplyResistance(GetLightning()); }},
		{DarkGameplayTags::TAG_Ability_Damage_Type_Holy, [&]() { return ApplyResistance(GetHoly()); }},
		{DarkGameplayTags::TAG_Ability_Damage_Type_Poison, [&]() { return ApplyResistance(GetPoison()); }},
		{DarkGameplayTags::TAG_Ability_Damage_Type_Bleeding, [&]() { return ApplyResistance(GetBleeding()); }},
	};
		
	for (const auto& [Type, CalculateReduction] : DamageHandlers)
	{
		if (Type == Tag)
		{
			return CalculateReduction();
		}
	}
	return Magnitude;	
}

