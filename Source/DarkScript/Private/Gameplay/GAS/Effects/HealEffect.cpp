#include "Gameplay/GAS/Effects/HealEffect.h"
/*#include "Gameplay/GAS/AttributesSet.h"*/
#include "Utils/Libraries/DarkTags.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
UHealEffect::UHealEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo HealthModifier;
	
	/*HealthModifier.Attribute = UAttributesSet::GetIncomingHealingAttribute();*/
	
	HealthModifier.ModifierOp = EGameplayModOp::Additive;
	
	FSetByCallerFloat SetByCallerData;
	SetByCallerData.DataName = SetByCallerName::Health;
	HealthModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCallerData);
	Modifiers.Add(HealthModifier);
}
