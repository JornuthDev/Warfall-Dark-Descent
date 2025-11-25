// ═══════════════════════════════════════════════════════════════════════════
// INVENTORY MANAGER - IMPLEMENTATION
// ═══════════════════════════════════════════════════════════════════════════

#include "Inventory/InventoryManager.h"

#include "GameFramework/PlayerState.h"
#include "Inventory/Inventory.h"
#include "Net/UnrealNetwork.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

UInventoryManager::UInventoryManager()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	Backpack.Add(FContainerData(FName("Backpack"), FText::FromString(TEXT("Backpack")), FIntPoint(8, 5)));
	
	for (int32 i = 0; i < 5; i++)
	{
		Storage.Add(FContainerData(FName(FString::Printf(TEXT("Tab%d"), i)), FText::FromString(FString::Printf(TEXT("Tab%d"), i)), FIntPoint(8, 5)));
	}
}

void UInventoryManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetOwnerRole() == ROLE_Authority)
	{
		APlayerState* PlayerState = Cast<APlayerController>(GetOwner())->PlayerState;
		UInventory* InventoryBackpack = Cast<UInventory>(PlayerState->GetComponentByClass(UInventory::StaticClass()));
		UInventory* InventoryStorage = Cast<UInventory>(GetOwner()->GetComponentByClass(UInventory::StaticClass()));
		
		ManagedInventories.Add(InventoryBackpack);
		ManagedInventories.Add(InventoryStorage);
		InventoryBackpack->Containers.Empty();
		for (auto& Container : Backpack)
		{
			InventoryBackpack->AddContainer(Container.ContainerId, Container.DisplayName, Container.GridSize);
		}
		InventoryStorage->Containers.Empty();
		for (auto& Container : Storage)
		{
			InventoryStorage->AddContainer(Container.ContainerId, Container.DisplayName, Container.GridSize);
		}
		OnInitialized.Broadcast();
		bInitialized = true;
	}
}

void UInventoryManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UInventoryManager, ManagedInventories);
	DOREPLIFETIME(UInventoryManager, bInitialized);
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

void UInventoryManager::AddItemToInventory(UInventory* TargetInventory, FName RowName, int32 Quantity, int32 ContainerIndex)
{
	if (!TargetInventory)
	{
		return;
	}

	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_AddItemToInventory(TargetInventory, RowName, Quantity, ContainerIndex);
	}
	else
	{
		TargetInventory->AddItem(RowName, Quantity, ContainerIndex);
	}
}

void UInventoryManager::RemoveItemFromInventory(UInventory* TargetInventory, FName RowName, int32 Quantity, int32 ContainerIndex)
{
	if (!TargetInventory)
	{
		return;
	}

	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_RemoveItemFromInventory(TargetInventory, RowName, Quantity, ContainerIndex);
	}
	else
	{
		TargetInventory->RemoveItem(RowName, Quantity, ContainerIndex);
	}
}

void UInventoryManager::MoveItemBetweenInventories(
	UInventory* FromInventory, int32 FromContainer, FIntPoint FromPosition,
	UInventory* ToInventory, int32 ToContainer, FIntPoint ToPosition, bool bRotate)
{
	if (!FromInventory || !ToInventory)
	{
		return;
	}

	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_MoveItemBetweenInventories(FromInventory, FromContainer, FromPosition, ToInventory, ToContainer, ToPosition, bRotate);
	}
	else
	{
		// Same inventory: use direct move
		if (FromInventory == ToInventory)
		{
			FromInventory->MoveItem(FromContainer, FromPosition, ToContainer, ToPosition, bRotate);
		}
		else
		{
			// Different inventories: remove from source, add to target
			FGameItemData* SourceItem = FromInventory->GetItemAt(FromContainer, FromPosition);
			if (SourceItem)
			{
				// Add to target first
				if (ToInventory->AddItemAt(SourceItem->ItemRowHandle.RowName, SourceItem->Quantity, ToContainer, ToPosition, bRotate))
				{
					// Remove from source
					FromInventory->RemoveItem(SourceItem->ItemRowHandle.RowName, SourceItem->Quantity, FromContainer);
				}
			}
		}
	}
}

void UInventoryManager::SplitItemBetweenInventories(
	UInventory* SourceInventory, int32 ContainerIndex, FIntPoint Position, int32 SplitQuantity,
	UInventory* TargetInventory, int32 ToContainer, FIntPoint ToPosition)
{
	if (!SourceInventory || !TargetInventory)
	{
		return;
	}

	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_SplitItemBetweenInventories(SourceInventory, ContainerIndex, Position, SplitQuantity, TargetInventory, ToContainer, ToPosition);
	}
	else
	{
		// Same inventory: use direct split
		if (SourceInventory == TargetInventory)
		{
			SourceInventory->SplitItem(ContainerIndex, Position, SplitQuantity, ToContainer, ToPosition);
		}
		else
		{
			// Different inventories: manual split
			FGameItemData* SourceItem = SourceInventory->GetItemAt(ContainerIndex, Position);
			if (SourceItem && SourceItem->Quantity > SplitQuantity)
			{
				// Add to target
				if (TargetInventory->AddItemAt(SourceItem->ItemRowHandle.RowName, SplitQuantity, ToContainer, ToPosition, false))
				{
					// Reduce source
					SourceInventory->RemoveItem(SourceItem->ItemRowHandle.RowName, SplitQuantity, ContainerIndex);
				}
			}
		}
	}
}

void UInventoryManager::SwapItemsBetweenInventories(
	UInventory* Inventory1, int32 Container1, FIntPoint Position1,
	UInventory* Inventory2, int32 Container2, FIntPoint Position2)
{
	if (!Inventory1 || !Inventory2)
	{
		return;
	}

	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_SwapItemsBetweenInventories(Inventory1, Container1, Position1, Inventory2, Container2, Position2);
	}
	else
	{
		// Same inventory: use direct swap
		if (Inventory1 == Inventory2)
		{
			Inventory1->SwapItems(Container1, Position1, Container2, Position2);
		}
		else
		{
			// Different inventories: complex swap
			FGameItemData* Item1 = Inventory1->GetItemAt(Container1, Position1);
			FGameItemData* Item2 = Inventory2->GetItemAt(Container2, Position2);

			if (Item1 && Item2)
			{
				// Store item 1 data
				FName Item1RowName = Item1->ItemRowHandle.RowName;
				int32 Item1Quantity = Item1->Quantity;
				bool Item1Rotated = Item1->bIsRotated;

				// Store item 2 data
				FName Item2RowName = Item2->ItemRowHandle.RowName;
				int32 Item2Quantity = Item2->Quantity;
				bool Item2Rotated = Item2->bIsRotated;

				// Remove both
				Inventory1->RemoveItem(Item1RowName, Item1Quantity, Container1);
				Inventory2->RemoveItem(Item2RowName, Item2Quantity, Container2);

				// Add swapped
				Inventory1->AddItemAt(Item2RowName, Item2Quantity, Container1, Position1, Item2Rotated);
				Inventory2->AddItemAt(Item1RowName, Item1Quantity, Container2, Position2, Item1Rotated);
			}
		}
	}
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── RPC Functions ─────────────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

void UInventoryManager::Server_AddItemToInventory_Implementation(UInventory* TargetInventory, FName RowName, int32 Quantity, int32 ContainerIndex)
{
	AddItemToInventory(TargetInventory, RowName, Quantity, ContainerIndex);
}

bool UInventoryManager::Server_AddItemToInventory_Validate(UInventory* TargetInventory, FName RowName, int32 Quantity, int32 ContainerIndex)
{
	return TargetInventory != nullptr && Quantity > 0 && Quantity <= 999;
}

void UInventoryManager::Server_RemoveItemFromInventory_Implementation(UInventory* TargetInventory, FName RowName, int32 Quantity, int32 ContainerIndex)
{
	RemoveItemFromInventory(TargetInventory, RowName, Quantity, ContainerIndex);
}

bool UInventoryManager::Server_RemoveItemFromInventory_Validate(UInventory* TargetInventory, FName RowName, int32 Quantity, int32 ContainerIndex)
{
	return TargetInventory != nullptr && Quantity > 0;
}

void UInventoryManager::Server_MoveItemBetweenInventories_Implementation(
	UInventory* FromInventory, int32 FromContainer, FIntPoint FromPosition,
	UInventory* ToInventory, int32 ToContainer, FIntPoint ToPosition, bool bRotate)
{
	MoveItemBetweenInventories(FromInventory, FromContainer, FromPosition, ToInventory, ToContainer, ToPosition, bRotate);
}

bool UInventoryManager::Server_MoveItemBetweenInventories_Validate(
	UInventory* FromInventory, int32 FromContainer, FIntPoint FromPosition,
	UInventory* ToInventory, int32 ToContainer, FIntPoint ToPosition, bool bRotate)
{
	return FromInventory != nullptr && ToInventory != nullptr;
}

void UInventoryManager::Server_SplitItemBetweenInventories_Implementation(
	UInventory* SourceInventory, int32 ContainerIndex, FIntPoint Position, int32 SplitQuantity,
	UInventory* TargetInventory, int32 ToContainer, FIntPoint ToPosition)
{
	SplitItemBetweenInventories(SourceInventory, ContainerIndex, Position, SplitQuantity, TargetInventory, ToContainer, ToPosition);
}

bool UInventoryManager::Server_SplitItemBetweenInventories_Validate(
	UInventory* SourceInventory, int32 ContainerIndex, FIntPoint Position, int32 SplitQuantity,
	UInventory* TargetInventory, int32 ToContainer, FIntPoint ToPosition)
{
	return SourceInventory != nullptr && TargetInventory != nullptr && SplitQuantity > 0;
}

void UInventoryManager::Server_SwapItemsBetweenInventories_Implementation(
	UInventory* Inventory1, int32 Container1, FIntPoint Position1,
	UInventory* Inventory2, int32 Container2, FIntPoint Position2)
{
	SwapItemsBetweenInventories(Inventory1, Container1, Position1, Inventory2, Container2, Position2);
}

bool UInventoryManager::Server_SwapItemsBetweenInventories_Validate(
	UInventory* Inventory1, int32 Container1, FIntPoint Position1,
	UInventory* Inventory2, int32 Container2, FIntPoint Position2)
{
	return Inventory1 != nullptr && Inventory2 != nullptr;
}