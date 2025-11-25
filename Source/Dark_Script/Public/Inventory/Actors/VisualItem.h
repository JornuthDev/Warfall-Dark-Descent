// ═══════════════════════════════════════════════════════════════════════════
// VISUAL ITEM - WORLD REPRESENTATION
// ═══════════════════════════════════════════════════════════════════════════

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VisualItem.generated.h"

class UGameItem;

/**
 * Visual representation of an item in the world
 * Spawned by UGameItem when needed (equip/use)
 * Always has a reference to its owning UGameItem
 */
UCLASS(Blueprintable, BlueprintType)
class DARK_SCRIPT_API AVisualItem : public AActor
{
	GENERATED_BODY()

public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	AVisualItem();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/** Get the owning game item */
	UFUNCTION(BlueprintPure, Category = "Item")
	UGameItem* GetOwningGameItem() const { return OwningGameItem; }

	/** Set the owning game item (called by UGameItem on spawn) */
	void SetOwningGameItem(UGameItem* InGameItem);

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Blueprint Events ──────────────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/**
	 * Called when visual item is spawned
	 * @param Character - The character that owns this visual
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Item|Events")
	void OnVisualSpawned(ACharacter* Character);

	/**
	 * Called when visual item is about to be destroyed
	 * @param Character - The character that owns this visual
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Item|Events")
	void OnVisualDestroyed(ACharacter* Character);

protected:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/** Reference to the UGameItem that owns this visual */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UGameItem> OwningGameItem = nullptr;

	/** Root mesh component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
};