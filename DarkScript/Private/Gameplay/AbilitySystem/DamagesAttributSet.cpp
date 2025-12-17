#include "Gameplay/AbilitySystem/DamagesAttributSet.h"
#include "Net/UnrealNetwork.h"
#include "Utils/Tags.h"

// ═══════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════

UDamagesAttributSet::UDamagesAttributSet()
{
}

void UDamagesAttributSet::Init(const FName RowName)
{
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Overrides)
// ═══════════════════════════════════════════════════════════════════════

void UDamagesAttributSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UDamagesAttributSet, Damage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDamagesAttributSet, FleshDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDamagesAttributSet, PenetrationDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDamagesAttributSet, IceDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDamagesAttributSet, FireDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDamagesAttributSet, LightningDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDamagesAttributSet, HolyDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDamagesAttributSet, PoisonDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDamagesAttributSet, BleedingDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDamagesAttributSet, RangedDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDamagesAttributSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
}

void UDamagesAttributSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Helpers)
// ═══════════════════════════════════════════════════════════════════════

void UDamagesAttributSet::SetAttribute(const FGameplayAttribute& Attribute, const float NewValue) const
{
	if (!GetOwningAbilitySystemComponent()) return;
	GetOwningAbilitySystemComponent()->SetNumericAttributeBase(Attribute, NewValue);
}

float UDamagesAttributSet::GetFinalDamage(const FGameplayTag& IncomingDamage) const
{
	struct FDamageTypeInfo
	{
		FGameplayTag DamageType;
		float Multiplier;
		
		FDamageTypeInfo(const FGameplayTag& InType, const float InMultiplier = 1.f)
			: DamageType(InType)
			, Multiplier(InMultiplier)
		{}
	};

	// Dans ta fonction (là où tu en as besoin)
	const TArray<FDamageTypeInfo> DamageTypes = {
		{DamageType::Flesh,       GetFleshDamageMultiplier()},
		{DamageType::Penetration, GetPenetrationDamageMultiplier()},
		{DamageType::Ice,         GetIceDamageMultiplier()},
		{DamageType::Fire,        GetFireDamageMultiplier()},
		{DamageType::Lightning,   GetLightningDamageMultiplier()},
		{DamageType::Holy,        GetHolyDamageMultiplier()},
		{DamageType::Poison,      GetPoisonDamageMultiplier()},
		{DamageType::Bleeding,    GetBleedingDamageMultiplier()},
		{DamageType::Ranged,      GetRangedDamageMultiplier()}
	};
	
	const FDamageTypeInfo* FoundDamage = DamageTypes.FindByPredicate([&](const FDamageTypeInfo& Info)
	{
	return Info.DamageType == IncomingDamage; // ou ton tag à chercher
	});
	
	if (!FoundDamage) return GetDamage();
	return GetDamage() * FoundDamage->Multiplier;
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Notify)
// ═══════════════════════════════════════════════════════════════════════

void UDamagesAttributSet::OnRep_Damage(FGameplayAttributeData& OldDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDamagesAttributSet, Damage, OldDamage);
}

void UDamagesAttributSet::OnRep_FleshDamageMultiplier(FGameplayAttributeData& OldFleshDamageMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDamagesAttributSet, FleshDamageMultiplier, OldFleshDamageMultiplier);
}

void UDamagesAttributSet::OnRep_PenetrationDamageMultiplier(FGameplayAttributeData& OldPenetrationDamageMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDamagesAttributSet, PenetrationDamageMultiplier, OldPenetrationDamageMultiplier);
}

void UDamagesAttributSet::OnRep_IceDamageMultiplier(FGameplayAttributeData& OldIceDamageMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDamagesAttributSet, IceDamageMultiplier, OldIceDamageMultiplier);
}

void UDamagesAttributSet::OnRep_FireDamageMultiplier(FGameplayAttributeData& OldFireDamageMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDamagesAttributSet, FireDamageMultiplier, OldFireDamageMultiplier);
}

void UDamagesAttributSet::OnRep_LightningDamageMultiplier(FGameplayAttributeData& OldLightningDamageMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDamagesAttributSet, LightningDamageMultiplier, OldLightningDamageMultiplier);
}

void UDamagesAttributSet::OnRep_HolyDamageMultiplier(FGameplayAttributeData& OldHolyDamageMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDamagesAttributSet, HolyDamageMultiplier, OldHolyDamageMultiplier);
}

void UDamagesAttributSet::OnRep_PoisonDamageMultiplier(FGameplayAttributeData& OldPoisonDamageMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDamagesAttributSet, PoisonDamageMultiplier, OldPoisonDamageMultiplier);
}

void UDamagesAttributSet::OnRep_BleedingDamageMultiplier(FGameplayAttributeData& OldBleedingDamageMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDamagesAttributSet, BleedingDamageMultiplier, OldBleedingDamageMultiplier);
}

void UDamagesAttributSet::OnRep_RangedDamageMultiplier(FGameplayAttributeData& OldRangedDamageMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDamagesAttributSet, RangedDamageMultiplier, OldRangedDamageMultiplier);
}

void UDamagesAttributSet::OnRep_ArmorPenetration(FGameplayAttributeData& OldArmorPenetration)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDamagesAttributSet, ArmorPenetration, OldArmorPenetration);
}
