// ═══════════════════════════════════════════════════════════════════════════
// INVENTORY - MAIN COMPONENT
// ═══════════════════════════════════════════════════════════════════════════

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Tools/Types/InventoryTypes.h"
#include "Inventory.generated.h"

class UGameItem;
class ACharacter;

/**
 * Main inventory component (Tarkov-style grid system)
 * Can be attached to: PlayerController, PlayerState, Character, or any Actor
 * Has its own RPC for self-management (coffres/ennemis)
 * Can work with or without UInventoryManager
 * 
 * Base inventory: 40 slots (8×5 grid)
 * Upgradable via purchases/quests
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class DARK_SCRIPT_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	UInventory();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/** Initialize inventory with default containers */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void InitializeInventory();

	/** Add a new container to inventory */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddContainer(FName ContainerId, const FText& DisplayName, FIntPoint GridSize);

	/** Remove a container from inventory */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveContainer(int32 ContainerIndex);

	/** Get all containers */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<FContainerData>& GetContainers() { return Containers; }

	/** Get container by index */
	FContainerData* GetContainer(int32 ContainerIndex);

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Item Management) ───────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/**
	 * Add item to inventory (finds first available spot)
	 * @param RowName - Item row name from DataTable
	 * @param Quantity - Amount to add
	 * @param ContainerIndex - Target container (-1 = first available)
	 * @return Success
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Items")
	bool AddItem(FName RowName, int32 Quantity = 1, int32 ContainerIndex = -1);

	/**
	 * Add item at specific position
	 * @param RowName - Item row name from DataTable
	 * @param Quantity - Amount to add
	 * @param ContainerIndex - Target container
	 * @param Position - Grid position
	 * @param bRotated - Is item rotated
	 * @return Success
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Items")
	bool AddItemAt(FName RowName, int32 Quantity, int32 ContainerIndex, FIntPoint Position, bool bRotated = false);

	/**
	 * Remove item from inventory
	 * @param RowName - Item row name
	 * @param Quantity - Amount to remove
	 * @param ContainerIndex - Target container (-1 = search all)
	 * @return Success
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Items")
	bool RemoveItem(FName RowName, int32 Quantity = 1, int32 ContainerIndex = -1);

	/**
	 * Move item within or between containers
	 * @param FromContainer - Source container
	 * @param FromPosition - Source position
	 * @param ToContainer - Target container
	 * @param ToPosition - Target position
	 * @param bRotate - Should item be rotated
	 * @return Success
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Items")
	bool MoveItem(int32 FromContainer, FIntPoint FromPosition, int32 ToContainer, FIntPoint ToPosition, bool bRotate = false);

	/**
	 * Split item stack
	 * @param ContainerIndex - Container with item
	 * @param Position - Item position
	 * @param SplitQuantity - Amount to split
	 * @param ToContainer - Destination container
	 * @param ToPosition - Destination position
	 * @return Success
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Items")
	bool SplitItem(int32 ContainerIndex, FIntPoint Position, int32 SplitQuantity, int32 ToContainer, FIntPoint ToPosition);

	/**
	 * Swap two items
	 * @param Container1 - First container
	 * @param Position1 - First position
	 * @param Container2 - Second container
	 * @param Position2 - Second position
	 * @return Success
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Items")
	bool SwapItems(int32 Container1, FIntPoint Position1, int32 Container2, FIntPoint Position2);

	/**
	 * Stack items from source to target (adds to existing stack)
	 * @param FromContainer - Source container
	 * @param FromPosition - Source position
	 * @param ToContainer - Target container
	 * @param ToPosition - Target position (must have same item)
	 * @param Quantity - Amount to transfer (0 = all available)
	 * @return Success
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Items")
	bool StackItems(int32 FromContainer, FIntPoint FromPosition, int32 ToContainer, FIntPoint ToPosition, int32 Quantity = 0);

	/**
	 * Find item by row name
	 * @param RowName - Item to find
	 * @param OutContainerIndex - Found container index
	 * @param OutItemData - Found item data
	 * @return Success
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Items")
	bool FindItem(FName RowName, int32& OutContainerIndex, FGameItemData& OutItemData);

	/**
	 * Get item at position
	 * @param ContainerIndex - Container to check
	 * @param Position - Position to check
	 * @return Item data or nullptr
	 */
	FGameItemData* GetItemAt(int32 ContainerIndex, FIntPoint Position);

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Grid Management) ───────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/**
	 * Check if item can fit at position (Tarkov-style)
	 * @param ContainerIndex - Container to check
	 * @param Position - Position to check
	 * @param Size - Item size
	 * @param bRotated - Is item rotated
	 * @param IgnoreItemData - Item to ignore (for moves)
	 * @return Can fit
	 */
	bool CanItemFitAt(int32 ContainerIndex, FIntPoint Position, FIntPoint Size, bool bRotated, const FGameItemData* IgnoreItemData = nullptr) const;

	/**
	 * Find first available position for item
	 * @param ContainerIndex - Container to search (-1 = all)
	 * @param Size - Item size
	 * @param OutContainer - Found container
	 * @param OutPosition - Found position
	 * @param OutRotated - Should be rotated
	 * @return Success
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Grid")
	bool FindAvailablePosition(int32 ContainerIndex, FIntPoint Size, int32& OutContainer, FIntPoint& OutPosition, bool& OutRotated);

	/**
	 * Get all cells occupied by an item
	 * @param Position - Item position
	 * @param Size - Item size
	 * @param bRotated - Is rotated
	 * @return List of occupied cells
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Grid")
	TArray<FIntPoint> GetOccupiedCells(FIntPoint Position, FIntPoint Size, bool bRotated) const;

	/**
	 * Check if position is valid in container
	 * @param ContainerIndex - Container to check
	 * @param Position - Position to check
	 * @return Is valid
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Grid")
	bool IsValidPosition(int32 ContainerIndex, FIntPoint Position) const;

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── RPC Functions (Server) ────────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddItem(FName RowName, int32 Quantity, int32 ContainerIndex);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddItemAt(FName RowName, int32 Quantity, int32 ContainerIndex, FIntPoint Position, bool bRotated);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RemoveItem(FName RowName, int32 Quantity, int32 ContainerIndex);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveItem(int32 FromContainer, FIntPoint FromPosition, int32 ToContainer, FIntPoint ToPosition, bool bRotate);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SplitItem(int32 ContainerIndex, FIntPoint Position, int32 SplitQuantity, int32 ToContainer, FIntPoint ToPosition);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SwapItems(int32 Container1, FIntPoint Position1, int32 Container2, FIntPoint Position2);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StackItems(int32 FromContainer, FIntPoint FromPosition, int32 ToContainer, FIntPoint ToPosition, int32 Quantity);

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Delegates ─────────────────────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnItemAdded OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnItemRemoved OnItemRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnItemMoved OnItemMoved;

protected:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Internal Functions ────────────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/** Create runtime item if needed */
	UGameItem* CreateRuntimeItem(FGameItemData& ItemData);

	/** Destroy runtime item */
	void DestroyRuntimeItem(FGameItemData& ItemData);

	/** Try to stack item with existing stack */
	bool TryStackItem(FName RowName, int32& Quantity, int32 ContainerIndex);

	/** Get character owner */
	ACharacter* GetCharacterOwner() const;

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/** All inventory containers */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory")
	TArray<FContainerData> Containers;

	/** Reference to items DataTable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Config")
	TObjectPtr<UDataTable> ItemsDataTable;
};