// ═══════════════════════════════════════════════════════════════════════════
// GAME ITEM - BASE CLASS
// ═══════════════════════════════════════════════════════════════════════════

#pragma once
#include "CoreMinimal.h"
#include "Tools/Types/InventoryTypes.h"
#include "GameItem.generated.h"

class UInventory;
class ACharacter;
struct FGameItemData;

/**
 * Base class for all game items
 * Handles item logic and events (Authority only for now)
 * Can be optionally specified in FItemRowDetails
 * 
 * Outer: UInventoryManager if present, otherwise UInventory
 */
UCLASS(Blueprintable, BlueprintType)
class DARK_SCRIPT_API UGameItem : public UObject
{
	GENERATED_BODY()

public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	UGameItem();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack) override;

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Blueprint Events (Authority Only) ─────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/**
	 * Called when item is initialized (Authority only)
	 * @param Character - The character that owns this item
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Item|Events")
	void OnInitialized(ACharacter* Character);

	/**
	 * Called when item is destroyed (Authority only)
	 * @param Character - The character that owns this item
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Item|Events")
	void OnDestroyed(ACharacter* Character);

	/**
	 * Called when item is added to inventory (Authority only)
	 * @param Character - The character that owns this item
	 * @param Quantity - Amount added
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Item|Events")
	void OnAdded(ACharacter* Character, int32 Quantity);

	/**
	 * Called when item is removed from inventory (Authority only)
	 * @param Character - The character that owns this item
	 * @param Quantity - Amount removed
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Item|Events")
	void OnRemoved(ACharacter* Character, int32 Quantity);

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/** Get the inventory that owns this item */
	UFUNCTION(BlueprintPure, Category = "Item")
	UInventory* GetOwningInventory() const { return OwningInventory; }

	/** Get the item data associated with this runtime item */
	FGameItemData* GetItemData() const;

	/** Get the row details from DataTable */
	FItemRowDetails* GetRowDetails() const;

	/** Spawn visual item actor (on equip/use) */
	UFUNCTION(BlueprintCallable, Category = "Item")
	AVisualItem* SpawnVisualItem(const FTransform& SpawnTransform);

	/** Destroy visual item actor */
	UFUNCTION(BlueprintCallable, Category = "Item")
	void DestroyVisualItem();

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/** Initialize the item with its owning inventory */
	void Initialize(UInventory* InInventory, ACharacter* Character);

	/** Cleanup before destruction */
	void Cleanup(ACharacter* Character);

	/** Get the visual item actor */
	AVisualItem* GetVisualItem() const { return VisualItem; }

protected:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/** The inventory that owns this item */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UInventory> OwningInventory = nullptr;

	/** The spawned visual item actor (spawned on equip/use) */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Item")
	TObjectPtr<AVisualItem> VisualItem = nullptr;

	/** Has this item been initialized? */
	UPROPERTY()
	bool bIsInitialized = false;
};