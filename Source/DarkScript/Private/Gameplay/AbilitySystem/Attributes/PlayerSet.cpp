#include "Gameplay/AbilitySystem/Attributes/PlayerSet.h"

#include "GameplayEffectExecutionCalculation.h"
#include "GameplayEffectExtension.h"
#include "GameplayRules.h"
#include "Gameplay/AbilitySystem/AttributeTypes.h"
#include "Gameplay/AbilitySystem/DefaultAbilitySystem.h"
#include "Gameplay/AbilitySystem/DynamicEffectConfig.h"
#include "Gameplay/AbilitySystem/Calculations/Perks_Calculations.h"
#include "Net/UnrealNetwork.h"

UPlayerSet::UPlayerSet()
	: Level(LevelRules::StartLevel)
	, Experience(0.f)
	, ExpForNextLevel(0.f)
	, Vitality(0.f)
	, Strength(0.f)
	, Accuracy(0.f)
	, Intelligence(0.f)
	, Mentality(0.f)
{
	InitExpForNextLevel(CalculateExpRequiredForLevel());
	
	LevelBeforeAttributeChange = 0.f;
	ExperienceBeforeAttributeChange = 0.f;
	VitalityBeforeAttributeChange = 0.f;
	StrengthBeforeAttributeChange = 0.f;
	AccuracyBeforeAttributeChange = 0.f;
	IntelligenceBeforeAttributeChange = 0.f;
	MentalityBeforeAttributeChange = 0.f;
}

void UPlayerSet::Init()
{
	if (PerksPerLevelEffectHandle.IsValid())
	{
		GetOwningAbilitySystemComponent()->RemoveActiveGameplayEffect(PerksPerLevelEffectHandle);
	}

	const TArray<FDynamicModifierConfig> Mods{
		{GetVitalityAttribute(), UCalculation_Vitality::StaticClass()},
		{GetStrengthAttribute(), UCalculation_Strength::StaticClass()},
		{GetAccuracyAttribute(), UCalculation_Accuracy::StaticClass()},
		{GetIntelligenceAttribute(), UCalculation_Intelligence::StaticClass()},
		{GetMentalityAttribute(), UCalculation_Mentality::StaticClass()}
	};
	
	FDynamicEffectConfig Config(Mods);
	Config.DurationPolicy = EGameplayEffectDurationType::Infinite;
	PerksPerLevelEffectHandle = GetDefaultAbilitySystem()->ApplyDynamicEffect(Config);
}

void UPlayerSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerSet, Level, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerSet, Experience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerSet, ExpForNextLevel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerSet, Vitality, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerSet, Accuracy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerSet, Mentality, COND_None, REPNOTIFY_Always);
}

void UPlayerSet::OnRep_Level(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerSet, Level, OldValue);
	
	OnLevelChanged.Broadcast(nullptr, nullptr, nullptr, GetLevel() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetLevel());
}

void UPlayerSet::OnRep_Experience(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerSet, Experience, OldValue);
	
	OnExperienceChanged.Broadcast(nullptr, nullptr, nullptr, GetExperience() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetExperience());
}

void UPlayerSet::OnRep_ExpForNextLevel(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerSet, ExpForNextLevel, OldValue);
	
}

void UPlayerSet::OnRep_Vitality(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerSet, Vitality, OldValue);
	
	OnVitalityChanged.Broadcast(nullptr, nullptr, nullptr, GetVitality() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetVitality());
}

void UPlayerSet::OnRep_Strength(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerSet, Strength, OldValue);
	
	OnStrengthChanged.Broadcast(nullptr, nullptr, nullptr, GetStrength() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetStrength());
}

void UPlayerSet::OnRep_Accuracy(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerSet, Accuracy, OldValue);
	
	OnAccuracyChanged.Broadcast(nullptr, nullptr, nullptr, GetAccuracy() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetAccuracy());
}

void UPlayerSet::OnRep_Intelligence(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerSet, Intelligence, OldValue);
	
	OnIntelligenceChanged.Broadcast(nullptr, nullptr, nullptr, GetIntelligence() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetIntelligence());
}

void UPlayerSet::OnRep_Mentality(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerSet, Mentality, OldValue);
	
	OnMentalityChanged.Broadcast(nullptr, nullptr, nullptr, GetMentality() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetMentality());
}

bool UPlayerSet::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}
	if (Data.EvaluatedData.Attribute == GetExperienceAttribute())
	{
		if (GetLevel() >= LevelRules::MaxLevel || Data.EvaluatedData.Magnitude == 0)
		{
			return false;
		}
	}
	
	LevelBeforeAttributeChange = GetLevel();
	ExperienceBeforeAttributeChange = GetExperience();
	VitalityBeforeAttributeChange = GetVitality();
	StrengthBeforeAttributeChange = GetStrength();
	AccuracyBeforeAttributeChange = GetAccuracy();
	IntelligenceBeforeAttributeChange = GetIntelligence();
	MentalityBeforeAttributeChange = GetMentality();
	return true;
}

void UPlayerSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();
	
	// Gestion de l'expérience et level up
	if (Data.EvaluatedData.Attribute == GetExperienceAttribute())
	{
		OnExperienceUpdating(Data);
	}
	else if (GetVitality() != VitalityBeforeAttributeChange)
	{
		OnVitalityChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, VitalityBeforeAttributeChange, GetVitality());
	}
	else if (GetStrength() != StrengthBeforeAttributeChange)
	{
		OnStrengthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, StrengthBeforeAttributeChange, GetStrength());
	}
	else if (GetAccuracy() != AccuracyBeforeAttributeChange)
	{
		OnAccuracyChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, AccuracyBeforeAttributeChange, GetAccuracy());
	}
	else if (GetIntelligence() != IntelligenceBeforeAttributeChange)
	{
		OnIntelligenceChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, IntelligenceBeforeAttributeChange, GetIntelligence());
	}
	else if (GetMentality() != MentalityBeforeAttributeChange)
	{
		OnMentalityChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MentalityBeforeAttributeChange, GetMentality());
	}
}

void UPlayerSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void UPlayerSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void UPlayerSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

void UPlayerSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::ClampAttribute(Attribute, NewValue);
	
	if (Attribute != GetExperienceAttribute() || Attribute != GetExpForNextLevelAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
	else if (Attribute == GetExpForNextLevelAttribute())
	{
		NewValue = FMath::Max(CalculateExpRequiredForLevel(), 0.f);
	}
}

float UPlayerSet::CalculateExpRequiredForLevel() const
{
	if (GetLevel() <= 0)
	{
		return 0.f;
	}
	const float XPRequired = LevelRules::RequiredExpForLevel * FMath::Pow(GetLevel(), LevelRules::ExperienceCurvePow);
	return XPRequired;
}

void UPlayerSet::OnExperienceUpdating(const struct FGameplayEffectModCallbackData& Data)
{
	AActor* Instigator = Data.EffectSpec.GetContext().GetInstigator();
	AActor* Causer = Data.EffectSpec.GetContext().GetEffectCauser();
	
	float CurrentExp = GetExperience();
	float ExpNeeded = GetExpForNextLevel();
	while (CurrentExp >= ExpNeeded && GetLevel() < LevelRules::MaxLevel)
	{
		CurrentExp -= ExpNeeded;

		const float NewLevel = GetLevel() + 1.f;
		SetLevel(NewLevel);
		if (NewLevel >= LevelRules::MaxLevel)
		{
			const float MaxLevelExpRequired = CalculateExpRequiredForLevel();
			SetExpForNextLevel(MaxLevelExpRequired);
			CurrentExp = MaxLevelExpRequired;
			OnLevelUp(Data);
			break;
		}

		const float NewExpRequired = CalculateExpRequiredForLevel();
		SetExpForNextLevel(NewExpRequired);
		ExpNeeded = NewExpRequired;
			
		OnLevelUp(Data);
	}
	if (GetLevel() >= LevelRules::MaxLevel)
	{
		CurrentExp = 0;
	}
	SetExperience(CurrentExp);
	OnExperienceChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, ExperienceBeforeAttributeChange, GetExperience());
}

void UPlayerSet::OnLevelUp(const FGameplayEffectModCallbackData& Data)
{
	OnLevelChanged.Broadcast(nullptr, nullptr, &Data.EffectSpec, Data.EvaluatedData.Magnitude, ExperienceBeforeAttributeChange, GetExperience());
	
	
}
