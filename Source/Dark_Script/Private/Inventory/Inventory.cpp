// ═══════════════════════════════════════════════════════════════════════════
// INVENTORY - IMPLEMENTATION
// ═══════════════════════════════════════════════════════════════════════════

#include "Inventory/Inventory.h"
#include "Inventory/Items/GameItem.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Engine/DataTable.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

UInventory::UInventory()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UInventory::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UInventory, Containers);
}

void UInventory::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetOwnerRole() == ROLE_Authority)
	{
		InitializeInventory();
	}
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

void UInventory::InitializeInventory()
{
	if (Containers.Num() > 0)
	{
		return; // Already initialized
	}

	// Create default container (8×5 = 40 slots)
	AddContainer(FName("MainInventory"), FText::FromString(TEXT("Inventory")), FIntPoint(8, 5));
}

bool UInventory::AddContainer(FName ContainerId, const FText& DisplayName, FIntPoint GridSize)
{
	if (!GetOwner()->HasAuthority())
	{
		return false;
	}

	// Check if container already exists
	for (const FContainerData& Container : Containers)
	{
		if (Container.ContainerId == ContainerId)
		{
			return false;
		}
	}

	FContainerData NewContainer(ContainerId, DisplayName, GridSize);
	NewContainer.Items.OwningInventory = this;
	Containers.Add(NewContainer);
	
	return true;
}

bool UInventory::RemoveContainer(int32 ContainerIndex)
{
	if (!GetOwner()->HasAuthority())
	{
		return false;
	}

	if (!Containers.IsValidIndex(ContainerIndex))
	{
		return false;
	}

	// Cleanup all runtime items in container
	FContainerData& Container = Containers[ContainerIndex];
	for (FGameItemDataEntry& Entry : Container.Items.Items)
	{
		DestroyRuntimeItem(Entry.ItemData);
	}

	Containers.RemoveAt(ContainerIndex);
	return true;
}

FContainerData* UInventory::GetContainer(int32 ContainerIndex)
{
	if (!Containers.IsValidIndex(ContainerIndex))
	{
		return nullptr;
	}
	return &Containers[ContainerIndex];
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Item Management) ───────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

bool UInventory::AddItem(FName RowName, int32 Quantity, int32 ContainerIndex)
{
	if (!GetOwner()->HasAuthority())
	{
		Server_AddItem(RowName, Quantity, ContainerIndex);
		return true; // Optimistic
	}

	if (!ItemsDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("UInventory::AddItem - ItemsDataTable is null!"));
		return false;
	}

	// Get item details
	FItemRowDetails* RowDetails = ItemsDataTable->FindRow<FItemRowDetails>(RowName, TEXT("AddItem"));
	if (!RowDetails)
	{
		UE_LOG(LogTemp, Error, TEXT("UInventory::AddItem - Item not found: %s"), *RowName.ToString());
		return false;
	}

	// Try to stack with existing items first
	if (RowDetails->MaxStackSize > 1)
	{
		if (TryStackItem(RowName, Quantity, ContainerIndex))
		{
			if (Quantity <= 0)
			{
				return true; // All stacked
			}
		}
	}

	// Find available position for remaining quantity
	while (Quantity > 0)
	{
		int32 TargetContainer = ContainerIndex;
		FIntPoint TargetPosition;
		bool bRotated = false;

		if (!FindAvailablePosition(ContainerIndex, RowDetails->ObjectSize2D, TargetContainer, TargetPosition, bRotated))
		{
			UE_LOG(LogTemp, Warning, TEXT("UInventory::AddItem - No space available for: %s"), *RowName.ToString());
			return false;
		}

		// Create new item
		int32 StackQuantity = FMath::Min(Quantity, RowDetails->MaxStackSize);
		
		if (!AddItemAt(RowName, StackQuantity, TargetContainer, TargetPosition, bRotated))
		{
			return false;
		}

		Quantity -= StackQuantity;
	}

	return true;
}

bool UInventory::AddItemAt(FName RowName, int32 Quantity, int32 ContainerIndex, FIntPoint Position, bool bRotated)
{
	if (!GetOwner()->HasAuthority())
	{
		Server_AddItemAt(RowName, Quantity, ContainerIndex, Position, bRotated);
		return true; // Optimistic
	}

	if (!ItemsDataTable || !Containers.IsValidIndex(ContainerIndex))
	{
		return false;
	}

	FItemRowDetails* RowDetails = ItemsDataTable->FindRow<FItemRowDetails>(RowName, TEXT("AddItemAt"));
	if (!RowDetails)
	{
		return false;
	}

	// Check if position is valid and available
	if (!CanItemFitAt(ContainerIndex, Position, RowDetails->ObjectSize2D, bRotated, nullptr))
	{
		return false;
	}

	// Create item data
	FGameItemData NewItemData;
	NewItemData.ItemRowHandle.DataTable = ItemsDataTable;
	NewItemData.ItemRowHandle.RowName = RowName;
	NewItemData.Quantity = FMath::Clamp(Quantity, 1, RowDetails->MaxStackSize);
	NewItemData.ContainerIndex = ContainerIndex;
	NewItemData.GridPosition = FInventoryGridPosition(Position, bRotated);
	NewItemData.ObjectSize2D = RowDetails->ObjectSize2D;
	NewItemData.bIsRotated = bRotated;
	NewItemData.Durability = RowDetails->MaxDurability;

	// Create runtime item if needed
	if (RowDetails->ItemClass)
	{
		NewItemData.RuntimeItem = CreateRuntimeItem(NewItemData);
	}

	// Add to container
	FGameItemDataEntry NewEntry;
	NewEntry.ItemData = NewItemData;
	Containers[ContainerIndex].Items.Items.Add(NewEntry);
	Containers[ContainerIndex].Items.MarkItemDirty(NewEntry);

	// Broadcast event
	OnItemAdded.Broadcast(NewItemData, ContainerIndex, true);

	return true;
}

bool UInventory::RemoveItem(FName RowName, int32 Quantity, int32 ContainerIndex)
{
	if (!GetOwner()->HasAuthority())
	{
		Server_RemoveItem(RowName, Quantity, ContainerIndex);
		return true; // Optimistic
	}

	int32 RemainingToRemove = Quantity;
	bool bRemovedAny = false;

	// Determine containers to search
	TArray<int32> ContainersToSearch;
	if (ContainerIndex >= 0 && Containers.IsValidIndex(ContainerIndex))
	{
		ContainersToSearch.Add(ContainerIndex);
	}
	else
	{
		for (int32 i = 0; i < Containers.Num(); i++)
		{
			ContainersToSearch.Add(i);
		}
	}

	// Search and remove from stacks
	for (int32 SearchContainer : ContainersToSearch)
	{
		if (!Containers.IsValidIndex(SearchContainer))
		{
			continue;
		}

		FContainerData& Container = Containers[SearchContainer];
		
		for (int32 i = Container.Items.Items.Num() - 1; i >= 0 && RemainingToRemove > 0; i--)
		{
			FGameItemDataEntry& Entry = Container.Items.Items[i];
			
			// Check if this is the item we're looking for
			if (Entry.ItemData.ItemRowHandle.RowName != RowName)
			{
				continue;
			}

			int32 RemoveFromStack = FMath::Min(RemainingToRemove, Entry.ItemData.Quantity);
			Entry.ItemData.Quantity -= RemoveFromStack;
			RemainingToRemove -= RemoveFromStack;
			bRemovedAny = true;

			// Call item event
			if (Entry.ItemData.RuntimeItem)
			{
				Entry.ItemData.RuntimeItem->OnRemoved(GetCharacterOwner(), RemoveFromStack);
			}

			// Remove stack if empty
			if (Entry.ItemData.Quantity <= 0)
			{
				FGameItemData RemovedData = Entry.ItemData;
				DestroyRuntimeItem(Entry.ItemData);
				Container.Items.Items.RemoveAt(i);
				Container.Items.MarkArrayDirty();

				OnItemRemoved.Broadcast(RemovedData, SearchContainer, true);
			}
			else
			{
				Container.Items.MarkItemDirty(Entry);
			}
		}

		if (RemainingToRemove <= 0)
		{
			break;
		}
	}

	return bRemovedAny;
}

bool UInventory::MoveItem(int32 FromContainer, FIntPoint FromPosition, int32 ToContainer, FIntPoint ToPosition, bool bRotate)
{
	if (!GetOwner()->HasAuthority())
	{
		Server_MoveItem(FromContainer, FromPosition, ToContainer, ToPosition, bRotate);
		return true; // Optimistic
	}

	// Validate containers
	if (!Containers.IsValidIndex(FromContainer) || !Containers.IsValidIndex(ToContainer))
	{
		return false;
	}

	// Find source item
	FGameItemData* SourceItem = GetItemAt(FromContainer, FromPosition);
	if (!SourceItem)
	{
		return false;
	}

	// Check if target position is valid
	if (!CanItemFitAt(ToContainer, ToPosition, SourceItem->ObjectSize2D, bRotate, SourceItem))
	{
		return false;
	}

	// Update item data
	SourceItem->ContainerIndex = ToContainer;
	SourceItem->GridPosition = FInventoryGridPosition(ToPosition, bRotate);
	SourceItem->bIsRotated = bRotate;

	// If moving between containers, need to update arrays
	if (FromContainer != ToContainer)
	{
		FContainerData& FromCont = Containers[FromContainer];
		FContainerData& ToCont = Containers[ToContainer];

		// Find and remove from source
		for (int32 i = 0; i < FromCont.Items.Items.Num(); i++)
		{
			if (FromCont.Items.Items[i].ItemData.GridPosition.Position == FromPosition)
			{
				FGameItemDataEntry MovedEntry = FromCont.Items.Items[i];
				MovedEntry.ItemData = *SourceItem;
				
				FromCont.Items.Items.RemoveAt(i);
				FromCont.Items.MarkArrayDirty();

				ToCont.Items.Items.Add(MovedEntry);
				ToCont.Items.MarkItemDirty(MovedEntry);
				break;
			}
		}
	}
	else
	{
		// Same container, just mark dirty
		Containers[FromContainer].Items.MarkArrayDirty();
	}

	OnItemMoved.Broadcast(*SourceItem, FromContainer, ToContainer, true);
	return true;
}

bool UInventory::SplitItem(int32 ContainerIndex, FIntPoint Position, int32 SplitQuantity, int32 ToContainer, FIntPoint ToPosition)
{
	if (!GetOwner()->HasAuthority())
	{
		Server_SplitItem(ContainerIndex, Position, SplitQuantity, ToContainer, ToPosition);
		return true; // Optimistic
	}

	// Validate
	if (!Containers.IsValidIndex(ContainerIndex) || !Containers.IsValidIndex(ToContainer))
	{
		return false;
	}

	FGameItemData* SourceItem = GetItemAt(ContainerIndex, Position);
	if (!SourceItem || SourceItem->Quantity <= SplitQuantity)
	{
		return false; // Can't split entire stack or invalid quantity
	}

	FItemRowDetails* RowDetails = SourceItem->GetRowDetails();
	if (!RowDetails)
	{
		return false;
	}

	// Check if target position is available
	if (!CanItemFitAt(ToContainer, ToPosition, SourceItem->ObjectSize2D, false, nullptr))
	{
		return false;
	}

	// Reduce source quantity
	SourceItem->Quantity -= SplitQuantity;
	Containers[ContainerIndex].Items.MarkArrayDirty();

	// Create new stack at target
	return AddItemAt(SourceItem->ItemRowHandle.RowName, SplitQuantity, ToContainer, ToPosition, false);
}

bool UInventory::SwapItems(int32 Container1, FIntPoint Position1, int32 Container2, FIntPoint Position2)
{
	if (!GetOwner()->HasAuthority())
	{
		Server_SwapItems(Container1, Position1, Container2, Position2);
		return true; // Optimistic
	}

	// Validate
	if (!Containers.IsValidIndex(Container1) || !Containers.IsValidIndex(Container2))
	{
		return false;
	}

	FGameItemData* Item1 = GetItemAt(Container1, Position1);
	FGameItemData* Item2 = GetItemAt(Container2, Position2);

	if (!Item1 || !Item2)
	{
		return false;
	}

	// Check if items can fit at each other's positions
	if (!CanItemFitAt(Container2, Position2, Item1->ObjectSize2D, Item1->bIsRotated, Item1))
	{
		return false;
	}
	if (!CanItemFitAt(Container1, Position1, Item2->ObjectSize2D, Item2->bIsRotated, Item2))
	{
		return false;
	}

	// Swap positions
	FInventoryGridPosition TempPosition = Item1->GridPosition;
	int32 TempContainer = Item1->ContainerIndex;

	Item1->GridPosition = Item2->GridPosition;
	Item1->ContainerIndex = Item2->ContainerIndex;

	Item2->GridPosition = TempPosition;
	Item2->ContainerIndex = TempContainer;

	// Mark dirty
	Containers[Container1].Items.MarkArrayDirty();
	if (Container1 != Container2)
	{
		Containers[Container2].Items.MarkArrayDirty();
	}

	return true;
}

bool UInventory::FindItem(FName RowName, int32& OutContainerIndex, FGameItemData& OutItemData)
{
	for (int32 i = 0; i < Containers.Num(); i++)
	{
		for (const FGameItemDataEntry& Entry : Containers[i].Items.Items)
		{
			if (Entry.ItemData.ItemRowHandle.RowName == RowName)
			{
				OutContainerIndex = i;
				OutItemData = Entry.ItemData;
				return true;
			}
		}
	}
	return false;
}

FGameItemData* UInventory::GetItemAt(int32 ContainerIndex, FIntPoint Position)
{
	if (!Containers.IsValidIndex(ContainerIndex))
	{
		return nullptr;
	}

	for (FGameItemDataEntry& Entry : Containers[ContainerIndex].Items.Items)
	{
		// Check if position falls within item's occupied cells
		TArray<FIntPoint> OccupiedCells = GetOccupiedCells(
			Entry.ItemData.GridPosition.Position,
			Entry.ItemData.ObjectSize2D,
			Entry.ItemData.bIsRotated
		);

		if (OccupiedCells.Contains(Position))
		{
			return &Entry.ItemData;
		}
	}

	return nullptr;
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Grid Management) ───────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

bool UInventory::CanItemFitAt(int32 ContainerIndex, FIntPoint Position, FIntPoint Size, bool bRotated, const FGameItemData* IgnoreItemData) const
{
	if (!Containers.IsValidIndex(ContainerIndex))
	{
		return false;
	}

	const FContainerData& Container = Containers[ContainerIndex];
	FIntPoint ActualSize = bRotated ? FIntPoint(Size.Y, Size.X) : Size;

	// Check bounds
	if (Position.X < 0 || Position.Y < 0)
	{
		return false;
	}
	if (Position.X + ActualSize.X > Container.GridSize.X)
	{
		return false;
	}
	if (Position.Y + ActualSize.Y > Container.GridSize.Y)
	{
		return false;
	}

	// Check for overlaps with existing items
	TArray<FIntPoint> RequiredCells = GetOccupiedCells(Position, Size, bRotated);

	for (const FGameItemDataEntry& Entry : Container.Items.Items)
	{
		// Skip if this is the item we're ignoring (for moves)
		if (IgnoreItemData && &Entry.ItemData == IgnoreItemData)
		{
			continue;
		}

		TArray<FIntPoint> ExistingCells = GetOccupiedCells(
			Entry.ItemData.GridPosition.Position,
			Entry.ItemData.ObjectSize2D,
			Entry.ItemData.bIsRotated
		);

		// Check for overlap
		for (const FIntPoint& RequiredCell : RequiredCells)
		{
			if (ExistingCells.Contains(RequiredCell))
			{
				return false; // Overlap detected
			}
		}
	}

	return true;
}

bool UInventory::FindAvailablePosition(int32 ContainerIndex, FIntPoint Size, int32& OutContainer, FIntPoint& OutPosition, bool& OutRotated)
{
	// Determine containers to search
	TArray<int32> ContainersToSearch;
	if (ContainerIndex >= 0 && Containers.IsValidIndex(ContainerIndex))
	{
		ContainersToSearch.Add(ContainerIndex);
	}
	else
	{
		for (int32 i = 0; i < Containers.Num(); i++)
		{
			ContainersToSearch.Add(i);
		}
	}

	// Try each container
	for (int32 SearchContainer : ContainersToSearch)
	{
		if (!Containers.IsValidIndex(SearchContainer))
		{
			continue;
		}

		const FContainerData& Container = Containers[SearchContainer];

		// Try normal orientation first
		for (int32 Y = 0; Y < Container.GridSize.Y; Y++)
		{
			for (int32 X = 0; X < Container.GridSize.X; X++)
			{
				FIntPoint TestPos(X, Y);
				
				if (CanItemFitAt(SearchContainer, TestPos, Size, false, nullptr))
				{
					OutContainer = SearchContainer;
					OutPosition = TestPos;
					OutRotated = false;
					return true;
				}
			}
		}

		// Try rotated if item can be rotated
		if (Size.X != Size.Y)
		{
			for (int32 Y = 0; Y < Container.GridSize.Y; Y++)
			{
				for (int32 X = 0; X < Container.GridSize.X; X++)
				{
					FIntPoint TestPos(X, Y);
					
					if (CanItemFitAt(SearchContainer, TestPos, Size, true, nullptr))
					{
						OutContainer = SearchContainer;
						OutPosition = TestPos;
						OutRotated = true;
						return true;
					}
				}
			}
		}
	}

	return false; // No space found
}

TArray<FIntPoint> UInventory::GetOccupiedCells(FIntPoint Position, FIntPoint Size, bool bRotated) const
{
	TArray<FIntPoint> Cells;
	FIntPoint ActualSize = bRotated ? FIntPoint(Size.Y, Size.X) : Size;

	for (int32 Y = 0; Y < ActualSize.Y; Y++)
	{
		for (int32 X = 0; X < ActualSize.X; X++)
		{
			Cells.Add(FIntPoint(Position.X + X, Position.Y + Y));
		}
	}

	return Cells;
}

bool UInventory::IsValidPosition(int32 ContainerIndex, FIntPoint Position) const
{
	if (!Containers.IsValidIndex(ContainerIndex))
	{
		return false;
	}

	const FContainerData& Container = Containers[ContainerIndex];
	return Position.X >= 0 && Position.Y >= 0 && 
	       Position.X < Container.GridSize.X && Position.Y < Container.GridSize.Y;
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── RPC Functions ─────────────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

void UInventory::Server_AddItem_Implementation(FName RowName, int32 Quantity, int32 ContainerIndex)
{
	AddItem(RowName, Quantity, ContainerIndex);
}

bool UInventory::Server_AddItem_Validate(FName RowName, int32 Quantity, int32 ContainerIndex)
{
	return Quantity > 0 && Quantity <= 999;
}

void UInventory::Server_AddItemAt_Implementation(FName RowName, int32 Quantity, int32 ContainerIndex, FIntPoint Position, bool bRotated)
{
	AddItemAt(RowName, Quantity, ContainerIndex, Position, bRotated);
}

bool UInventory::Server_AddItemAt_Validate(FName RowName, int32 Quantity, int32 ContainerIndex, FIntPoint Position, bool bRotated)
{
	return Quantity > 0 && Quantity <= 999 && Containers.IsValidIndex(ContainerIndex);
}

void UInventory::Server_RemoveItem_Implementation(FName RowName, int32 Quantity, int32 ContainerIndex)
{
	RemoveItem(RowName, Quantity, ContainerIndex);
}

bool UInventory::Server_RemoveItem_Validate(FName RowName, int32 Quantity, int32 ContainerIndex)
{
	return Quantity > 0;
}

void UInventory::Server_MoveItem_Implementation(int32 FromContainer, FIntPoint FromPosition, int32 ToContainer, FIntPoint ToPosition, bool bRotate)
{
	MoveItem(FromContainer, FromPosition, ToContainer, ToPosition, bRotate);
}

bool UInventory::Server_MoveItem_Validate(int32 FromContainer, FIntPoint FromPosition, int32 ToContainer, FIntPoint ToPosition, bool bRotate)
{
	return Containers.IsValidIndex(FromContainer) && Containers.IsValidIndex(ToContainer);
}

void UInventory::Server_SplitItem_Implementation(int32 ContainerIndex, FIntPoint Position, int32 SplitQuantity, int32 ToContainer, FIntPoint ToPosition)
{
	SplitItem(ContainerIndex, Position, SplitQuantity, ToContainer, ToPosition);
}

bool UInventory::Server_SplitItem_Validate(int32 ContainerIndex, FIntPoint Position, int32 SplitQuantity, int32 ToContainer, FIntPoint ToPosition)
{
	return SplitQuantity > 0 && Containers.IsValidIndex(ContainerIndex) && Containers.IsValidIndex(ToContainer);
}

void UInventory::Server_SwapItems_Implementation(int32 Container1, FIntPoint Position1, int32 Container2, FIntPoint Position2)
{
	SwapItems(Container1, Position1, Container2, Position2);
}

bool UInventory::Server_SwapItems_Validate(int32 Container1, FIntPoint Position1, int32 Container2, FIntPoint Position2)
{
	return Containers.IsValidIndex(Container1) && Containers.IsValidIndex(Container2);
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Internal Functions ────────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

UGameItem* UInventory::CreateRuntimeItem(FGameItemData& ItemData)
{
	FItemRowDetails* RowDetails = ItemData.GetRowDetails();
	if (!RowDetails || !RowDetails->ItemClass)
	{
		return nullptr;
	}

	// Determine Outer (Manager if exists, otherwise this Inventory)
	UObject* Outer = this; // Default to Inventory
	// TODO: Check for InventoryManager and use it as Outer if present

	UGameItem* NewItem = NewObject<UGameItem>(Outer, RowDetails->ItemClass);
	if (NewItem)
	{
		NewItem->Initialize(this, GetCharacterOwner());
		NewItem->OnAdded(GetCharacterOwner(), ItemData.Quantity);
	}

	return NewItem;
}

void UInventory::DestroyRuntimeItem(FGameItemData& ItemData)
{
	if (ItemData.RuntimeItem)
	{
		ItemData.RuntimeItem->Cleanup(GetCharacterOwner());
		ItemData.RuntimeItem = nullptr;
	}
}

bool UInventory::TryStackItem(FName RowName, int32& Quantity, int32 ContainerIndex)
{
	// Determine containers to search
	TArray<int32> ContainersToSearch;
	if (ContainerIndex >= 0 && Containers.IsValidIndex(ContainerIndex))
	{
		ContainersToSearch.Add(ContainerIndex);
	}
	else
	{
		for (int32 i = 0; i < Containers.Num(); i++)
		{
			ContainersToSearch.Add(i);
		}
	}

	bool bStackedAny = false;

	for (int32 SearchContainer : ContainersToSearch)
	{
		if (!Containers.IsValidIndex(SearchContainer) || Quantity <= 0)
		{
			continue;
		}

		FContainerData& Container = Containers[SearchContainer];

		for (FGameItemDataEntry& Entry : Container.Items.Items)
		{
			if (Entry.ItemData.ItemRowHandle.RowName != RowName)
			{
				continue;
			}

			FItemRowDetails* RowDetails = Entry.ItemData.GetRowDetails();
			if (!RowDetails)
			{
				continue;
			}

			int32 AvailableSpace = RowDetails->MaxStackSize - Entry.ItemData.Quantity;
			if (AvailableSpace <= 0)
			{
				continue;
			}

			int32 AmountToStack = FMath::Min(Quantity, AvailableSpace);
			Entry.ItemData.Quantity += AmountToStack;
			Quantity -= AmountToStack;
			bStackedAny = true;

			Container.Items.MarkItemDirty(Entry);

			if (Quantity <= 0)
			{
				return true;
			}
		}
	}

	return bStackedAny;
}

ACharacter* UInventory::GetCharacterOwner() const
{
	return Cast<ACharacter>(GetOwner());
}
// ───────────────────────────────────────────────────────────────────────────────
// ─── Stack Items ───────────────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

bool UInventory::StackItems(int32 FromContainer, FIntPoint FromPosition, int32 ToContainer, FIntPoint ToPosition, int32 Quantity)
{
	// Client-side: send RPC to server
	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_StackItems(FromContainer, FromPosition, ToContainer, ToPosition, Quantity);
		return true;
	}

	// Server-side: perform stacking
	FGameItemData* SourceItem = GetItemAt(FromContainer, FromPosition);
	FGameItemData* TargetItem = GetItemAt(ToContainer, ToPosition);

	if (!SourceItem || !TargetItem)
	{
		return false;
	}

	// Verify items can stack (same item type)
	if (SourceItem->ItemRowHandle.RowName != TargetItem->ItemRowHandle.RowName)
	{
		return false;
	}

	// Get item details for MaxStackSize
	FItemRowDetails* RowDetails = SourceItem->GetRowDetails();
	if (!RowDetails)
	{
		return false;
	}

	// Calculate how much space is available in target
	int32 AvailableSpace = RowDetails->MaxStackSize - TargetItem->Quantity;
	if (AvailableSpace <= 0)
	{
		return false; // Target is already full
	}

	// Determine actual quantity to move
	int32 SourceQuantity = (Quantity > 0) ? Quantity : SourceItem->Quantity;
	int32 QuantityToMove = FMath::Min(SourceQuantity, AvailableSpace);

	if (QuantityToMove <= 0)
	{
		return false;
	}

	// Update quantities
	SourceItem->Quantity -= QuantityToMove;
	TargetItem->Quantity += QuantityToMove;

	// Remove source item if empty
	if (SourceItem->Quantity <= 0)
	{
		FContainerData* Container = GetContainer(FromContainer);
		if (Container)
		{
			for (int32 i = Container->Items.Num() - 1; i >= 0; --i)
			{
				if (Container->Items[i].GridPosition.Position == FromPosition)
				{
					// Destroy runtime item if exists
					DestroyRuntimeItem(Container->Items[i]);
					
					// Remove from array
					Container->Items.RemoveAt(i);
					break;
				}
			}
		}
	}

	// Mark dirty for replication
	MarkArrayDirty();

	// Broadcast event (reusing OnItemMoved for now)
	OnItemMoved.Broadcast(*SourceItem, FromPosition, ToPosition, true);

	return true;
}

void UInventory::Server_StackItems_Implementation(int32 FromContainer, FIntPoint FromPosition, int32 ToContainer, FIntPoint ToPosition, int32 Quantity)
{
	StackItems(FromContainer, FromPosition, ToContainer, ToPosition, Quantity);
}

bool UInventory::Server_StackItems_Validate(int32 FromContainer, FIntPoint FromPosition, int32 ToContainer, FIntPoint ToPosition, int32 Quantity)
{
	return Quantity >= 0 && Quantity <= 9999 && Containers.IsValidIndex(FromContainer) && Containers.IsValidIndex(ToContainer);
}