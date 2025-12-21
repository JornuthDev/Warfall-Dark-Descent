#pragma once

namespace LevelRules
{
	constexpr float StartLevel = 1;
	constexpr float MaxLevel = 50;
	constexpr float RequiredExpForLevel = 50;
	/** The higher the power, the harder the experience curve is */
	constexpr float ExperienceCurvePow = 2.4;
}

namespace AttributeRules
{
	constexpr float HealthMaxPerPerk = 5;
	constexpr float HealthRegenRatePerPerk = 0.001;
	constexpr float ManaMaxPerPerk = 2;
	constexpr float ManaRegenRatePerPerk = 0.0005;
	constexpr float StaminaMaxPerPerk = 2;
	constexpr float StaminaRegenRatePerPerk = 0.0005;
	constexpr float FocusMaxPerPerk = 2;
	constexpr float FocusRegenRatePerPerk = 0.0005;
	
	constexpr float PhysicDamagePerPerk = 0.005;
	constexpr float RangedDamagePerPerk = 0.005;
	constexpr float MagicDamagePerPerk = 0.005;
	
	constexpr float PhysicCriticalChancePerPerk = 0.0025;
	constexpr float PhysicCriticalPowerPerPerk = 0.0033;
	constexpr float MagicCriticalChancePerPerk = 0.0025;
	constexpr float MagicCriticalPowerPerPerk = 0.0033;
	
	constexpr float HealingPerPerk = 0.005;
	constexpr float HealCriticalChancePerPerk = 0.0025;
	constexpr float HealCriticalPowerPerPerk = 0.0033;
}