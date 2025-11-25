// ═══════════════════════════════════════════════════════════════════════════
// INVENTORY TYPES - DARK DESCENT
// Based on Game Design Document - Tarkov-style Inventory System
// ═══════════════════════════════════════════════════════════════════════════

#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "GameplayTagContainer.h"
#include "InventoryTypes.generated.h"

// ───────────────────────────────────────────────────────────────────────────
// ─── FORWARD DECLARATIONS ──────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────

class UGameItem;
class AVisualItem;
class UInventory;

// ───────────────────────────────────────────────────────────────────────────
// ─── ENUMS ─────────────────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────

/**
 * Types of lootable items (Armors are NOT here - they're fixed & upgradable)
 */
UENUM(BlueprintType)
enum class EItemType : uint8
{
	None			UMETA(DisplayName = "None"),
	Weapon			UMETA(DisplayName = "Weapon"),
	Ring			UMETA(DisplayName = "Ring"),
	Jewelry			UMETA(DisplayName = "Jewelry"),
	Consumable		UMETA(DisplayName = "Consumable"),
	Resource		UMETA(DisplayName = "Resource"),
	Quest			UMETA(DisplayName = "Quest Item"),
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None			UMETA(DisplayName = "None"),
	Sword			UMETA(DisplayName = "Sword"),
	Axe				UMETA(DisplayName = "Axe"),
	Bow				UMETA(DisplayName = "Bow"),
	Staff			UMETA(DisplayName = "Staff"),
	Wand			UMETA(DisplayName = "Wand"),
	Dagger			UMETA(DisplayName = "Dagger"),
	Mace			UMETA(DisplayName = "Mace"),
	Crossbow		UMETA(DisplayName = "Crossbow"),
};

UENUM(BlueprintType)
enum class EItemQuality : uint8
{
	Common			UMETA(DisplayName = "Common"),
	Uncommon		UMETA(DisplayName = "Uncommon"),
	Rare			UMETA(DisplayName = "Rare"),
	Epic			UMETA(DisplayName = "Epic"),
	Legendary		UMETA(DisplayName = "Legendary"),
};

// ───────────────────────────────────────────────────────────────────────────
// ─── GRID STRUCTURES (Tarkov-style) ────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────

/**
 * Represents a position in the inventory grid with rotation state
 */
USTRUCT(BlueprintType)
struct DARK_SCRIPT_API FInventoryGridPosition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	FIntPoint Position = FIntPoint::ZeroValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	bool bIsRotated = false;

	FInventoryGridPosition() = default;
	
	FInventoryGridPosition(const FIntPoint& InPosition, bool bInRotated = false)
		: Position(InPosition), bIsRotated(bInRotated)
	{}

	bool operator==(const FInventoryGridPosition& Other) const
	{
		return Position == Other.Position && bIsRotated == Other.bIsRotated;
	}

	bool operator!=(const FInventoryGridPosition& Other) const
	{
		return !(*this == Other);
	}

	FString ToString() const
	{
		return FString::Printf(TEXT("Pos(%d,%d) Rotated:%s"), 
			Position.X, Position.Y, bIsRotated ? TEXT("Yes") : TEXT("No"));
	}
};

// ───────────────────────────────────────────────────────────────────────────
// ─── DATA TABLE ROW ────────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────

/**
 * DataTable row for lootable items ONLY
 * NOTE: Armors are NOT managed here (fixed equipment, upgraded separately)
 */
USTRUCT(BlueprintType)
struct DARK_SCRIPT_API FItemRowDetails : public FTableRowBase
{
	GENERATED_BODY()

	// ─── Basic Info ────────────────────────────────────────────────────────
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic")
	FText Name = FText::FromString(TEXT("Item"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic")
	FText ShortDesc = FText::GetEmpty();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic")
	FText LongDesc = FText::GetEmpty();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic")
	EItemType Type = EItemType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic")
	EItemQuality Quality = EItemQuality::Common;

	// ─── Classes (Optional) ────────────────────────────────────────────────
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Classes")
	TSubclassOf<UGameItem> ItemClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Classes")
	TSubclassOf<AVisualItem> VisualClass = nullptr;

	// ─── Visuals ───────────────────────────────────────────────────────────
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	TObjectPtr<UStaticMesh> StaticMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	TObjectPtr<USkeletalMesh> SkeletalMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	TObjectPtr<UTexture2D> Icon = nullptr;

	// ─── Grid Settings (Tarkov-style) ──────────────────────────────────────
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (ClampMin = "1", ClampMax = "10"))
	FIntPoint ObjectSize2D = FIntPoint(1, 1);

	// ─── Gameplay Settings ─────────────────────────────────────────────────
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	FGameplayTagContainer Flags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (ClampMin = "1"))
	int32 MaxStackSize = 1;

	// ─── Weapon Settings ───────────────────────────────────────────────────
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponType WeaponType = EWeaponType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = "0.0"))
	float WeaponDamage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = "0.0"))
	float ResourceCost = 0.0f;

	// ─── Consumable Settings ───────────────────────────────────────────────
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Consumable", meta = (ClampMin = "0.0"))
	float FoodValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Consumable", meta = (ClampMin = "0.0"))
	float WaterValue = 0.0f;

	// ─── Durability ────────────────────────────────────────────────────────
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Durability", meta = (ClampMin = "0.0"))
	float MaxDurability = 100.0f;

	// ─── Helper Functions ──────────────────────────────────────────────────
	
	/** Check if this item can be rotated (X != Y) */
	bool CanBeRotated() const
	{
		return ObjectSize2D.X != ObjectSize2D.Y;
	}

	/** Get the actual size considering rotation */
	FIntPoint GetActualSize(bool bIsRotated) const
	{
		if (bIsRotated && CanBeRotated())
		{
			return FIntPoint(ObjectSize2D.Y, ObjectSize2D.X);
		}
		return ObjectSize2D;
	}
};

// ───────────────────────────────────────────────────────────────────────────
// ─── GAME ITEM DATA (Instance) ─────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────

/**
 * Instance data for an item in a container
 */
USTRUCT(BlueprintType)
struct DARK_SCRIPT_API FGameItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FDataTableRowHandle ItemRowHandle;

	UPROPERTY(BlueprintReadOnly, Category = "Item")
	TObjectPtr<UGameItem> RuntimeItem = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Quantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float Durability = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 CurrentCharges = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bIsFavorite = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bIsLocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ContainerIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FInventoryGridPosition GridPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FIntPoint ObjectSize2D = FIntPoint(1, 1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bIsRotated = false;

	FGameItemData() = default;

	/** Get the row details from DataTable */
	FItemRowDetails* GetRowDetails() const
	{
		if (ItemRowHandle.IsNull())
		{
			return nullptr;
		}
		return ItemRowHandle.GetRow<FItemRowDetails>(TEXT("FGameItemData::GetRowDetails"));
	}

	/** Get actual size considering rotation */
	FIntPoint GetActualSize() const
	{
		if (bIsRotated)
		{
			return FIntPoint(ObjectSize2D.Y, ObjectSize2D.X);
		}
		return ObjectSize2D;
	}
};

// ───────────────────────────────────────────────────────────────────────────
// ─── FAST ARRAY SERIALIZER ─────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────

/**
 * Fast Array Item Entry for efficient replication
 */
USTRUCT()
struct DARK_SCRIPT_API FGameItemDataEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	FGameItemData ItemData;

	void PreReplicatedRemove(const struct FGameItemDataArray& InArraySerializer);
	void PostReplicatedAdd(const struct FGameItemDataArray& InArraySerializer);
	void PostReplicatedChange(const struct FGameItemDataArray& InArraySerializer);
};

/**
 * Fast Array Serializer for items in container
 */
USTRUCT()
struct DARK_SCRIPT_API FGameItemDataArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FGameItemDataEntry> Items;

	UPROPERTY()
	TObjectPtr<UInventory> OwningInventory = nullptr;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FGameItemDataEntry, FGameItemDataArray>(Items, DeltaParms, *this);
	}
};

template<>
struct TStructOpsTypeTraits<FGameItemDataArray> : public TStructOpsTypeTraitsBase2<FGameItemDataArray>
{
	enum { WithNetDeltaSerializer = true };
};

// ───────────────────────────────────────────────────────────────────────────
// ─── CONTAINER DATA ────────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────

/**
 * Container instance in inventory (Tarkov-style grid)
 * Base inventory = 40 slots (8×5), Storage = 5 containers of 40 slots each
 */
USTRUCT(BlueprintType)
struct DARK_SCRIPT_API FContainerData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container")
	FName ContainerId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container")
	FText DisplayName = FText::GetEmpty();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container", meta = (ClampMin = "1"))
	FIntPoint GridSize = FIntPoint(8, 5); // Default 40 slots

	UPROPERTY()
	FGameItemDataArray Items;

	FContainerData() = default;

	FContainerData(FName InId, const FText& InName, FIntPoint InGridSize)
		: ContainerId(InId)
		, DisplayName(InName)
		, GridSize(InGridSize)
	{}
};

// ───────────────────────────────────────────────────────────────────────────
// ─── DELEGATES ─────────────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemAdded, const FGameItemData&, ItemData, int32, ContainerIndex, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemRemoved, const FGameItemData&, ItemData, int32, ContainerIndex, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnItemMoved, const FGameItemData&, ItemData, int32, FromContainer, int32, ToContainer, bool, bSuccess);