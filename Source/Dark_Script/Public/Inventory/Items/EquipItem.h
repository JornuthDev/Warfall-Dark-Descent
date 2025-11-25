// ═══════════════════════════════════════════════════════════════════════════
// EQUIP ITEM - EQUIPMENT CLASS
// ═══════════════════════════════════════════════════════════════════════════

#pragma once
#include "CoreMinimal.h"
#include "Inventory/Items/GameItem.h"
#include "EquipItem.generated.h"

/**
 * Equipment item class (Weapons, Rings, Jewelry)
 * Handles equip/unequip logic with visual spawning
 */
UCLASS(Blueprintable, BlueprintType)
class DARK_SCRIPT_API UEquipItem : public UGameItem
{
	GENERATED_BODY()

public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Blueprint Events (Authority Only) ─────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/**
	 * Called when item is equipped (Authority only)
	 * @param Character - The character equipping this item
	 * Visual item should be spawned here
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Item|Events")
	void OnEquip(ACharacter* Character);

	/**
	 * Called when item is unequipped (Authority only)
	 * @param Character - The character unequipping this item
	 * Visual item should be destroyed here
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Item|Events")
	void OnUnequip(ACharacter* Character);

protected:
	virtual void OnEquip_Implementation(ACharacter* Character);
	virtual void OnUnequip_Implementation(ACharacter* Character);
};