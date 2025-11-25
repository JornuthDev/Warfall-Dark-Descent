// ═══════════════════════════════════════════════════════════════════════════
// INVENTORY MANAGER - PLAYER INVENTORY DISPATCHER
// ═══════════════════════════════════════════════════════════════════════════

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Tools/Types/InventoryTypes.h"
#include "InventoryManager.generated.h"

class UInventory;

/**
 * Inventory Manager Component
 * Manages multiple UInventory components and dispatches RPC
 * Used client-side only (typically on PlayerController)
 * Necessary because some inventories are in PlayerState (can't call their own RPC)
 * 
 * Workflow:
 * - Player calls function on Manager
 * - Manager dispatches Server RPC
 * - Server RPC calls target Inventory function
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class DARK_SCRIPT_API UInventoryManager : public UActorComponent
{
	GENERATED_BODY()

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryManagerInitialized);	
	
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	UInventoryManager();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/**
	 * Add item to inventory (finds first available spot)
	 * @param TargetInventory - Target inventory
	 * @param RowName - Item row name from DataTable
	 * @param Quantity - Amount to add
	 * @param ContainerIndex - Target container (-1 = first available)
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Manager")
	void AddItemToInventory(UInventory* TargetInventory, FName RowName, int32 Quantity = 1, int32 ContainerIndex = -1);

	/**
	 * Remove item from inventory
	 * @param TargetInventory - Target inventory
	 * @param RowName - Item row name
	 * @param Quantity - Amount to remove
	 * @param ContainerIndex - Target container (-1 = search all)
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Manager")
	void RemoveItemFromInventory(UInventory* TargetInventory, FName RowName, int32 Quantity = 1, int32 ContainerIndex = -1);

	/**
	 * Move item within or between inventories
	 * @param FromInventory - Source inventory
	 * @param FromContainer - Source container
	 * @param FromPosition - Source position
	 * @param ToInventory - Target inventory
	 * @param ToContainer - Target container
	 * @param ToPosition - Target position
	 * @param bRotate - Should item be rotated
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Manager")
	void MoveItemBetweenInventories(
		UInventory* FromInventory, int32 FromContainer, FIntPoint FromPosition,
		UInventory* ToInventory, int32 ToContainer, FIntPoint ToPosition, bool bRotate = false);

	/**
	 * Split item between inventories
	 * @param SourceInventory - Source inventory
	 * @param ContainerIndex - Container with item
	 * @param Position - Item position
	 * @param SplitQuantity - Amount to split
	 * @param TargetInventory - Destination inventory
	 * @param ToContainer - Destination container
	 * @param ToPosition - Destination position
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Manager")
	void SplitItemBetweenInventories(
		UInventory* SourceInventory, int32 ContainerIndex, FIntPoint Position, int32 SplitQuantity,
		UInventory* TargetInventory, int32 ToContainer, FIntPoint ToPosition);

	/**
	 * Swap items between inventories
	 * @param Inventory1 - First inventory
	 * @param Container1 - First container
	 * @param Position1 - First position
	 * @param Inventory2 - Second inventory
	 * @param Container2 - Second container
	 * @param Position2 - Second position
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Manager")
	void SwapItemsBetweenInventories(
		UInventory* Inventory1, int32 Container1, FIntPoint Position1,
		UInventory* Inventory2, int32 Container2, FIntPoint Position2);

	
	UFUNCTION(BlueprintPure)
	UInventory* GetBackpack() { return ManagedInventories[0]; }
	UFUNCTION(BlueprintPure)
	UInventory* GetStorage() { return ManagedInventories[1]; }
	
private:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Replication Callbacks ─────────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/** Called on clients when ManagedInventories is replicated */
	UFUNCTION()
	void OnRep_Initialized()
	{
		OnInitialized.Broadcast();
	}
	
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── RPC Functions (Server) ────────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddItemToInventory(UInventory* TargetInventory, FName RowName, int32 Quantity, int32 ContainerIndex);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RemoveItemFromInventory(UInventory* TargetInventory, FName RowName, int32 Quantity, int32 ContainerIndex);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveItemBetweenInventories(
		UInventory* FromInventory, int32 FromContainer, FIntPoint FromPosition,
		UInventory* ToInventory, int32 ToContainer, FIntPoint ToPosition, bool bRotate);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SplitItemBetweenInventories(
		UInventory* SourceInventory, int32 ContainerIndex, FIntPoint Position, int32 SplitQuantity,
		UInventory* TargetInventory, int32 ToContainer, FIntPoint ToPosition);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SwapItemsBetweenInventories(
		UInventory* Inventory1, int32 Container1, FIntPoint Position1,
		UInventory* Inventory2, int32 Container2, FIntPoint Position2);

private:
	
protected:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────
	UPROPERTY(BlueprintAssignable)
	FOnInventoryManagerInitialized OnInitialized;
	
	/** Registered inventories managed by this component */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory")
	TArray<TObjectPtr<UInventory>> ManagedInventories;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FContainerData> Backpack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FContainerData> Storage;
	UPROPERTY(ReplicatedUsing=OnRep_Initialized)
	bool bInitialized = false;
};