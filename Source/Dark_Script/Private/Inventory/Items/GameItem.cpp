// ═══════════════════════════════════════════════════════════════════════════
// GAME ITEM - IMPLEMENTATION
// ═══════════════════════════════════════════════════════════════════════════

#include "Inventory/Items/GameItem.h"
#include "Inventory/Inventory.h"
#include "Inventory/Actors/VisualItem.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

UGameItem::UGameItem()
{
	bIsInitialized = false;
}

void UGameItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UGameItem, OwningInventory);
	DOREPLIFETIME(UGameItem, VisualItem);
}

int32 UGameItem::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return FunctionCallspace::Local;
	}
	check(GetOuter() != nullptr);
	return GetOuter()->GetFunctionCallspace(Function, Stack);
}

bool UGameItem::CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack)
{
	check(!HasAnyFlags(RF_ClassDefaultObject));
	
	// Try to find an actor owner for networking
	AActor* Owner = Cast<AActor>(GetOuter());
	if (Owner == nullptr)
	{
		// If outer is UInventory, get its owner
		if (UInventory* Inventory = Cast<UInventory>(GetOuter()))
		{
			Owner = Inventory->GetOwner();
		}
	}
	
	if (Owner != nullptr)
	{
		UNetDriver* NetDriver = Owner->GetNetDriver();
		if (NetDriver)
		{
			NetDriver->ProcessRemoteFunction(Owner, Function, Parms, OutParms, Stack, this);
			return true;
		}
	}
	return false;
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Blueprint Events (Authority Only) ─────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

void UGameItem::OnInitialized_Implementation(ACharacter* Character)
{
	// Override in Blueprint
}

void UGameItem::OnDestroyed_Implementation(ACharacter* Character)
{
	// Override in Blueprint
}

void UGameItem::OnAdded_Implementation(ACharacter* Character, int32 Quantity)
{
	// Override in Blueprint
}

void UGameItem::OnRemoved_Implementation(ACharacter* Character, int32 Quantity)
{
	// Override in Blueprint
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

FGameItemData* UGameItem::GetItemData() const
{
	if (!OwningInventory)
	{
		return nullptr;
	}
	
	// Find the item data that references this runtime item
	for (FContainerData& Container : OwningInventory->GetContainers())
	{
		for (FGameItemDataEntry& Entry : Container.Items.Items)
		{
			if (Entry.ItemData.RuntimeItem == this)
			{
				return &Entry.ItemData;
			}
		}
	}
	
	return nullptr;
}

FItemRowDetails* UGameItem::GetRowDetails() const
{
	FGameItemData* ItemData = GetItemData();
	if (!ItemData)
	{
		return nullptr;
	}
	
	return ItemData->GetRowDetails();
}

AVisualItem* UGameItem::SpawnVisualItem(const FTransform& SpawnTransform)
{
	if (!GetWorld() || !GetWorld()->GetAuthGameMode())
	{
		return nullptr;
	}

	FItemRowDetails* RowDetails = GetRowDetails();
	if (!RowDetails || !RowDetails->VisualClass)
	{
		return nullptr;
	}

	// Destroy existing visual if any
	if (VisualItem)
	{
		VisualItem->Destroy();
		VisualItem = nullptr;
	}

	// Spawn new visual item
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwningInventory ? OwningInventory->GetOwner() : nullptr;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	VisualItem = GetWorld()->SpawnActor<AVisualItem>(
		RowDetails->VisualClass,
		SpawnTransform,
		SpawnParams
	);

	if (VisualItem)
	{
		VisualItem->SetOwningGameItem(this);
	}

	return VisualItem;
}

void UGameItem::DestroyVisualItem()
{
	if (VisualItem)
	{
		VisualItem->Destroy();
		VisualItem = nullptr;
	}
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

void UGameItem::Initialize(UInventory* InInventory, ACharacter* Character)
{
	if (bIsInitialized)
	{
		return;
	}

	OwningInventory = InInventory;
	bIsInitialized = true;

	// Call Blueprint event (Authority only)
	if (GetWorld() && GetWorld()->GetAuthGameMode())
	{
		OnInitialized(Character);
	}
}

void UGameItem::Cleanup(ACharacter* Character)
{
	if (!bIsInitialized)
	{
		return;
	}

	// Call Blueprint event (Authority only)
	if (GetWorld() && GetWorld()->GetAuthGameMode())
	{
		OnDestroyed(Character);
	}

	// Cleanup visual item
	DestroyVisualItem();

	bIsInitialized = false;
	OwningInventory = nullptr;
}