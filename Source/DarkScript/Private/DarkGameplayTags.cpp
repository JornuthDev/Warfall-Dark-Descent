#include "DarkGameplayTags.h"

#include "Engine/EngineTypes.h"
#include "GameplayTagsManager.h"
#include "Gameplay/AbilitySystem/Attributes/PlayerSet.h"
#include "Gameplay/AbilitySystem/Attributes/PropertyCauser.h"
#include "Gameplay/AbilitySystem/Attributes/Mutual/HealthSet.h"
#include "Gameplay/AbilitySystem/Attributes/Mutual/PropertyInstigator.h"
#include "Gameplay/AbilitySystem/Attributes/Mutual/ResistancesSet.h"
#include "Gameplay/AbilitySystem/Attributes/Resources/FocusSet.h"
#include "Gameplay/AbilitySystem/Attributes/Resources/ManaSet.h"
#include "Gameplay/AbilitySystem/Attributes/Resources/StaminaSet.h"

namespace DarkGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Causer_Type_Physic_Melee, "Ability.Causer.Type.Physic.Melee");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Causer_Type_Physic_Ranged, "Ability.Causer.Type.Physic.Ranged");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Causer_Type_Magical, "Ability.Causer.Type.Magical");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Causer_Type_Healing, "Ability.Causer.Type.Healing");

	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Causer_PropertyType_Damage, "Ability.Causer.PropertyType.Damage");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Causer_PropertyType_ArmorPenetration, "Ability.Causer.PropertyType.ArmorPenetration");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Causer_PropertyType_Multiplier_FleshMultiplier, "Ability.Causer.PropertyType.Multiplier.FleshMultiplier");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Causer_PropertyType_Multiplier_PenetrationMultiplier, "Ability.Causer.PropertyType.Multiplier.PenetrationMultiplier");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Causer_PropertyType_Multiplier_IceMultiplier, "Ability.Causer.PropertyType.Multiplier.IceMultiplier");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Causer_PropertyType_Multiplier_FireMultiplier, "Ability.Causer.PropertyType.Multiplier.FireMultiplier");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Causer_PropertyType_Multiplier_LightningMultiplier, "Ability.Causer.PropertyType.Multiplier.LightningMultiplier");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Causer_PropertyType_Multiplier_HolyMultiplier, "Ability.Causer.PropertyType.Multiplier.HolyMultiplier");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Causer_PropertyType_Multiplier_PoisonMultiplier, "Ability.Causer.PropertyType.Multiplier.PoisonMultiplier");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Causer_PropertyType_Multiplier_BleedingMultiplier, "Ability.Causer.PropertyType.Multiplier.BleedingMultiplier");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Causer_PropertyType_Multiplier_HealingMultiplier, "Ability.Causer.PropertyType.Multiplier.HealingMultiplier");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Causer_PropertyType_Multiplier_SpeedMultiplier, "Ability.Causer.PropertyType.Multiplier.SpeedMultiplier");

	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_HealthMax, "Ability.Character.PropertyType.HealthMax");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_HealthRegenRate, "Ability.Character.PropertyType.HealthRegenRate");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Resource_StaminaMax, "Ability.Character.PropertyType.Resource.StaminaMax");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Resource_StaminaRegenRate, "Ability.Character.PropertyType.Resource.StaminaRegenRate");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Resource_ManaMax, "Ability.Character.PropertyType.Resource.ManaMax");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Resource_ManaRegenRate, "Ability.Character.PropertyType.Resource.ManaRegenRate");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Resource_FocusMax, "Ability.Character.PropertyType.Resource.FocusMax");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Resource_FocusRegenRate, "Ability.Character.PropertyType.Resource.FocusRegenRate");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Perk_Vitality, "Ability.Character.PropertyType.Perk.Vitality");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Perk_Strength, "Ability.Character.PropertyType.Perk.Strength");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Perk_Accuracy, "Ability.Character.PropertyType.Perk.Accuracy");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Perk_Intelligence, "Ability.Character.PropertyType.Perk.Intelligence");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Perk_Mentality, "Ability.Character.PropertyType.Perk.Mentality");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Resistance_Armor, "Ability.Character.PropertyType.Resistance.Armor");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Resistance_Ice, "Ability.Character.PropertyType.Resistance.Ice");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Resistance_Fire, "Ability.Character.PropertyType.Resistance.Fire");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Resistance_Lightning, "Ability.Character.PropertyType.Resistance.Lightning");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Resistance_Holy, "Ability.Character.PropertyType.Resistance.Holy");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Resistance_Poison, "Ability.Character.PropertyType.Resistance.Poison");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Resistance_Bleeding, "Ability.Character.PropertyType.Resistance.Bleeding");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Damage_Physic, "Ability.Character.PropertyType.Damage.Physic");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Damage_Ranged, "Ability.Character.PropertyType.Damage.Ranged");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Damage_Magic, "Ability.Character.PropertyType.Damage.Magic");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Damage_Physic_CriticalChance, "Ability.Character.PropertyType.Damage.Physic.CriticalChanced");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Damage_Physic_CriticalPower, "Ability.Character.PropertyType.Damage.Physic.CriticalPower");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Damage_Magic_CriticalChance, "Ability.Character.PropertyType.Damage.Magic.CriticalChance");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Damage_Magic_CriticalPower, "Ability.Character.PropertyType.Damage.Magic.CriticalPower");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Heal, "Ability.Character.PropertyType.Heal");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Heal_CriticalChance, "Ability.Character.PropertyType.Heal.CriticalChance");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Character_PropertyType_Heal_CriticalPower, "Ability.Character.PropertyType.Heal.CriticalPower");
	
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Damage_Type_Flesh_Melee, "Ability.Damage.Type.Flesh.Melee");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Damage_Type_Flesh_Ranged, "Ability.Damage.Type.Flesh.Ranged");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Damage_Type_Penetration_Melee, "Ability.Damage.Type.Penetration.Melee");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Damage_Type_Penetration_Ranged, "Ability.Damage.Type.Penetration.Ranged");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Damage_Type_Ice, "Ability.Damage.Type.Ice");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Damage_Type_Fire, "Ability.Damage.Type.Fire");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Damage_Type_Lightning, "Ability.Damage.Type.Lightning");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Damage_Type_Holy, "Ability.Damage.Type.Holy");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Damage_Type_Poison, "Ability.Damage.Type.Poison");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Damage_Type_Bleeding, "Ability.Damage.Type.Bleeding");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Duration, "SetByCaller.Duration", "SetByCaller tag used by duration of gameplay effects.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Modifier, "SetByCaller.Modifier", "SetByCaller tag used by modifier gameplay effects.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Damage, "SetByCaller.Damage", "SetByCaller tag used by damage gameplay effects.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Heal, "SetByCaller.Heal", "SetByCaller tag used by healing gameplay effects.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Modifier_Resistance, "Modifier.Resistance", "Modifier tag used by resistance");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Modifier_MultiplierType, "Modifier.MultiplierType", "Modifier tag used by multiplierType");

	FGameplayTag FindTagByString(const FString& TagString, const bool bMatchPartialString)
	{
		const UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
		FGameplayTag Tag = Manager.RequestGameplayTag(FName(*TagString), false);
		
		if (!Tag.IsValid() && bMatchPartialString)
		{
			FGameplayTagContainer AllTags;
			Manager.RequestAllGameplayTags(AllTags, true);
			
			for (const FGameplayTag& TestTag : AllTags)
			{
				if (TestTag.ToString().Contains(TagString))
				{
					Tag = TestTag;
					break;
				}
			}
		}
		return Tag;
	}

	FGameplayAttribute FindAttributeByTag(const FGameplayTag& Tag)
	{
		TMap<FGameplayTag, FGameplayAttribute> TagToAttributeMap{
		{TAG_Ability_Character_PropertyType_HealthMax, UHealthSet::GetHealthMaxAttribute()},
		{TAG_Ability_Character_PropertyType_HealthRegenRate, UHealthSet::GetHealthRegenRateAttribute()},
		{TAG_Ability_Character_PropertyType_Resource_ManaMax, UManaSet::GetManaMaxAttribute()},
		{TAG_Ability_Character_PropertyType_Resource_ManaRegenRate, UManaSet::GetManaRegenRateAttribute()},
		{TAG_Ability_Character_PropertyType_Resource_StaminaMax, UStaminaSet::GetStaminaMaxAttribute()},
		{TAG_Ability_Character_PropertyType_Resource_StaminaRegenRate, UStaminaSet::GetStaminaRegenRateAttribute()},
		{TAG_Ability_Character_PropertyType_Resource_FocusMax, UFocusSet::GetFocusMaxAttribute()},
		{TAG_Ability_Character_PropertyType_Resource_FocusRegenRate, UFocusSet::GetFocusRegenRateAttribute()},
		{TAG_Ability_Character_PropertyType_Perk_Vitality, UPlayerSet::GetVitalityAttribute()},
		{TAG_Ability_Character_PropertyType_Perk_Strength, UPlayerSet::GetStrengthAttribute()},
		{TAG_Ability_Character_PropertyType_Perk_Accuracy, UPlayerSet::GetAccuracyAttribute()},
		{TAG_Ability_Character_PropertyType_Perk_Intelligence, UPlayerSet::GetIntelligenceAttribute()},
		{TAG_Ability_Character_PropertyType_Perk_Mentality, UPlayerSet::GetMentalityAttribute()},
		{TAG_Ability_Character_PropertyType_Resistance_Armor, UResistancesSet::GetArmorAttribute()},
		{TAG_Ability_Character_PropertyType_Resistance_Ice,	UResistancesSet::GetIceAttribute()},
		{TAG_Ability_Character_PropertyType_Resistance_Fire, UResistancesSet::GetFireAttribute()},
		{TAG_Ability_Character_PropertyType_Resistance_Lightning, UResistancesSet::GetLightningAttribute()},
		{TAG_Ability_Character_PropertyType_Resistance_Holy, UResistancesSet::GetHolyAttribute()},
		{TAG_Ability_Character_PropertyType_Resistance_Poison, UResistancesSet::GetPoisonAttribute()},
		{TAG_Ability_Character_PropertyType_Resistance_Bleeding, UResistancesSet::GetBleedingAttribute()},
		{TAG_Ability_Character_PropertyType_Damage_Physic, UPropertyInstigator::GetPhysicDamageMultiplierAttribute()},
		{TAG_Ability_Character_PropertyType_Damage_Ranged, UPropertyInstigator::GetRangedDamageMultiplierAttribute()},
		{TAG_Ability_Character_PropertyType_Damage_Magic, UPropertyInstigator::GetMagicDamageMultiplierAttribute()},
		{TAG_Ability_Character_PropertyType_Damage_Physic_CriticalChance, UPropertyInstigator::GetPhysicCriticalChanceAttribute()},
		{TAG_Ability_Character_PropertyType_Damage_Physic_CriticalPower, UPropertyInstigator::GetPhysicCriticalPowerAttribute()},
		{TAG_Ability_Character_PropertyType_Damage_Magic_CriticalChance, UPropertyInstigator::GetMagicCriticalChanceAttribute()},
		{TAG_Ability_Character_PropertyType_Damage_Magic_CriticalPower,	UPropertyInstigator::GetMagicCriticalPowerAttribute()},
		{TAG_Ability_Character_PropertyType_Heal, UPropertyInstigator::GetHealingMultiplierAttribute()},
		{TAG_Ability_Character_PropertyType_Heal_CriticalChance, UPropertyInstigator::GetHealCriticalChanceAttribute()},
		{TAG_Ability_Character_PropertyType_Heal_CriticalPower,	UPropertyInstigator::GetHealCriticalPowerAttribute()},
		{TAG_Ability_Causer_PropertyType_Damage, UPropertyCauser::GetDamageAttribute()},
		{TAG_Ability_Causer_PropertyType_ArmorPenetration, UPropertyCauser::GetArmorPenetrationAttribute()},
		{TAG_Ability_Causer_PropertyType_Multiplier_FleshMultiplier, UPropertyCauser::GetFleshMultiplierAttribute()},
		{TAG_Ability_Causer_PropertyType_Multiplier_PenetrationMultiplier, UPropertyCauser::GetPenetrationMultiplierAttribute()},
		{TAG_Ability_Causer_PropertyType_Multiplier_IceMultiplier, UPropertyCauser::GetIceMultiplierAttribute()},
		{TAG_Ability_Causer_PropertyType_Multiplier_FireMultiplier, UPropertyCauser::GetFireMultiplierAttribute()},
		{TAG_Ability_Causer_PropertyType_Multiplier_LightningMultiplier, UPropertyCauser::GetLightningMultiplierAttribute()},
		{TAG_Ability_Causer_PropertyType_Multiplier_HolyMultiplier, UPropertyCauser::GetHolyMultiplierAttribute()},
		{TAG_Ability_Causer_PropertyType_Multiplier_PoisonMultiplier, UPropertyCauser::GetPoisonMultiplierAttribute()},
		{TAG_Ability_Causer_PropertyType_Multiplier_BleedingMultiplier, UPropertyCauser::GetBleedingMultiplierAttribute()},
		{TAG_Ability_Causer_PropertyType_Multiplier_HealingMultiplier, UPropertyCauser::GetHealingMultiplierAttribute()},
		{TAG_Ability_Causer_PropertyType_Multiplier_SpeedMultiplier, UPropertyCauser::GetSpeedMultiplierAttribute()},
		};
		
		return *TagToAttributeMap.Find(Tag);
	}
}
