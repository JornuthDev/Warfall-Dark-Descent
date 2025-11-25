// ═══════════════════════════════════════════════════════════════════════════
// WEAPON BASE - WEAPON VISUAL WITH GAMEPLAY
// ═══════════════════════════════════════════════════════════════════════════

#pragma once
#include "CoreMinimal.h"
#include "Inventory/Actors/VisualItem.h"
#include "WeaponBase.generated.h"

/**
 * Weapon visual actor with gameplay logic
 * Handles animations, damage, effects, etc.
 */
UCLASS(Blueprintable, BlueprintType)
class DARK_SCRIPT_API AWeaponBase : public AVisualItem
{
	GENERATED_BODY()

public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	AWeaponBase();

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Blueprint Events ──────────────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/**
	 * Called when weapon attack is triggered
	 * @param Character - The character attacking
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|Events")
	void OnAttack(ACharacter* Character);

	/**
	 * Called when weapon needs to deal damage
	 * @param Character - The character wielding the weapon
	 * @param Target - The target actor
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|Events")
	void OnDealDamage(ACharacter* Character, AActor* Target);

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/** Get weapon damage from item data */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	float GetWeaponDamage() const;

	/** Get weapon resource cost from item data */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	float GetResourceCost() const;

protected:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/** Skeletal mesh for animated weapons */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;
};