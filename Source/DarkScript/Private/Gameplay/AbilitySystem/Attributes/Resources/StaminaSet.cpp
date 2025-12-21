#include "Gameplay/AbilitySystem/Attributes/Resources/StaminaSet.h"
#include "Gameplay/AbilitySystem/DefaultAbilitySystem.h"
#include "Engine/World.h"
#include "GameplayEffectExtension.h"
#include "Gameplay/AbilitySystem/AttributeTypes.h"
#include "Gameplay/AbilitySystem/Calculations/Attributes_Calculations.h"
#include "Net/UnrealNetwork.h"

UStaminaSet::UStaminaSet()
	: Stamina(100.f)
	, StaminaRegenRate(0.01f)
	, StaminaMax(100.f)
{
	StaminaMaxBeforeAttributeChange = 0.f;
	StaminaRegenRateBeforeAttributeChange = 0.f;
	StaminaBeforeAttributeChange = 0.f;
}

void UStaminaSet::Init()
{
	if (GetDefaultAbilitySystem() && GetDefaultAbilitySystem()->StaminaPerPerkEffectHandle.IsValid())
	{
		GetOwningAbilitySystemComponent()->RemoveActiveGameplayEffect(GetDefaultAbilitySystem()->StaminaPerPerkEffectHandle);
	}
	const TMap<FGameplayAttribute, TSubclassOf<UGameplayEffectCalculation>> MaxAttributes{
				{GetStaminaMaxAttribute(), UCalculation_StaminaMax::StaticClass()}
	};
	FDynamicEffect StaminaPerPerkEffect(FDynamicEffectArray(MaxAttributes), false, "StaminaPerPerkEffect");
	GetDefaultAbilitySystem()->StaminaPerPerkEffectHandle = GetDefaultAbilitySystem()->ApplyDynamicGameplayEffect(StaminaPerPerkEffect);
}

void UStaminaSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UStaminaSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UStaminaSet, StaminaMax, COND_None, REPNOTIFY_Always);
}

void UStaminaSet::OnRep_Stamina(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStaminaSet, Stamina, OldValue);
	
	const float CurrentStamina = GetStamina();
	const float EstimatedMagnitude = CurrentStamina - OldValue.GetCurrentValue();
	
	OnStaminaChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentStamina);
}

void UStaminaSet::OnRep_StaminaRegenRate(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStaminaSet, StaminaRegenRate, OldValue);
	
	OnStaminaRegenRateChanged.Broadcast(nullptr, nullptr, nullptr, GetStaminaRegenRate() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetStaminaRegenRate());
}

void UStaminaSet::OnRep_StaminaMax(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStaminaSet, StaminaMax, OldValue);
	
	OnStaminaMaxChanged.Broadcast(nullptr, nullptr, nullptr, GetStaminaMax() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetStaminaMax());
}

bool UStaminaSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}
	
	StaminaBeforeAttributeChange = GetStamina();
	StaminaRegenRateBeforeAttributeChange = GetStaminaRegenRate();
	StaminaMaxBeforeAttributeChange = GetStaminaMax();
	return true;
}

void UStaminaSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();
	
	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		constexpr float MinimumStamina = 0.0f;
		SetStamina(FMath::Clamp(GetStamina(), MinimumStamina, GetStaminaMax()));
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaMaxAttribute())
	{
		OnStaminaMaxChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, StaminaMaxBeforeAttributeChange, GetStaminaMax());
	}
	
	if (GetStamina() != StaminaBeforeAttributeChange)
	{
		OnStaminaChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, StaminaBeforeAttributeChange, GetStamina());
	}
	if (GetStaminaRegenRate() != StaminaRegenRateBeforeAttributeChange)
	{
		OnStaminaRegenRateChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, StaminaRegenRateBeforeAttributeChange, GetStaminaRegenRate());
	}
}

void UStaminaSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void UStaminaSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void UStaminaSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	if (Attribute == GetStaminaMaxAttribute())
	{
		if (GetStamina() > NewValue)
		{
			UDefaultAbilitySystem* DefaultASC = GetDefaultAbilitySystem();
			check(DefaultASC);
			DefaultASC->ApplyModToAttribute(GetStaminaAttribute(), EGameplayModOp::Override, NewValue);
		}
	}
	
}

void UStaminaSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::ClampAttribute(Attribute, NewValue);
	
	if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetStaminaMax());
	}
	else if (Attribute == GetStaminaMaxAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}
	else if (Attribute == GetStaminaRegenRateAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
}
