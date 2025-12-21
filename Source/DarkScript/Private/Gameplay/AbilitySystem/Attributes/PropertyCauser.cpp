#include "Gameplay/AbilitySystem/Attributes/PropertyCauser.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UPropertyCauser::UPropertyCauser()
	: Damage(0.f)
	, ArmorPenetration(0.f)
	, FleshMultiplier(1.f)
	, PenetrationMultiplier(1.f)
	, IceMultiplier(1.f)
	, FireMultiplier(1.f)
	, LightningMultiplier(1.f)
	, HolyMultiplier(1.f)
	, PoisonMultiplier(1.f)
	, BleedingMultiplier(1.f)
	, HealingMultiplier(1.f)
	, SpeedMultiplier(1.f)
{
	DamageBeforeAttributeChange = 0.f;
	ArmorPenetrationBeforeAttributeChange = 0.f;
	FleshMultiplierBeforeAttributeChange = 0.f;
	PenetrationMultiplierBeforeAttributeChange = 0.f;
	IceMultiplierBeforeAttributeChange = 0.f;
	FireMultiplierBeforeAttributeChange = 0.f;
	LightningMultiplierBeforeAttributeChange = 0.f;
	HolyMultiplierBeforeAttributeChange = 0.f;
	PoisonMultiplierBeforeAttributeChange = 0.f;
	BleedingMultiplierBeforeAttributeChange = 0.f;
	HealingMultiplierBeforeAttributeChange = 0.f;
	SpeedMultiplierBeforeAttributeChange = 0.f;
}

void UPropertyCauser::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UPropertyCauser, Damage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPropertyCauser, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPropertyCauser, FleshMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPropertyCauser, PenetrationMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPropertyCauser, IceMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPropertyCauser, FireMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPropertyCauser, LightningMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPropertyCauser, HolyMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPropertyCauser, PoisonMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPropertyCauser, BleedingMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPropertyCauser, HealingMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPropertyCauser, SpeedMultiplier, COND_None, REPNOTIFY_Always);
}

void UPropertyCauser::OnRep_Damage(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPropertyCauser, Damage, OldValue);
	
	OnDamageChanged.Broadcast(nullptr, nullptr, nullptr, GetDamage() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetDamage());
}

void UPropertyCauser::OnRep_ArmorPenetration(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPropertyCauser, ArmorPenetration, OldValue);
	
	OnArmorPenetrationChanged.Broadcast(nullptr, nullptr, nullptr, GetArmorPenetration() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetArmorPenetration());
}

void UPropertyCauser::OnRep_FleshMultiplier(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPropertyCauser, FleshMultiplier, OldValue);
	
	OnFleshMultiplierChanged.Broadcast(nullptr, nullptr, nullptr, GetFleshMultiplier() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetFleshMultiplier());
}

void UPropertyCauser::OnRep_PenetrationMultiplier(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPropertyCauser, PenetrationMultiplier, OldValue);
	
	OnPenetrationMultiplierChanged.Broadcast(nullptr, nullptr, nullptr, GetPenetrationMultiplier() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetPenetrationMultiplier());
}

void UPropertyCauser::OnRep_IceMultiplier(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPropertyCauser, IceMultiplier, OldValue);
	
	OnIceMultiplierChanged.Broadcast(nullptr, nullptr, nullptr, GetIceMultiplier() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetIceMultiplier());
}

void UPropertyCauser::OnRep_FireMultiplier(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPropertyCauser, FireMultiplier, OldValue);
	
	OnFireMultiplierChanged.Broadcast(nullptr, nullptr, nullptr, GetFireMultiplier() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetFireMultiplier());
}

void UPropertyCauser::OnRep_LightningMultiplier(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPropertyCauser, LightningMultiplier, OldValue);
	
	OnLightningMultiplierChanged.Broadcast(nullptr, nullptr, nullptr, GetLightningMultiplier() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetLightningMultiplier());
}

void UPropertyCauser::OnRep_HolyMultiplier(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPropertyCauser, HolyMultiplier, OldValue);
	
	OnHolyMultiplierChanged.Broadcast(nullptr, nullptr, nullptr, GetHolyMultiplier() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetHolyMultiplier());
}

void UPropertyCauser::OnRep_PoisonMultiplier(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPropertyCauser, PoisonMultiplier, OldValue);
	
	OnPoisonMultiplierChanged.Broadcast(nullptr, nullptr, nullptr, GetPoisonMultiplier() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetPoisonMultiplier());
}

void UPropertyCauser::OnRep_BleedingMultiplier(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPropertyCauser, BleedingMultiplier, OldValue);
	
	OnBleedingMultiplierChanged.Broadcast(nullptr, nullptr, nullptr, GetBleedingMultiplier() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetBleedingMultiplier());
}

void UPropertyCauser::OnRep_HealingMultiplier(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPropertyCauser, HealingMultiplier, OldValue);

	OnHealingMultiplierChanged.Broadcast(nullptr, nullptr, nullptr, GetHealingMultiplier() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetHealingMultiplier());
}

void UPropertyCauser::OnRep_SpeedMultiplier(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPropertyCauser, SpeedMultiplier, OldValue);

	OnSpeedMultiplierChanged.Broadcast(nullptr, nullptr, nullptr, GetSpeedMultiplier() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetSpeedMultiplier());
}

bool UPropertyCauser::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}
	
	DamageBeforeAttributeChange = GetDamage();
	ArmorPenetrationBeforeAttributeChange = GetArmorPenetration();
	FleshMultiplierBeforeAttributeChange = GetFleshMultiplier();
	PenetrationMultiplierBeforeAttributeChange = GetPenetrationMultiplier();
	IceMultiplierBeforeAttributeChange = GetIceMultiplier();
	FireMultiplierBeforeAttributeChange = GetFireMultiplier();
	LightningMultiplierBeforeAttributeChange = GetLightningMultiplier();
	HolyMultiplierBeforeAttributeChange = GetHolyMultiplier();
	PoisonMultiplierBeforeAttributeChange = GetPoisonMultiplier();
	BleedingMultiplierBeforeAttributeChange = GetBleedingMultiplier();
	HealingMultiplierBeforeAttributeChange = GetHealingMultiplier();
	SpeedMultiplierBeforeAttributeChange = GetSpeedMultiplier();
	return true;
}

void UPropertyCauser::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();
	
	if (GetDamage() != DamageBeforeAttributeChange)
	{
		OnDamageChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, DamageBeforeAttributeChange, GetDamage());
	}
	else if (GetArmorPenetration() != ArmorPenetrationBeforeAttributeChange)
	{
		OnArmorPenetrationChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, ArmorPenetrationBeforeAttributeChange, GetArmorPenetration());
	}
	else if (GetFleshMultiplier() != FleshMultiplierBeforeAttributeChange)
	{
		OnFleshMultiplierChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, FleshMultiplierBeforeAttributeChange, GetFleshMultiplier());
	}
	else if (GetPenetrationMultiplier() != PenetrationMultiplierBeforeAttributeChange)
	{
		OnPenetrationMultiplierChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, PenetrationMultiplierBeforeAttributeChange, GetPenetrationMultiplier());
	}
	else if (GetIceMultiplier() != IceMultiplierBeforeAttributeChange)
	{
		OnIceMultiplierChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, IceMultiplierBeforeAttributeChange, GetIceMultiplier());
	}
	else if (GetFireMultiplier() != FireMultiplierBeforeAttributeChange)
	{
		OnFireMultiplierChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, FireMultiplierBeforeAttributeChange, GetFireMultiplier());
	}
	else if (GetLightningMultiplier() != LightningMultiplierBeforeAttributeChange)
	{
		OnLightningMultiplierChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, LightningMultiplierBeforeAttributeChange, GetLightningMultiplier());
	}
	else if (GetHolyMultiplier() != HolyMultiplierBeforeAttributeChange)
	{
		OnHolyMultiplierChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HolyMultiplierBeforeAttributeChange, GetHolyMultiplier());
	}
	else if (GetPoisonMultiplier() != PoisonMultiplierBeforeAttributeChange)
	{
		OnPoisonMultiplierChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, PoisonMultiplierBeforeAttributeChange, GetPoisonMultiplier());
	}
	else if (GetBleedingMultiplier() != BleedingMultiplierBeforeAttributeChange)
	{
		OnBleedingMultiplierChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, BleedingMultiplierBeforeAttributeChange, GetBleedingMultiplier());
	}
	else if (GetHealingMultiplier() != HealingMultiplierBeforeAttributeChange)
	{
		OnHealingMultiplierChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealingMultiplierBeforeAttributeChange, GetHealingMultiplier());
	}
	else if (GetSpeedMultiplier() != SpeedMultiplierBeforeAttributeChange)
	{
		OnSpeedMultiplierChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, SpeedMultiplierBeforeAttributeChange, GetSpeedMultiplier());
	}
}

void UPropertyCauser::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void UPropertyCauser::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void UPropertyCauser::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

void UPropertyCauser::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::ClampAttribute(Attribute, NewValue);
	
	NewValue = FMath::Max(NewValue, 0.0f);
}
