// ═══════════════════════════════════════════════════════════════════════════
// CONSUMABLE ITEM - IMPLEMENTATION
// ═══════════════════════════════════════════════════════════════════════════

#include "Inventory/Items/ConsumableItem.h"
#include "GameFramework/Character.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Blueprint Events (Authority Only) ─────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

void UConsumableItem::OnUse_Implementation(ACharacter* Character)
{
	// Override in Blueprint to customize behavior
	// Default: Apply effect immediately
	
	ApplyEffect(Character);
}

void UConsumableItem::ApplyEffect_Implementation(ACharacter* Character)
{
	// Override in Blueprint
	// Example: Heal character, add buff, etc.
}

void UConsumableItem::RemoveEffect_Implementation(ACharacter* Character)
{
	// Override in Blueprint
	// Example: Remove temporary buff
}