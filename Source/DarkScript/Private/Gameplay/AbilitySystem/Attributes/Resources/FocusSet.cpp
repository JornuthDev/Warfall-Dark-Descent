#include "Gameplay/AbilitySystem/Attributes/Resources/FocusSet.h"
#include "Gameplay/AbilitySystem/DefaultAbilitySystem.h"
#include "Engine/World.h"
#include "GameplayEffectExtension.h"
#include "Gameplay/AbilitySystem/AttributeTypes.h"
#include "Gameplay/AbilitySystem/Calculations/Attributes_Calculations.h"
#include "Net/UnrealNetwork.h"

UFocusSet::UFocusSet()
	: Focus(100.f)
	, FocusRegenRate(0.01f)
	, FocusMax(100.f)
{
	FocusMaxBeforeAttributeChange = 0.f;
	FocusRegenRateBeforeAttributeChange = 0.f;
	FocusBeforeAttributeChange = 0.f;
}

void UFocusSet::Init()
{
	if (GetDefaultAbilitySystem() && GetDefaultAbilitySystem()->FocusPerPerkEffectHandle.IsValid())
	{
		GetOwningAbilitySystemComponent()->RemoveActiveGameplayEffect(GetDefaultAbilitySystem()->FocusPerPerkEffectHandle);
	}
	const TMap<FGameplayAttribute, TSubclassOf<UGameplayEffectCalculation>> MaxAttributes{
			{GetFocusMaxAttribute(), UCalculation_FocusMax::StaticClass()}
	};
	FDynamicEffect FocusPerPerkEffect(FDynamicEffectArray(MaxAttributes), false, "FocusPerPerkEffect");
	GetDefaultAbilitySystem()->FocusPerPerkEffectHandle = GetDefaultAbilitySystem()->ApplyDynamicGameplayEffect(FocusPerPerkEffect);
}

void UFocusSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UFocusSet, Focus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFocusSet, FocusMax, COND_None, REPNOTIFY_Always);
}

void UFocusSet::OnRep_Focus(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFocusSet, Focus, OldValue);
	
	const float CurrentFocus = GetFocus();
	const float EstimatedMagnitude = CurrentFocus - OldValue.GetCurrentValue();
	
	OnFocusChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentFocus);
}

void UFocusSet::OnRep_FocusRegenRate(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFocusSet, FocusRegenRate, OldValue);
	
	OnFocusRegenRateChanged.Broadcast(nullptr, nullptr, nullptr, GetFocusRegenRate() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetFocusRegenRate());
}

void UFocusSet::OnRep_FocusMax(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFocusSet, FocusMax, OldValue);
	
	OnFocusMaxChanged.Broadcast(nullptr, nullptr, nullptr, GetFocusMax() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetFocusMax());
}

bool UFocusSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}
	
	FocusBeforeAttributeChange = GetFocus();
	FocusRegenRateBeforeAttributeChange = GetFocusRegenRate();
	FocusMaxBeforeAttributeChange = GetFocusMax();
	return true;
}

void UFocusSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();
	
	if (Data.EvaluatedData.Attribute == GetFocusAttribute())
	{
		constexpr float MinimumFocus = 0.0f;
		SetFocus(FMath::Clamp(GetFocus(), MinimumFocus, GetFocusMax()));
	}
	else if (Data.EvaluatedData.Attribute == GetFocusMaxAttribute())
	{
		OnFocusMaxChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, FocusMaxBeforeAttributeChange, GetFocusMax());
	}
	
	if (GetFocus() != FocusBeforeAttributeChange)
	{
		OnFocusChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, FocusBeforeAttributeChange, GetFocus());
	}
	if (GetFocusRegenRate() != FocusRegenRateBeforeAttributeChange)
	{
		OnFocusRegenRateChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, FocusRegenRateBeforeAttributeChange, GetFocusRegenRate());
	}
}

void UFocusSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void UFocusSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void UFocusSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	if (Attribute == GetFocusMaxAttribute())
	{
		if (GetFocus() > NewValue)
		{
			UDefaultAbilitySystem* DefaultASC = GetDefaultAbilitySystem();
			check(DefaultASC);
			DefaultASC->ApplyModToAttribute(GetFocusAttribute(), EGameplayModOp::Override, NewValue);
		}
	}
}

void UFocusSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::ClampAttribute(Attribute, NewValue);
	
	if (Attribute == GetFocusAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetFocusMax());
	}
	else if (Attribute == GetFocusMaxAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}
	else if (Attribute == GetFocusRegenRateAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
}
