// ═══════════════════════════════════════════════════════════════════════════
// INVENTORY HELPERS - IMPLEMENTATION
// ═══════════════════════════════════════════════════════════════════════════

#include "Tools/Helpers/InventoryHelpers.h"
#include "Inventory/Inventory.h"
#include "Inventory/InventoryManager.h"
#include "Engine/DataTable.h"
#include "DrawDebugHelpers.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Item Validation ───────────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

bool UInventoryHelpers::IsValidItemRow(UDataTable* DataTable, FName RowName)
{
	if (!DataTable)
	{
		return false;
	}

	return DataTable->FindRow<FItemRowDetails>(RowName, TEXT("IsValidItemRow")) != nullptr;
}

bool UInventoryHelpers::GetItemRowDetails(UDataTable* DataTable, FName RowName, FItemRowDetails& OutRowDetails)
{
	if (!DataTable)
	{
		return false;
	}

	FItemRowDetails* RowDetails = DataTable->FindRow<FItemRowDetails>(RowName, TEXT("GetItemRowDetails"));
	if (!RowDetails)
	{
		return false;
	}

	OutRowDetails = *RowDetails;
	return true;
}

bool UInventoryHelpers::IsItemStackable(const FItemRowDetails& RowDetails)
{
	return RowDetails.MaxStackSize > 1;
}

bool UInventoryHelpers::CanItemRotate(const FItemRowDetails& RowDetails)
{
	return RowDetails.CanBeRotated();
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Grid Helpers ──────────────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

int32 UInventoryHelpers::GetTotalSlotsInGrid(FIntPoint GridSize)
{
	return GridSize.X * GridSize.Y;
}

int32 UInventoryHelpers::GetUsedSlotsInContainer(UInventory* Inventory, int32 ContainerIndex)
{
	if (!Inventory)
	{
		return 0;
	}

	FContainerData* Container = Inventory->GetContainer(ContainerIndex);
	if (!Container)
	{
		return 0;
	}

	int32 UsedSlots = 0;
	for (const FGameItemDataEntry& Entry : Container->Items.Items)
	{
		FIntPoint ActualSize = Entry.ItemData.GetActualSize();
		UsedSlots += ActualSize.X * ActualSize.Y;
	}

	return UsedSlots;
}

int32 UInventoryHelpers::GetFreeSlotsInContainer(UInventory* Inventory, int32 ContainerIndex)
{
	if (!Inventory)
	{
		return 0;
	}

	FContainerData* Container = Inventory->GetContainer(ContainerIndex);
	if (!Container)
	{
		return 0;
	}

	int32 TotalSlots = GetTotalSlotsInGrid(Container->GridSize);
	int32 UsedSlots = GetUsedSlotsInContainer(Inventory, ContainerIndex);
	return TotalSlots - UsedSlots;
}

float UInventoryHelpers::GetContainerFillPercentage(UInventory* Inventory, int32 ContainerIndex)
{
	if (!Inventory)
	{
		return 0.0f;
	}

	FContainerData* Container = Inventory->GetContainer(ContainerIndex);
	if (!Container)
	{
		return 0.0f;
	}

	int32 TotalSlots = GetTotalSlotsInGrid(Container->GridSize);
	if (TotalSlots <= 0)
	{
		return 0.0f;
	}

	int32 UsedSlots = GetUsedSlotsInContainer(Inventory, ContainerIndex);
	return static_cast<float>(UsedSlots) / static_cast<float>(TotalSlots);
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Item Queries ──────────────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

int32 UInventoryHelpers::CountItemInInventory(UInventory* Inventory, FName RowName, int32 ContainerIndex)
{
	if (!Inventory)
	{
		return 0;
	}

	int32 TotalCount = 0;

	// Determine containers to search
	TArray<int32> ContainersToSearch;
	if (ContainerIndex >= 0)
	{
		ContainersToSearch.Add(ContainerIndex);
	}
	else
	{
		for (int32 i = 0; i < Inventory->GetContainers().Num(); i++)
		{
			ContainersToSearch.Add(i);
		}
	}

	// Count in each container
	for (int32 SearchContainer : ContainersToSearch)
	{
		FContainerData* Container = Inventory->GetContainer(SearchContainer);
		if (!Container)
		{
			continue;
		}

		for (const FGameItemDataEntry& Entry : Container->Items.Items)
		{
			if (Entry.ItemData.ItemRowHandle.RowName == RowName)
			{
				TotalCount += Entry.ItemData.Quantity;
			}
		}
	}

	return TotalCount;
}

bool UInventoryHelpers::HasItem(UInventory* Inventory, FName RowName, int32 RequiredQuantity, int32 ContainerIndex)
{
	return CountItemInInventory(Inventory, RowName, ContainerIndex) >= RequiredQuantity;
}

int32 UInventoryHelpers::GetItemsByType(UInventory* Inventory, EItemType ItemType, TArray<FGameItemData>& OutItems)
{
	OutItems.Empty();

	if (!Inventory)
	{
		return 0;
	}

	for (FContainerData& Container : Inventory->GetContainers())
	{
		for (FGameItemDataEntry& Entry : Container.Items.Items)
		{
			FItemRowDetails* RowDetails = Entry.ItemData.GetRowDetails();
			if (RowDetails && RowDetails->Type == ItemType)
			{
				OutItems.Add(Entry.ItemData);
			}
		}
	}

	return OutItems.Num();
}

int32 UInventoryHelpers::GetItemsByQuality(UInventory* Inventory, EItemQuality Quality, TArray<FGameItemData>& OutItems)
{
	OutItems.Empty();

	if (!Inventory)
	{
		return 0;
	}

	for (FContainerData& Container : Inventory->GetContainers())
	{
		for (FGameItemDataEntry& Entry : Container.Items.Items)
		{
			FItemRowDetails* RowDetails = Entry.ItemData.GetRowDetails();
			if (RowDetails && RowDetails->Quality == Quality)
			{
				OutItems.Add(Entry.ItemData);
			}
		}
	}

	return OutItems.Num();
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Inventory Management ──────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

UInventory* UInventoryHelpers::GetInventoryFromActor(AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}

	return Actor->FindComponentByClass<UInventory>();
}

UInventoryManager* UInventoryHelpers::GetInventoryManagerFromActor(AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}

	return Actor->FindComponentByClass<UInventoryManager>();
}

bool UInventoryHelpers::CanInventoriesTrade(UInventory* FromInventory, UInventory* ToInventory)
{
	// Basic validation
	if (!FromInventory || !ToInventory)
	{
		return false;
	}

	// Can't trade with self
	if (FromInventory == ToInventory)
	{
		return false;
	}

	// Both must have valid owners
	if (!FromInventory->GetOwner() || !ToInventory->GetOwner())
	{
		return false;
	}

	// Additional checks can be added here (distance, permissions, etc.)

	return true;
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Conversion Helpers ────────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

FString UInventoryHelpers::ItemTypeToString(EItemType ItemType)
{
	switch (ItemType)
	{
	case EItemType::None: return TEXT("None");
	case EItemType::Weapon: return TEXT("Weapon");
	case EItemType::Ring: return TEXT("Ring");
	case EItemType::Jewelry: return TEXT("Jewelry");
	case EItemType::Consumable: return TEXT("Consumable");
	case EItemType::Resource: return TEXT("Resource");
	case EItemType::Quest: return TEXT("Quest");
	default: return TEXT("Unknown");
	}
}

FString UInventoryHelpers::WeaponTypeToString(EWeaponType WeaponType)
{
	switch (WeaponType)
	{
	case EWeaponType::None: return TEXT("None");
	case EWeaponType::Sword: return TEXT("Sword");
	case EWeaponType::Axe: return TEXT("Axe");
	case EWeaponType::Bow: return TEXT("Bow");
	case EWeaponType::Staff: return TEXT("Staff");
	case EWeaponType::Wand: return TEXT("Wand");
	case EWeaponType::Dagger: return TEXT("Dagger");
	case EWeaponType::Mace: return TEXT("Mace");
	case EWeaponType::Crossbow: return TEXT("Crossbow");
	default: return TEXT("Unknown");
	}
}

FString UInventoryHelpers::QualityToString(EItemQuality Quality)
{
	switch (Quality)
	{
	case EItemQuality::Common: return TEXT("Common");
	case EItemQuality::Uncommon: return TEXT("Uncommon");
	case EItemQuality::Rare: return TEXT("Rare");
	case EItemQuality::Epic: return TEXT("Epic");
	case EItemQuality::Legendary: return TEXT("Legendary");
	default: return TEXT("Unknown");
	}
}

FLinearColor UInventoryHelpers::GetQualityColor(EItemQuality Quality)
{
	switch (Quality)
	{
	case EItemQuality::Common: return FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);
	case EItemQuality::Uncommon: return FLinearColor(0.2f, 0.8f, 0.2f, 1.0f);
	case EItemQuality::Rare: return FLinearColor(0.2f, 0.5f, 1.0f, 1.0f);
	case EItemQuality::Epic: return FLinearColor(0.6f, 0.2f, 0.8f, 1.0f);
	case EItemQuality::Legendary: return FLinearColor(1.0f, 0.6f, 0.1f, 1.0f);
	default: return FLinearColor::White;
	}
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Debug Helpers ─────────────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

void UInventoryHelpers::DebugPrintInventory(UInventory* Inventory, int32 ContainerIndex)
{
	if (!Inventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("DebugPrintInventory: Inventory is null"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("========================================"));
	UE_LOG(LogTemp, Log, TEXT("INVENTORY DEBUG: %s"), *Inventory->GetOwner()->GetName());
	UE_LOG(LogTemp, Log, TEXT("========================================"));

	// Determine containers to print
	TArray<int32> ContainersToPrint;
	if (ContainerIndex >= 0)
	{
		ContainersToPrint.Add(ContainerIndex);
	}
	else
	{
		for (int32 i = 0; i < Inventory->GetContainers().Num(); i++)
		{
			ContainersToPrint.Add(i);
		}
	}

	// Print each container
	for (int32 Index : ContainersToPrint)
	{
		FContainerData* Container = Inventory->GetContainer(Index);
		if (!Container)
		{
			continue;
		}

		UE_LOG(LogTemp, Log, TEXT("Container %d: %s (Size: %dx%d)"),
			Index,
			*Container->DisplayName.ToString(),
			Container->GridSize.X,
			Container->GridSize.Y
		);

		int32 ItemCount = 0;
		for (const FGameItemDataEntry& Entry : Container->Items.Items)
		{
			FItemRowDetails* RowDetails = Entry.ItemData.GetRowDetails();
			if (!RowDetails)
			{
				continue;
			}

			UE_LOG(LogTemp, Log, TEXT("  [%d] %s x%d at (%d,%d) Size:%dx%d %s"),
				ItemCount++,
				*RowDetails->Name.ToString(),
				Entry.ItemData.Quantity,
				Entry.ItemData.GridPosition.Position.X,
				Entry.ItemData.GridPosition.Position.Y,
				Entry.ItemData.ObjectSize2D.X,
				Entry.ItemData.ObjectSize2D.Y,
				Entry.ItemData.bIsRotated ? TEXT("(Rotated)") : TEXT("")
			);
		}

		UE_LOG(LogTemp, Log, TEXT("  Total Items: %d"), ItemCount);
		UE_LOG(LogTemp, Log, TEXT("  Used Slots: %d / %d (%.1f%%)"),
			GetUsedSlotsInContainer(Inventory, Index),
			GetTotalSlotsInGrid(Container->GridSize),
			GetContainerFillPercentage(Inventory, Index) * 100.0f
		);
		UE_LOG(LogTemp, Log, TEXT("----------------------------------------"));
	}

	UE_LOG(LogTemp, Log, TEXT("========================================"));
}

void UInventoryHelpers::DebugDrawInventoryGrid(UObject* World, UInventory* Inventory, int32 ContainerIndex, FVector Origin, float Duration)
{
	if (!World || !Inventory)
	{
		return;
	}

	FContainerData* Container = Inventory->GetContainer(ContainerIndex);
	if (!Container)
	{
		return;
	}

	const float CellSize = 50.0f;
	const FVector CellOffset(0, 0, 10);

	// Draw grid
	for (int32 Y = 0; Y <= Container->GridSize.Y; Y++)
	{
		FVector Start = Origin + FVector(0, Y * CellSize, 0);
		FVector End = Start + FVector(Container->GridSize.X * CellSize, 0, 0);
		DrawDebugLine(World->GetWorld(), Start, End, FColor::White, false, Duration, 0, 2.0f);
	}

	for (int32 X = 0; X <= Container->GridSize.X; X++)
	{
		FVector Start = Origin + FVector(X * CellSize, 0, 0);
		FVector End = Start + FVector(0, Container->GridSize.Y * CellSize, 0);
		DrawDebugLine(World->GetWorld(), Start, End, FColor::White, false, Duration, 0, 2.0f);
	}

	// Draw items
	for (const FGameItemDataEntry& Entry : Container->Items.Items)
	{
		FIntPoint ActualSize = Entry.ItemData.GetActualSize();
		FVector ItemOrigin = Origin + FVector(
			Entry.ItemData.GridPosition.Position.X * CellSize,
			Entry.ItemData.GridPosition.Position.Y * CellSize,
			0
		);

		FVector BoxExtent(
			ActualSize.X * CellSize * 0.5f,
			ActualSize.Y * CellSize * 0.5f,
			5.0f
		);

		FVector BoxCenter = ItemOrigin + FVector(BoxExtent.X, BoxExtent.Y, 0) + CellOffset;

		FItemRowDetails* RowDetails = Entry.ItemData.GetRowDetails();
		FColor ItemColor = FColor::Green;
		if (RowDetails)
		{
			ItemColor = GetQualityColor(RowDetails->Quality).ToFColor(true);
		}

		DrawDebugBox(World->GetWorld(), BoxCenter, BoxExtent, ItemColor, false, Duration, 0, 3.0f);

		// Draw quantity text
		if (Entry.ItemData.Quantity > 1)
		{
			DrawDebugString(
				World->GetWorld(),
				BoxCenter + FVector(0, 0, 10),
				FString::Printf(TEXT("x%d"), Entry.ItemData.Quantity),
				nullptr,
				FColor::White,
				Duration,
				true,
				1.5f
			);
		}
	}
}