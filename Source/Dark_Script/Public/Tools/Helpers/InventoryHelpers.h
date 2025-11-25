// ═══════════════════════════════════════════════════════════════════════════
// INVENTORY HELPERS - UTILITY FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Tools/Types/InventoryTypes.h"
#include "InventoryHelpers.generated.h"

class UInventory;
class UInventoryManager;

/**
 * Inventory helper functions
 * Blueprint-callable utility functions for common inventory operations
 */
UCLASS()
class DARK_SCRIPT_API UInventoryHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Item Validation ───────────────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/**
	 * Check if item row exists in DataTable
	 * @param DataTable - Items DataTable
	 * @param RowName - Row to check
	 * @return Exists
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Helpers")
	static bool IsValidItemRow(UDataTable* DataTable, FName RowName);

	/**
	 * Get item row details
	 * @param DataTable - Items DataTable
	 * @param RowName - Row name
	 * @param OutRowDetails - Output row details
	 * @return Success
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Helpers")
	static bool GetItemRowDetails(UDataTable* DataTable, FName RowName, FItemRowDetails& OutRowDetails);

	/**
	 * Check if item is stackable
	 * @param RowDetails - Item row details
	 * @return Is stackable
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Helpers")
	static bool IsItemStackable(const FItemRowDetails& RowDetails);

	/**
	 * Check if item can be rotated
	 * @param RowDetails - Item row details
	 * @return Can rotate
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Helpers")
	static bool CanItemRotate(const FItemRowDetails& RowDetails);

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Grid Helpers ──────────────────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/**
	 * Calculate total slots in grid
	 * @param GridSize - Grid dimensions
	 * @return Total slots
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Helpers")
	static int32 GetTotalSlotsInGrid(FIntPoint GridSize);

	/**
	 * Calculate used slots in container
	 * @param Inventory - Target inventory
	 * @param ContainerIndex - Container index
	 * @return Used slots count
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Helpers")
	static int32 GetUsedSlotsInContainer(UInventory* Inventory, int32 ContainerIndex);

	/**
	 * Calculate free slots in container
	 * @param Inventory - Target inventory
	 * @param ContainerIndex - Container index
	 * @return Free slots count
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Helpers")
	static int32 GetFreeSlotsInContainer(UInventory* Inventory, int32 ContainerIndex);

	/**
	 * Get percentage of container capacity used
	 * @param Inventory - Target inventory
	 * @param ContainerIndex - Container index
	 * @return Percentage (0.0 - 1.0)
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Helpers")
	static float GetContainerFillPercentage(UInventory* Inventory, int32 ContainerIndex);

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Item Queries ──────────────────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/**
	 * Count total quantity of item in inventory
	 * @param Inventory - Target inventory
	 * @param RowName - Item to count
	 * @param ContainerIndex - Container to search (-1 = all)
	 * @return Total quantity
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Helpers")
	static int32 CountItemInInventory(UInventory* Inventory, FName RowName, int32 ContainerIndex = -1);

	/**
	 * Check if inventory has item
	 * @param Inventory - Target inventory
	 * @param RowName - Item to check
	 * @param RequiredQuantity - Required quantity
	 * @param ContainerIndex - Container to search (-1 = all)
	 * @return Has item
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Helpers")
	static bool HasItem(UInventory* Inventory, FName RowName, int32 RequiredQuantity = 1, int32 ContainerIndex = -1);

	/**
	 * Get all items of specific type
	 * @param Inventory - Target inventory
	 * @param ItemType - Type to filter
	 * @param OutItems - Output items array
	 * @return Count
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Helpers")
	static int32 GetItemsByType(UInventory* Inventory, EItemType ItemType, TArray<FGameItemData>& OutItems);

	/**
	 * Get all items of specific quality
	 * @param Inventory - Target inventory
	 * @param Quality - Quality to filter
	 * @param OutItems - Output items array
	 * @return Count
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Helpers")
	static int32 GetItemsByQuality(UInventory* Inventory, EItemQuality Quality, TArray<FGameItemData>& OutItems);

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Inventory Management ──────────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/**
	 * Find inventory component on actor
	 * @param Actor - Actor to search
	 * @return Inventory component
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Helpers", meta = (DefaultToSelf = "Actor"))
	static UInventory* GetInventoryFromActor(AActor* Actor);

	/**
	 * Find inventory manager component on actor
	 * @param Actor - Actor to search
	 * @return Inventory manager component
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Helpers", meta = (DefaultToSelf = "Actor"))
	static UInventoryManager* GetInventoryManagerFromActor(AActor* Actor);

	/**
	 * Check if inventories can trade
	 * @param FromInventory - Source inventory
	 * @param ToInventory - Target inventory
	 * @return Can trade
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Helpers")
	static bool CanInventoriesTrade(UInventory* FromInventory, UInventory* ToInventory);

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Conversion Helpers ────────────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/**
	 * Convert item type to string
	 * @param ItemType - Type to convert
	 * @return String representation
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Helpers")
	static FString ItemTypeToString(EItemType ItemType);

	/**
	 * Convert weapon type to string
	 * @param WeaponType - Type to convert
	 * @return String representation
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Helpers")
	static FString WeaponTypeToString(EWeaponType WeaponType);

	/**
	 * Convert quality to string
	 * @param Quality - Quality to convert
	 * @return String representation
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Helpers")
	static FString QualityToString(EItemQuality Quality);

	/**
	 * Get color for item quality
	 * @param Quality - Item quality
	 * @return Color
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Helpers")
	static FLinearColor GetQualityColor(EItemQuality Quality);

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Debug Helpers ─────────────────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/**
	 * Print inventory contents to log
	 * @param Inventory - Inventory to print
	 * @param ContainerIndex - Container to print (-1 = all)
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Helpers|Debug")
	static void DebugPrintInventory(UInventory* Inventory, int32 ContainerIndex = -1);

	/**
	 * Draw debug grid visualization
	 * @param World - World context
	 * @param Inventory - Inventory to visualize
	 * @param ContainerIndex - Container to visualize
	 * @param Origin - World origin point
	 * @param Duration - Display duration
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Helpers|Debug", meta = (WorldContext = "World"))
	static void DebugDrawInventoryGrid(UObject* World, UInventory* Inventory, int32 ContainerIndex, FVector Origin, float Duration = 5.0f);
};