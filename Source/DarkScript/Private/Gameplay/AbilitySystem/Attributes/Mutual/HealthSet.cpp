#include "Gameplay/AbilitySystem/Attributes/Mutual/HealthSet.h"
#include "Gameplay/AbilitySystem/DefaultAbilitySystem.h"
#include "Engine/World.h"
#include "GameplayEffectExtension.h"
#include "Gameplay/AbilitySystem/AttributeTypes.h"
#include "Gameplay/AbilitySystem/Calculations/Attributes_Calculations.h"
#include "Net/UnrealNetwork.h"

UHealthSet::UHealthSet()
	: Health(100.f)
	, HealthRegenRate(0.01f)
	, HealthMax(100.f)
{
	bOutOfHealth = false;
	HealthMaxBeforeAttributeChange = 0.f;
	HealthRegenRateBeforeAttributeChange = 0.f;
	HealthBeforeAttributeChange = 0.f;
}

void UHealthSet::Init()
{
	if (GetDefaultAbilitySystem() && GetDefaultAbilitySystem()->HealthPerPerkEffectHandle.IsValid())
	{
		GetOwningAbilitySystemComponent()->RemoveActiveGameplayEffect(GetDefaultAbilitySystem()->HealthPerPerkEffectHandle);
	}
	const TMap<FGameplayAttribute, TSubclassOf<UGameplayEffectCalculation>> MaxAttributes{
		{GetHealthMaxAttribute(), UCalculation_HealthMax::StaticClass()}
	};
	FDynamicEffect HealthPerPerkEffect(FDynamicEffectArray(MaxAttributes), false, "HealthPerPerkEffect");
	GetDefaultAbilitySystem()->HealthPerPerkEffectHandle = GetDefaultAbilitySystem()->ApplyDynamicGameplayEffect(HealthPerPerkEffect);
}

void UHealthSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthSet, HealthMax, COND_None, REPNOTIFY_Always);
}

void UHealthSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthSet, Health, OldValue);
	
	const float CurrentHealth = GetHealth();
	const float EstimatedMagnitude = CurrentHealth - OldValue.GetCurrentValue();
	
	OnHealthChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentHealth);
	
	if (!bOutOfHealth && CurrentHealth <= 0.f)
	{
		OnOutOfHealth.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentHealth);
	}
	
	bOutOfHealth = (CurrentHealth <= 0.f);
}

void UHealthSet::OnRep_HealthRegenRate(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthSet, HealthRegenRate, OldValue);
	
	OnHealthRegenRateChanged.Broadcast(nullptr, nullptr, nullptr, GetHealthRegenRate() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetHealthRegenRate());
}

void UHealthSet::OnRep_HealthMax(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthSet, HealthMax, OldValue);
	
	OnHealthMaxChanged.Broadcast(nullptr, nullptr, nullptr, GetHealthMax() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetHealthMax());
}

bool UHealthSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}
	
	HealthBeforeAttributeChange = GetHealth();
	HealthRegenRateBeforeAttributeChange = GetHealthRegenRate();
	HealthMaxBeforeAttributeChange = GetHealthMax();
	return true;
}

void UHealthSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	float MinimumHealth = 0.0f;
	
	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();
	
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		if (Data.EvaluatedData.Magnitude != 0.f)
		{
			const float IncomingDamage = FMath::Abs(GetDamage());
			SetHealth(FMath::Clamp(GetHealth() - IncomingDamage, MinimumHealth, GetHealthMax()));
			SetDamage(0.f);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		const float IncomingHeal = FMath::Abs(GetHealing());
		SetHealth(FMath::Clamp(GetHealth() + IncomingHeal, MinimumHealth, GetHealthMax()));
		SetHealing(0.f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetHealthMax()));
	}
	else if (Data.EvaluatedData.Attribute == GetHealthMaxAttribute())
	{
		OnHealthMaxChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthMaxBeforeAttributeChange, GetHealthMax());
	}
	
	if (GetHealth() != HealthBeforeAttributeChange)
	{
		OnHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
	}
	if (GetHealthRegenRate() != HealthRegenRateBeforeAttributeChange)
	{
		OnHealthRegenRateChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthRegenRateBeforeAttributeChange, GetHealthRegenRate());
	}
	if ((GetHealth() <= 0.f) && !bOutOfHealth)
	{
		OnOutOfHealth.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
	}
	
	bOutOfHealth = (GetHealth() <= 0.f);
}

void UHealthSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void UHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void UHealthSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	if (Attribute == GetHealthMaxAttribute())
	{
		if (GetHealth() > NewValue)
		{
			UDefaultAbilitySystem* DefaultASC = GetDefaultAbilitySystem();
			check(DefaultASC);
			DefaultASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
		}
	}
	
	if (bOutOfHealth && (GetHealth() > 0.f))
	{
		bOutOfHealth = false;
	}
}

void UHealthSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::ClampAttribute(Attribute, NewValue);
	
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetHealthMax());
	}
	else if (Attribute == GetHealthMaxAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}
	else if (Attribute == GetHealthRegenRateAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
}
