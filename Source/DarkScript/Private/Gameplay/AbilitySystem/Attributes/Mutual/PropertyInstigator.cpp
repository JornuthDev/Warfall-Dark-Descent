#include "Gameplay/AbilitySystem/Attributes/Mutual/PropertyInstigator.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UPropertyInstigator::UPropertyInstigator()
	: PhysicDamageMultiplier(1.f)
	, RangedDamageMultiplier(1.f)
	, MagicDamageMultiplier(1.f)
	, HealingMultiplier(1.f)
	, PhysicCriticalChance(0.f)
	, PhysicCriticalPower(0.f)
	, MagicCriticalChance(0.f)
	, MagicCriticalPower(0.f)
	, HealCriticalChance(0.f)
	, HealCriticalPower(0.f)
{
	PhysicDamageMultiplierBeforeAttributeChange = 1.f;
	RangedDamageMultiplierBeforeAttributeChange = 1.f;
	MagicDamageMultiplierBeforeAttributeChange = 1.f;
	HealingMultiplierBeforeAttributeChange = 1.f;
	PhysicCriticalChanceBeforeAttributeChange = 0.f;
	PhysicCriticalPowerBeforeAttributeChange = 0.f;
	MagicCriticalChanceBeforeAttributeChange = 0.f;
	MagicCriticalPowerBeforeAttributeChange = 0.f;
	HealCriticalChanceBeforeAttributeChange = 0.f;
	HealCriticalPowerBeforeAttributeChange = 0.f;
}

void UPropertyInstigator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UPropertyInstigator, PhysicDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPropertyInstigator, RangedDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPropertyInstigator, MagicDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPropertyInstigator, HealingMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPropertyInstigator, PhysicCriticalChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPropertyInstigator, PhysicCriticalPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPropertyInstigator, MagicCriticalChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPropertyInstigator, MagicCriticalPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPropertyInstigator, HealCriticalChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPropertyInstigator, HealCriticalPower, COND_None, REPNOTIFY_Always);
}

void UPropertyInstigator::OnRep_PhysicDamageMultiplier(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPropertyInstigator, PhysicDamageMultiplier, OldValue);

	OnPhysicDamageMultiplierChanged.Broadcast(nullptr, nullptr, nullptr, GetPhysicDamageMultiplier() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetPhysicDamageMultiplier());
}

void UPropertyInstigator::OnRep_RangedDamageMultiplier(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPropertyInstigator, RangedDamageMultiplier, OldValue);

	OnRangedDamageMultiplierChanged.Broadcast(nullptr, nullptr, nullptr, GetRangedDamageMultiplier() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetRangedDamageMultiplier());
}

void UPropertyInstigator::OnRep_MagicDamageMultiplier(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPropertyInstigator, MagicDamageMultiplier, OldValue);

	OnMagicDamageMultiplierChanged.Broadcast(nullptr, nullptr, nullptr, GetMagicDamageMultiplier() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetMagicDamageMultiplier());
}

void UPropertyInstigator::OnRep_HealingMultiplier(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPropertyInstigator, HealingMultiplier, OldValue);

	OnHealingMultiplierChanged.Broadcast(nullptr, nullptr, nullptr, GetHealingMultiplier() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetHealingMultiplier());
}

void UPropertyInstigator::OnRep_PhysicCriticalChance(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPropertyInstigator, PhysicCriticalChance, OldValue);

	OnPhysicCriticalChanceChanged.Broadcast(nullptr, nullptr, nullptr, GetPhysicCriticalChance() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetPhysicCriticalChance());
}

void UPropertyInstigator::OnRep_PhysicCriticalPower(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPropertyInstigator, PhysicCriticalPower, OldValue);

	OnPhysicCriticalPowerChanged.Broadcast(nullptr, nullptr, nullptr, GetPhysicCriticalPower() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetPhysicCriticalPower());
}

void UPropertyInstigator::OnRep_MagicCriticalChance(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPropertyInstigator, MagicCriticalChance, OldValue);

	OnMagicCriticalChanceChanged.Broadcast(nullptr, nullptr, nullptr, GetMagicCriticalChance() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetMagicCriticalChance());
}

void UPropertyInstigator::OnRep_MagicCriticalPower(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPropertyInstigator, MagicCriticalPower, OldValue);

	OnMagicCriticalPowerChanged.Broadcast(nullptr, nullptr, nullptr, GetMagicCriticalPower() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetMagicCriticalPower());
}

void UPropertyInstigator::OnRep_HealCriticalChance(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPropertyInstigator, HealCriticalChance, OldValue);

	OnHealCriticalChanceChanged.Broadcast(nullptr, nullptr, nullptr, GetHealCriticalChance() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetHealCriticalChance());
}

void UPropertyInstigator::OnRep_HealCriticalPower(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPropertyInstigator, HealCriticalPower, OldValue);

	OnHealCriticalPowerChanged.Broadcast(nullptr, nullptr, nullptr, GetHealCriticalPower() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetHealCriticalPower());
}

bool UPropertyInstigator::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	PhysicDamageMultiplierBeforeAttributeChange = GetPhysicDamageMultiplier();
	RangedDamageMultiplierBeforeAttributeChange = GetRangedDamageMultiplier();
	MagicDamageMultiplierBeforeAttributeChange = GetMagicDamageMultiplier();
	HealingMultiplierBeforeAttributeChange = GetHealingMultiplier();
	PhysicCriticalChanceBeforeAttributeChange = GetPhysicCriticalChance();
	PhysicCriticalPowerBeforeAttributeChange = GetPhysicCriticalPower();
	MagicCriticalChanceBeforeAttributeChange = GetMagicCriticalChance();
	MagicCriticalPowerBeforeAttributeChange = GetMagicCriticalPower();
	HealCriticalChanceBeforeAttributeChange = GetHealCriticalChance();
	HealCriticalPowerBeforeAttributeChange = GetHealCriticalPower();
	return true;
}

void UPropertyInstigator::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	if (GetPhysicDamageMultiplier() != PhysicDamageMultiplierBeforeAttributeChange)
	{
		OnPhysicDamageMultiplierChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, PhysicDamageMultiplierBeforeAttributeChange, GetPhysicDamageMultiplier());
	}
	else if (GetRangedDamageMultiplier() != RangedDamageMultiplierBeforeAttributeChange)
	{
		OnRangedDamageMultiplierChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, RangedDamageMultiplierBeforeAttributeChange, GetRangedDamageMultiplier());
	}
	else if (GetMagicDamageMultiplier() != MagicDamageMultiplierBeforeAttributeChange)
	{
		OnMagicDamageMultiplierChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MagicDamageMultiplierBeforeAttributeChange, GetMagicDamageMultiplier());
	}
	else if (GetHealingMultiplier() != HealingMultiplierBeforeAttributeChange)
	{
		OnHealingMultiplierChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealingMultiplierBeforeAttributeChange, GetHealingMultiplier());
	}
	else if (GetPhysicCriticalChance() != PhysicCriticalChanceBeforeAttributeChange)
	{
		OnPhysicCriticalChanceChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, PhysicCriticalChanceBeforeAttributeChange, GetPhysicCriticalChance());
	}
	else if (GetPhysicCriticalPower() != PhysicCriticalPowerBeforeAttributeChange)
	{
		OnPhysicCriticalPowerChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, PhysicCriticalPowerBeforeAttributeChange, GetPhysicCriticalPower());
	}
	else if (GetMagicCriticalChance() != MagicCriticalChanceBeforeAttributeChange)
	{
		OnMagicCriticalChanceChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MagicCriticalChanceBeforeAttributeChange, GetMagicCriticalChance());
	}
	else if (GetMagicCriticalPower() != MagicCriticalPowerBeforeAttributeChange)
	{
		OnMagicCriticalPowerChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MagicCriticalPowerBeforeAttributeChange, GetMagicCriticalPower());
	}
	else if (GetHealCriticalChance() != HealCriticalChanceBeforeAttributeChange)
	{
		OnHealCriticalChanceChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealCriticalChanceBeforeAttributeChange, GetHealCriticalChance());
	}
	else if (GetHealCriticalPower() != HealCriticalPowerBeforeAttributeChange)
	{
		OnHealCriticalPowerChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealCriticalPowerBeforeAttributeChange, GetHealCriticalPower());
	}
}

void UPropertyInstigator::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UPropertyInstigator::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UPropertyInstigator::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

void UPropertyInstigator::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::ClampAttribute(Attribute, NewValue);

	NewValue = FMath::Max(NewValue, 0.0f);
}
