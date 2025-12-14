#include "Gameplay/AbilitySystem/Effects/DamageEffect.h"
/*#include "Gameplay/GAS/AttributesSet.h"*/
#include "Utils/Libraries/DarkTags.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
UDamageEffect::UDamageEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo HealthModifier;
	
	/*HealthModifier.Attribute = UAttributesSet::GetIncomingDamageAttribute();*/
	
	HealthModifier.ModifierOp = EGameplayModOp::Additive;
	
	FSetByCallerFloat SetByCallerData;
	SetByCallerData.DataName = SetByCallerName::Health;
	HealthModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCallerData);
	Modifiers.Add(HealthModifier);
}
