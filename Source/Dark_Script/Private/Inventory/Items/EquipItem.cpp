// ═══════════════════════════════════════════════════════════════════════════
// EQUIP ITEM - IMPLEMENTATION
// ═══════════════════════════════════════════════════════════════════════════

#include "Inventory/Items/EquipItem.h"
#include "Inventory/Actors/VisualItem.h"
#include "GameFramework/Character.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Blueprint Events (Authority Only) ─────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

void UEquipItem::OnEquip_Implementation(ACharacter* Character)
{
	// Override in Blueprint to customize behavior
	// Default: Spawn visual item at character location
	
	if (Character)
	{
		const FTransform SpawnTransform = Character->GetActorTransform();
		SpawnVisualItem(SpawnTransform);
	}
}

void UEquipItem::OnUnequip_Implementation(ACharacter* Character)
{
	// Override in Blueprint to customize behavior
	// Default: Destroy visual item
	
	DestroyVisualItem();
}