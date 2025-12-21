#include "Gameplay/AbilitySystem/Attributes/Resources/ManaSet.h"
#include "Gameplay/AbilitySystem/DefaultAbilitySystem.h"
#include "Engine/World.h"
#include "GameplayEffectExtension.h"
#include "Gameplay/AbilitySystem/DynamicEffectConfig.h"
#include "Gameplay/AbilitySystem/Calculations/Attributes_Calculations.h"
#include "Net/UnrealNetwork.h"

UManaSet::UManaSet()
	: Mana(100.f)
	, ManaRegenRate(0.01f)
	, ManaMax(100.f)
{
	ManaMaxBeforeAttributeChange = 0.f;
	ManaRegenRateBeforeAttributeChange = 0.f;
	ManaBeforeAttributeChange = 0.f;
}

void UManaSet::Init()
{
	if (!GetOwningAbilitySystemComponent()->GetOwner()->HasAuthority())
	{
		return;
	}
	
	if (GetDefaultAbilitySystem() && GetDefaultAbilitySystem()->ManaPerPerkEffectHandle.IsValid())
	{
		GetOwningAbilitySystemComponent()->RemoveActiveGameplayEffect(GetDefaultAbilitySystem()->ManaPerPerkEffectHandle);
	}
	const TArray<FDynamicModifierConfig> Mods{
				{GetManaMaxAttribute(), UCalculation_ManaMax::StaticClass()},
				{GetManaRegenRateAttribute(), UCalculation_ManaRegenRate::StaticClass()},
		};
	
	FDynamicEffectConfig Config(Mods);
	Config.DurationPolicy = EGameplayEffectDurationType::Infinite;
	GetDefaultAbilitySystem()->ManaPerPerkEffectHandle = GetDefaultAbilitySystem()->ApplyDynamicEffect(Config);
}

void UManaSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UManaSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UManaSet, ManaMax, COND_None, REPNOTIFY_Always);
}

void UManaSet::OnRep_Mana(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UManaSet, Mana, OldValue);
	
	const float CurrentMana = GetMana();
	const float EstimatedMagnitude = CurrentMana - OldValue.GetCurrentValue();
	
	OnManaChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentMana);
}

void UManaSet::OnRep_ManaRegenRate(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UManaSet, ManaRegenRate, OldValue);
	
	OnManaRegenRateChanged.Broadcast(nullptr, nullptr, nullptr, GetManaRegenRate() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetManaRegenRate());
}

void UManaSet::OnRep_ManaMax(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UManaSet, ManaMax, OldValue);
	
	OnManaMaxChanged.Broadcast(nullptr, nullptr, nullptr, GetManaMax() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetManaMax());
}

bool UManaSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}
	
	ManaBeforeAttributeChange = GetMana();
	ManaRegenRateBeforeAttributeChange = GetManaRegenRate();
	ManaMaxBeforeAttributeChange = GetManaMax();
	return true;
}

void UManaSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();
	
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		constexpr float MinimumMana = 0.0f;
		SetMana(FMath::Clamp(GetMana(), MinimumMana, GetManaMax()));
	}
	else if (Data.EvaluatedData.Attribute == GetManaMaxAttribute())
	{
		OnManaMaxChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, ManaMaxBeforeAttributeChange, GetManaMax());
	}
	
	if (GetMana() != ManaBeforeAttributeChange)
	{
		OnManaChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, ManaBeforeAttributeChange, GetMana());
	}
	if (GetManaRegenRate() != ManaRegenRateBeforeAttributeChange)
	{
		OnManaRegenRateChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, ManaRegenRateBeforeAttributeChange, GetManaRegenRate());
	}
}

void UManaSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void UManaSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void UManaSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	if (Attribute == GetManaMaxAttribute())
	{
		if (GetMana() > NewValue)
		{
			UDefaultAbilitySystem* DefaultASC = GetDefaultAbilitySystem();
			check(DefaultASC);
			DefaultASC->ApplyModToAttribute(GetManaAttribute(), EGameplayModOp::Override, NewValue);
		}
	}
}

void UManaSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::ClampAttribute(Attribute, NewValue);
	
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetManaMax());
	}
	else if (Attribute == GetManaMaxAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}
	else if (Attribute == GetManaRegenRateAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
}
