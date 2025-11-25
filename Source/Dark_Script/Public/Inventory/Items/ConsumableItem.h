// ═══════════════════════════════════════════════════════════════════════════
// CONSUMABLE ITEM - CONSUMABLE CLASS
// ═══════════════════════════════════════════════════════════════════════════

#pragma once
#include "CoreMinimal.h"
#include "Inventory/Items/GameItem.h"
#include "ConsumableItem.generated.h"

/**
 * Consumable item class (Potions, Food, etc.)
 * Handles use and effect application logic
 */
UCLASS(Blueprintable, BlueprintType)
class DARK_SCRIPT_API UConsumableItem : public UGameItem
{
	GENERATED_BODY()

public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Blueprint Events (Authority Only) ─────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/**
	 * Called when item is used (Authority only)
	 * @param Character - The character using this item
	 * Visual item can be spawned here (e.g., cheese visual)
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Item|Events")
	void OnUse(ACharacter* Character);

	/**
	 * Apply the consumable effect (Authority only)
	 * @param Character - The character receiving the effect
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Item|Events")
	void ApplyEffect(ACharacter* Character);

	/**
	 * Remove the consumable effect (Authority only)
	 * @param Character - The character losing the effect
	 * Used for temporary effects that need cleanup
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Item|Events")
	void RemoveEffect(ACharacter* Character);

protected:
	virtual void OnUse_Implementation(ACharacter* Character);
	virtual void ApplyEffect_Implementation(ACharacter* Character);
	virtual void RemoveEffect_Implementation(ACharacter* Character);
};