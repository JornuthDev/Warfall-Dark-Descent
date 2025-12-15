#include "Gameplay/AbilitySystem/DefaultAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Gameplay/AbilitySystem/GlobalAbilitySystem.h"
#include "Net/UnrealNetwork.h"
#include "Utils/Tags.h"
#include "Utils/Helpers/SystemsHelper.h"

// ═══════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════

UDefaultAttributeSet::UDefaultAttributeSet()
{
	
}

void UDefaultAttributeSet::Init(const EClassArchetype Archetype, const FName RowName)
{
	if (Archetype == EClassArchetype::None)
	{
		AttributeData = GetAttributeData(RowName);
	}
	if (!AttributeData) return;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── 1. Initialiser les Base Attributes (depuis Data) ─────────────────────────
	// Ces valeurs sont permanente et ne peuvent être modifiées
	SetBaseHealth(AttributeData->Health);
	SetBaseHealthRegenRate(AttributeData->HealthRegenRate);
	
	SetBaseMana(AttributeData->Mana);
	SetBaseManaRegenRate(AttributeData->ManaRegenRate);
	
	SetBaseStamina(AttributeData->Stamina);
	SetStaminaRegenRate(AttributeData->StaminaRegenRate);
	
	SetBaseFocus(AttributeData->Focus);
	SetBaseFocusRegenRate(AttributeData->FocusRegenRate);
	
	SetBasePhysicDamageMultiplier(AttributeData->PhysicDamageMultiplier);
	SetBaseMagicDamageMultiplier(AttributeData->MagicDamageMultiplier);
	SetBaseAccuracyDamageMultiplier(AttributeData->AccuracyDamageMultiplier);
	
	SetBaseCriticalChance(AttributeData->CriticalChance);
	SetBaseCriticalDamage(AttributeData->CriticalDamage);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── 2. Initialiser les Resistances (fixes depuis Data) ───────────────────────
	SetArmor(AttributeData->Armor);
	SetIceResistance(AttributeData->IceResistance);
	SetFireResistance(AttributeData->FireResistance);
	SetLightningResistance(AttributeData->LightningResistance);
	SetHolyResistance(AttributeData->HolyResistance);
	SetPoisonResistance(AttributeData->PoisonResistance);
	SetBleedingResistance(AttributeData->BleedingResistance);
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Static)
// ═══════════════════════════════════════════════════════════════════════

FAttributeDataRow* UDefaultAttributeSet::GetAttributeData(const FName RowName)
{
	const UDataTable* Table = Content::GetTable(Tables::Attributes);
	if (!Table) return nullptr;
	const FString Ctx(TEXT("Looking_For_Data"));
	
	if (FAttributeDataRow* Direct = Table->FindRow<FAttributeDataRow>(RowName, Ctx))
	{
		return Direct;
	}
	return nullptr;
}

// ═══════════════════════════════════════════════════════════════════════
// CALCUL
// ═══════════════════════════════════════════════════════════════════════

float UDefaultAttributeSet::ConvertDamages(const FGameplayEffectModCallbackData& Data, const float Amount) const
{
    const FGameplayEffectSpec& Spec = Data.EffectSpec;
    
    // Lambda helper pour calculer la réduction d'armure
    auto CalculateArmorReduction = [](const float Armor, const float ArmorMultiplier = 1.0f) -> float
    {
        const float EffectiveArmor = Armor * ArmorMultiplier;
        return EffectiveArmor / (EffectiveArmor + 1000.0f);
    };
    
    // Lambda helper pour appliquer une résistance en pourcentage
    auto ApplyResistance = [Amount](const float Resistance) -> float
    {
        return FMath::Max(0.f, Amount * (1.0f - Resistance / 100.0f));
    };
    
    // Structure pour mapper les types de dégâts à leur logique de réduction
    struct FDamageTypeHandler
    {
        FGameplayTag Tag;
        TFunction<float()> CalculateReduction;
    };
    
    // Table de mapping des types de dégâts
    const TArray<FDamageTypeHandler> DamageHandlers = {
        // Dégâts physiques avec armure
        {
            DamageType::Flesh,
            [&]() -> float
            {
                const float Reduction = CalculateArmorReduction(GetArmor());
                return FMath::Max(0.f, Amount * (1.0f - Reduction));
            }
        },
        {
            DamageType::Penetration,
            [&]() -> float
            {
                const float Reduction = CalculateArmorReduction(GetArmor(), 0.3f);
                return FMath::Max(0.f, Amount * (1.0f - Reduction));
            }
        },
        // Résistances élémentaires
        { DamageType::Ice, [&]() { return ApplyResistance(GetIceResistance()); } },
        { DamageType::Fire, [&]() { return ApplyResistance(GetFireResistance()); } },
        { DamageType::Lightning, [&]() { return ApplyResistance(GetLightningResistance()); } },
        { DamageType::Holy, [&]() { return ApplyResistance(GetHolyResistance()); } },
        { DamageType::Poison, [&]() { return ApplyResistance(GetPoisonResistance()); } },
        { DamageType::Bleeding, [&]() { return ApplyResistance(GetBleedingResistance()); } }
    };
    
    // Chercher et appliquer la réduction correspondante
    for (const auto& [Tag, CalculateReduction] : DamageHandlers)
    {
        if (Spec.GetSetByCallerMagnitude(Tag, false) > 0.f)
        {
            return CalculateReduction();
        }
    }
    
    return Amount; // Aucun type trouvé, retourner les dégâts non modifiés
}

void UDefaultAttributeSet::EmpoweredMagnitude(FGameplayEffectSpec& Spec, const FGameplayTag& DamageTypeTag) const
{
	// Vérifier si ce tag existe dans les SetByCallerMagnitudes
	if (!Spec.SetByCallerTagMagnitudes.Contains(DamageTypeTag))
	{
		return;
	}
    
	// Récupérer la magnitude actuelle
	float BaseMagnitude = Spec.GetSetByCallerMagnitude(DamageTypeTag);
    
	if (BaseMagnitude <= 0.f)
	{
		return;
	}
    
	// Déterminer le multiplicateur selon le type de dégât
	float Multiplier = 1.0f;
    
	if (DamageTypeTag == DamageType::Flesh || DamageTypeTag == DamageType::Penetration)
	{
		Multiplier = GetPhysicDamageMultiplier();
	}
	else if (DamageTypeTag == DamageType::Ice || DamageTypeTag == DamageType::Fire || 
			 DamageTypeTag == DamageType::Lightning || DamageTypeTag == DamageType::Holy)
	{
		Multiplier = GetMagicDamageMultiplier();
	}
	else if (DamageTypeTag == DamageType::Poison || DamageTypeTag == DamageType::Bleeding)
	{
		Multiplier = GetAccuracyDamageMultiplier();
	}
    
	// Modifier la magnitude avec le multiplicateur
	Spec.SetSetByCallerMagnitude(DamageTypeTag, BaseMagnitude * Multiplier);
}

// ═══════════════════════════════════════════════════════════════════════════
// RÉPLICATION
// ═══════════════════════════════════════════════════════════════════════════

void UDefaultAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// ───────────────────────────────────────────────────────────────────────
	// BASE ATTRIBUTES
	// ───────────────────────────────────────────────────────────────────────
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, BaseHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, BaseMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, BaseStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, BaseFocus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, BaseHealthRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, BaseManaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, BaseStaminaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, BaseFocusRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, BasePhysicDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, BaseMagicDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, BaseAccuracyDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, BaseCriticalChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, BaseCriticalDamage, COND_None, REPNOTIFY_Always);

	// ───────────────────────────────────────────────────────────────────────
	// HEALTH
	// ───────────────────────────────────────────────────────────────────────
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, HealthRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, HealthBonus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, HealthRegenBonus, COND_None, REPNOTIFY_Always);
	// ───────────────────────────────────────────────────────────────────────
	// MANA
	// ───────────────────────────────────────────────────────────────────────
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, ManaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, ManaBonus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, ManaRegenBonus, COND_None, REPNOTIFY_Always);
	// ───────────────────────────────────────────────────────────────────────
	// STAMINA
	// ───────────────────────────────────────────────────────────────────────
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, StaminaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, StaminaBonus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, StaminaRegenBonus, COND_None, REPNOTIFY_Always);
	// ───────────────────────────────────────────────────────────────────────
	// FOCUS
	// ───────────────────────────────────────────────────────────────────────
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, Focus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, FocusRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, MaxFocus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, FocusBonus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, FocusRegenBonus, COND_None, REPNOTIFY_Always);
	// ───────────────────────────────────────────────────────────────────────
	// ATTACK
	// ───────────────────────────────────────────────────────────────────────
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, PhysicDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, MagicDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, AccuracyDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, CriticalChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, CriticalDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, PhysicDamageBonus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, MagicDamageBonus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, AccuracyDamageBonus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, CriticalChanceBonus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, CriticalDamageBonus, COND_None, REPNOTIFY_Always);
	// ───────────────────────────────────────────────────────────────────────
	// RESISTANCE
	// ───────────────────────────────────────────────────────────────────────
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, IceResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, HolyResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, PoisonResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, BleedingResistance, COND_None, REPNOTIFY_Always);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - BASE ATTRIBUTES
// ═══════════════════════════════════════════════════════════════════════════

void UDefaultAttributeSet::OnRep_BaseHealth(const FGameplayAttributeData& OldBaseHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, BaseHealth, OldBaseHealth);
}

void UDefaultAttributeSet::OnRep_BaseMana(const FGameplayAttributeData& OldBaseMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, BaseMana, OldBaseMana);
}

void UDefaultAttributeSet::OnRep_BaseStamina(const FGameplayAttributeData& OldBaseStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, BaseStamina, OldBaseStamina);
}

void UDefaultAttributeSet::OnRep_BaseFocus(const FGameplayAttributeData& OldBaseFocus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, BaseFocus, OldBaseFocus);
}

void UDefaultAttributeSet::OnRep_BaseHealthRegenRate(const FGameplayAttributeData& OldBaseHealthRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, BaseHealthRegenRate, OldBaseHealthRegenRate);
}

void UDefaultAttributeSet::OnRep_BaseManaRegenRate(const FGameplayAttributeData& OldBaseManaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, BaseManaRegenRate, OldBaseManaRegenRate);
}

void UDefaultAttributeSet::OnRep_BaseStaminaRegenRate(const FGameplayAttributeData& OldBaseStaminaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, BaseStaminaRegenRate, OldBaseStaminaRegenRate);
}

void UDefaultAttributeSet::OnRep_BaseFocusRegenRate(const FGameplayAttributeData& OldBaseFocusRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, BaseFocusRegenRate, OldBaseFocusRegenRate);
}

void UDefaultAttributeSet::OnRep_BasePhysicDamageMultiplier(const FGameplayAttributeData& OldBasePhysicDamageMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, BasePhysicDamageMultiplier, OldBasePhysicDamageMultiplier);
}

void UDefaultAttributeSet::OnRep_BaseMagicDamageMultiplier(const FGameplayAttributeData& OldBaseMagicDamageMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, BaseMagicDamageMultiplier, OldBaseMagicDamageMultiplier);
}

void UDefaultAttributeSet::OnRep_BaseAccuracyDamageMultiplier(const FGameplayAttributeData& OldBaseAccuracyDamageMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, BaseAccuracyDamageMultiplier, OldBaseAccuracyDamageMultiplier);
}

void UDefaultAttributeSet::OnRep_BaseCriticalChance(const FGameplayAttributeData& OldBaseCriticalChance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, BaseCriticalChance, OldBaseCriticalChance);
}

void UDefaultAttributeSet::OnRep_BaseCriticalDamage(const FGameplayAttributeData& OldBaseCriticalDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, BaseCriticalDamage, OldBaseCriticalDamage);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - HEALTH
// ═══════════════════════════════════════════════════════════════════════════

void UDefaultAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, Health, OldHealth);
}

void UDefaultAttributeSet::OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, HealthRegenRate, OldHealthRegenRate);
}

void UDefaultAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, MaxHealth, OldMaxHealth);
}

void UDefaultAttributeSet::OnRep_HealthBonus(const FGameplayAttributeData& OldHealthBonus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, HealthBonus, OldHealthBonus);
}

void UDefaultAttributeSet::OnRep_HealthRegenBonus(const FGameplayAttributeData& OldHealthRegenBonus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, HealthRegenBonus, OldHealthRegenBonus);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - MANA
// ═══════════════════════════════════════════════════════════════════════════

void UDefaultAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, Mana, OldMana);
}

void UDefaultAttributeSet::OnRep_ManaRegenRate(const FGameplayAttributeData& OldManaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, ManaRegenRate, OldManaRegenRate);
}

void UDefaultAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, MaxMana, OldMaxMana);
}

void UDefaultAttributeSet::OnRep_ManaBonus(const FGameplayAttributeData& OldManaBonus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, ManaBonus, OldManaBonus);
}

void UDefaultAttributeSet::OnRep_ManaRegenBonus(const FGameplayAttributeData& OldManaRegenBonus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, ManaRegenBonus, OldManaRegenBonus);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - STAMINA
// ═══════════════════════════════════════════════════════════════════════════

void UDefaultAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, Stamina, OldStamina);
}

void UDefaultAttributeSet::OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, StaminaRegenRate, OldStaminaRegenRate);
}

void UDefaultAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, MaxStamina, OldMaxStamina);
}

void UDefaultAttributeSet::OnRep_StaminaBonus(const FGameplayAttributeData& OldStaminaBonus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, StaminaBonus, OldStaminaBonus);
}

void UDefaultAttributeSet::OnRep_StaminaRegenBonus(const FGameplayAttributeData& OldStaminaRegenBonus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, StaminaRegenBonus, OldStaminaRegenBonus);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - FOCUS
// ═══════════════════════════════════════════════════════════════════════════

void UDefaultAttributeSet::OnRep_Focus(const FGameplayAttributeData& OldFocus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, Focus, OldFocus);
}

void UDefaultAttributeSet::OnRep_FocusRegenRate(const FGameplayAttributeData& OldFocusRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, FocusRegenRate, OldFocusRegenRate);
}

void UDefaultAttributeSet::OnRep_MaxFocus(const FGameplayAttributeData& OldMaxFocus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, MaxFocus, OldMaxFocus);
}

void UDefaultAttributeSet::OnRep_FocusBonus(const FGameplayAttributeData& OldFocusBonus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, FocusBonus, OldFocusBonus);
}

void UDefaultAttributeSet::OnRep_FocusRegenBonus(const FGameplayAttributeData& OldFocusRegenBonus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, FocusRegenBonus, OldFocusRegenBonus);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - PHYSIC DAMAGE MULTIPLIER
// ═══════════════════════════════════════════════════════════════════════════

void UDefaultAttributeSet::OnRep_PhysicDamageMultiplier(const FGameplayAttributeData& OldPhysicDamageMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, PhysicDamageMultiplier, OldPhysicDamageMultiplier);
}

void UDefaultAttributeSet::OnRep_PhysicDamageBonus(const FGameplayAttributeData& OldPhysicDamageBonus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, PhysicDamageBonus, OldPhysicDamageBonus);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - MAGIC DAMAGE MULTIPLIER
// ═══════════════════════════════════════════════════════════════════════════

void UDefaultAttributeSet::OnRep_MagicDamageMultiplier(const FGameplayAttributeData& OldMagicDamageMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, MagicDamageMultiplier, OldMagicDamageMultiplier);
}

void UDefaultAttributeSet::OnRep_MagicDamageBonus(const FGameplayAttributeData& OldMagicDamageBonus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, MagicDamageBonus, OldMagicDamageBonus);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - ACCURACY DAMAGE MULTIPLIER
// ═══════════════════════════════════════════════════════════════════════════

void UDefaultAttributeSet::OnRep_AccuracyDamageMultiplier(const FGameplayAttributeData& OldAccuracyDamageMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, AccuracyDamageMultiplier, OldAccuracyDamageMultiplier);
}

void UDefaultAttributeSet::OnRep_AccuracyDamageBonus(const FGameplayAttributeData& OldAccuracyDamageBonus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, AccuracyDamageBonus, OldAccuracyDamageBonus);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - CRITICAL CHANCE
// ═══════════════════════════════════════════════════════════════════════════

void UDefaultAttributeSet::OnRep_CriticalChance(const FGameplayAttributeData& OldCriticalChance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, CriticalChance, OldCriticalChance);
}

void UDefaultAttributeSet::OnRep_CriticalChanceBonus(const FGameplayAttributeData& OldCriticalChanceBonus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, CriticalChanceBonus, OldCriticalChanceBonus);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - CRITICAL DAMAGE
// ═══════════════════════════════════════════════════════════════════════════

void UDefaultAttributeSet::OnRep_CriticalDamage(const FGameplayAttributeData& OldCriticalDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, CriticalDamage, OldCriticalDamage);
}

void UDefaultAttributeSet::OnRep_CriticalDamageBonus(const FGameplayAttributeData& OldCriticalDamageBonus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, CriticalDamageBonus, OldCriticalDamageBonus);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - ARMOR
// ═══════════════════════════════════════════════════════════════════════════

void UDefaultAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, Armor, OldArmor);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - ICE RESISTANCE
// ═══════════════════════════════════════════════════════════════════════════

void UDefaultAttributeSet::OnRep_IceResistance(const FGameplayAttributeData& OldIceResistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, IceResistance, OldIceResistance);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - FIRE RESISTANCE
// ═══════════════════════════════════════════════════════════════════════════

void UDefaultAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, FireResistance, OldFireResistance);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - LIGHTNING RESISTANCE
// ═══════════════════════════════════════════════════════════════════════════

void UDefaultAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, LightningResistance, OldLightningResistance);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - HOLY RESISTANCE
// ═══════════════════════════════════════════════════════════════════════════

void UDefaultAttributeSet::OnRep_HolyResistance(const FGameplayAttributeData& OldHolyResistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, HolyResistance, OldHolyResistance);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - POISON RESISTANCE
// ═══════════════════════════════════════════════════════════════════════════

void UDefaultAttributeSet::OnRep_PoisonResistance(const FGameplayAttributeData& OldPoisonResistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, PoisonResistance, OldPoisonResistance);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION - BLEEDING RESISTANCE
// ═══════════════════════════════════════════════════════════════════════════

void UDefaultAttributeSet::OnRep_BleedingResistance(const FGameplayAttributeData& OldBleedingResistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, BleedingResistance, OldBleedingResistance);
}

// ═══════════════════════════════════════════════════════════════════════════
// PRE ATTRIBUTE CHANGE - Clamping des valeurs
// ═══════════════════════════════════════════════════════════════════════════

void UDefaultAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	// ───────────────────────────────────────────────────────────────────────
	// HEALTH : Ne peut pas être négatif ni dépasser MaxHealth
	// ───────────────────────────────────────────────────────────────────────
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}

	// ───────────────────────────────────────────────────────────────────────
	// MAX HEALTH : Ne peut pas être négatif
	// ───────────────────────────────────────────────────────────────────────
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f); // Minimum 1 HP
	}

	// ───────────────────────────────────────────────────────────────────────
	// MANA : Ne peut pas être négatif ni dépasser MaxMana
	// ───────────────────────────────────────────────────────────────────────
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
	}

	// ───────────────────────────────────────────────────────────────────────
	// MAX MANA : Ne peut pas être négatif
	// ───────────────────────────────────────────────────────────────────────
	else if (Attribute == GetMaxManaAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f); // Peut être 0 (certains archétypes n'ont pas de mana)
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// POST GAMEPLAY EFFECT EXECUTE - Logique post-modification
// ═══════════════════════════════════════════════════════════════════════════

void UDefaultAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// Récupérer l'attribut qui a été modifié
	FGameplayAttribute Attribute = Data.EvaluatedData.Attribute;

	// ───────────────────────────────────────────────────────────────────────
	// APPLY DAMAGES : Gestion des dégâts
	// ───────────────────────────────────────────────────────────────────────
	if (Attribute == GetApplyDamagesAttribute())
	{
		float Damages = GetApplyDamages() == 0 ? 0.f : FMath::Abs(GetApplyDamages());
		UE_LOG(LogTemp, Warning, TEXT("Damages: %f"), Damages);
		SetApplyDamages(0.f);
		Damages = -FMath::Abs(ConvertDamages(Data, Damages));
		const float NewHealth = GetHealth() + Damages;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
	}
		
	// ───────────────────────────────────────────────────────────────────────
	// HEALTH : Gérer la mort si Health atteint 0
	// ───────────────────────────────────────────────────────────────────────
	if (Attribute == GetHealthAttribute())
	{
		// Clamper Health entre 0 et MaxHealth
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));

		// Si le personnage est mort (Health = 0)
		if (GetHealth() <= 0.0f)
		{
			// TODO PHASE 10+ : Déclencher la mort du personnage
			// - Désactiver les inputs
			// - Jouer l'animation de mort
			// - Notifier le GameMode
			// - Appeler un événement Blueprint "OnDeath"
			
			UE_LOG(LogTemp, Warning, TEXT("ArchetypeAttributeSet: Personnage mort (Health = 0)"));
		}
	}
	
	// ───────────────────────────────────────────────────────────────────────
	// MANA : Clamper entre 0 et MaxMana
	// ───────────────────────────────────────────────────────────────────────
	else if (Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));
	}
	
	// ───────────────────────────────────────────────────────────────────────
	// STAMINA : Clamper entre 0 et MaxStamina
	// ───────────────────────────────────────────────────────────────────────
	else if (Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina()));
	}
	
	// ───────────────────────────────────────────────────────────────────────
	// FOCUS : Clamper entre 0 et MaxFocus
	// ───────────────────────────────────────────────────────────────────────
	else if (Attribute == GetFocusAttribute())
	{
		SetFocus(FMath::Clamp(GetFocus(), 0.0f, GetMaxFocus()));
	}
	
	// NOTE : Les clamps pour PhysicDamageMultiplier, MagicDamageMultiplier, AccuracyDamageMultiplier,
	// CriticalChance et CriticalDamage sont maintenant gérés directement dans leurs MMC respectifs
	
	// ───────────────────────────────────────────────────────────────────────
	// ARMOR : Min 0
	// ───────────────────────────────────────────────────────────────────────
	else if (Attribute == GetArmorAttribute())
	{
		SetArmor(FMath::Max(GetArmor(), 0.f));
	}
	
	// ───────────────────────────────────────────────────────────────────────
	// ICE RESISTANCE : Clamper entre -100 et 100
	// ───────────────────────────────────────────────────────────────────────
	else if (Attribute == GetIceResistanceAttribute())
	{
		SetIceResistance(FMath::Clamp(GetIceResistance(), -100.0f, 100.0f));
	}
	
	// ───────────────────────────────────────────────────────────────────────
	// FIRE RESISTANCE : Clamper entre -100 et 100
	// ───────────────────────────────────────────────────────────────────────
	else if (Attribute == GetFireResistanceAttribute())
	{
		SetFireResistance(FMath::Clamp(GetFireResistance(), -100.0f, 100.0f));
	}
	
	// ───────────────────────────────────────────────────────────────────────
	// LIGHTNING RESISTANCE : Clamper entre -100 et 100
	// ───────────────────────────────────────────────────────────────────────
	else if (Attribute == GetLightningResistanceAttribute())
	{
		SetLightningResistance(FMath::Clamp(GetLightningResistance(), -100.0f, 100.0f));
	}
	
	// ───────────────────────────────────────────────────────────────────────
	// HOLY RESISTANCE : Clamper entre -100 et 100
	// ───────────────────────────────────────────────────────────────────────
	else if (Attribute == GetHolyResistanceAttribute())
	{
		SetHolyResistance(FMath::Clamp(GetHolyResistance(), -100.0f, 100.0f));
	}
	
	// ───────────────────────────────────────────────────────────────────────
	// POISON RESISTANCE : Clamper entre -100 et 100
	// ───────────────────────────────────────────────────────────────────────
	else if (Attribute == GetPoisonResistanceAttribute())
	{
		SetPoisonResistance(FMath::Clamp(GetPoisonResistance(), -100.0f, 100.0f));
	}
	
	// ───────────────────────────────────────────────────────────────────────
	// BLEEDING RESISTANCE : Clamper entre -100 et 100
	// ───────────────────────────────────────────────────────────────────────
	else if (Attribute == GetBleedingResistanceAttribute())
	{
		SetBleedingResistance(FMath::Clamp(GetBleedingResistance(), -100.0f, 100.0f));
	}
}

void UDefaultAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
		
	// ───────────────────────────────────────────────────────────────────────
	// HEALTH
	// ───────────────────────────────────────────────────────────────────────
	if (Attribute == GetMaxHealthAttribute())
	{
		if (GetHealth() > NewValue)
		{
			SetHealth(NewValue);
		}
	}
	
	// ───────────────────────────────────────────────────────────────────────
	// MANA
	// ───────────────────────────────────────────────────────────────────────
	if (Attribute == GetMaxManaAttribute())
	{
		if (GetMana() > NewValue)
		{
			SetMana(NewValue);
		}
	}
	
	// ───────────────────────────────────────────────────────────────────────
	// STAMINA
	// ───────────────────────────────────────────────────────────────────────
	if (Attribute == GetMaxStaminaAttribute())
	{
		if (GetStamina() > NewValue)
		{
			SetStamina(NewValue);
		}
	}
	
	// ───────────────────────────────────────────────────────────────────────
	// FOCUS
	// ───────────────────────────────────────────────────────────────────────
	if (Attribute == GetMaxFocusAttribute())
	{
		if (GetFocus() > NewValue)
		{
			SetFocus(NewValue);
		}
	}
}
