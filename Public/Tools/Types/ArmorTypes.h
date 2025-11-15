#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GlobalTypes.h"
#include "Tools/Tables.h"

#include "ArmorTypes.generated.h"

/**
 * Defines the available equipment slots for armor pieces.
 */
UENUM(BlueprintType)
enum class EArmorSlot : uint8
{
	E_Helmet UMETA(DisplayName = "Helmet"),
	E_Chest UMETA(DisplayName = "Chest"),
	E_Legs UMETA(DisplayName = "Legs"),
	E_Foots UMETA(DisplayName = "Foots"),
	E_Hands UMETA(DisplayName = "Hands"),
	E_Shoulders UMETA(DisplayName = "Shoulders"),
	E_Back UMETA(DisplayName = "Back"),
};

/**
 * Defines a tier that unlocks additional stat slots for an armor piece.
 */
USTRUCT(BlueprintType)
struct FArmorUnlockTier
{
	GENERATED_BODY()
	
	/** Required level to unlock this tier. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = INDEX_NONE;
	
	/** Number of stat slots unlocked at this tier. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SlotsUnlocked = 1;
	
	/** List of stats that can appear at this tier. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FStatDefinition> AvailableStats;
};

USTRUCT(BlueprintType)
struct FArmorDefinitionRow : public FTableRowBase
{
	GENERATED_BODY()

public:

	/* ===============================[ Identification ]=============================== */

	/** Slot de l'armure (Helmet, Chest, Legs, etc.) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EArmorSlot Slot = EArmorSlot::E_Helmet;

	/** Archétype pour lequel cette armure est prévue (Tank, Mage, etc.) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EPlayerArchetype Archetype = EPlayerArchetype::E_Tank;


	/* ===============================[ Progression System ]=============================== */

	/** Niveau maximum atteignable pour cet item */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxLevel = 50;

	/** Liste des niveaux où un nouvel emplacement de statistique est débloqué */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FArmorUnlockTier> UnlockTiers;
	

	/* ===============================[ Base stats & visuals ]=============================== */

	/** Statistiques de base de la pièce (valeurs fixes indépendantes des points investis) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="Attributes;Statistics"))
	TMap<FGameplayTag, float> BaseStats;

	/** Nom affiché dans l’inventaire / UI */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

	/** Description optionnelle */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(MultiLine=true))
	FText Description;

	/** Icône visuelle dans l’UI */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* Icon = nullptr;

	/** Qualité de base de la pièce (ex: Normal, Rare, Épique...) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 DefaultQuality = 0;
};

/**
 * Represents a single armor piece instance in runtime.
 * Combines static data (from DataAsset) and dynamic state (level, distributed points, etc.).
 */
USTRUCT(BlueprintType)
struct FArmorInstance
{
	GENERATED_BODY()

	/* =========================[ STATIC REFERENCE ]========================= */

	/** Reference to the armor DataAsset (design-time static data) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle ArmorData;


	/* =========================[ DYNAMIC DATA ]========================= */

	/** Current level of the armor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 0;

	/** Current quality of the armor (0=Normal, 1=Rare, 2=Epic, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quality = 0;
	
	/** Map of distributed points per stat (GameplayTag = allocated points) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Attributes;Statistics"))
	TMap<FGameplayTag, int32> DistributedPoints;


	/* =========================[ RUNTIME LOGIC ]========================= */

	/** Returns true if the DataAsset is valid */
	bool IsValid() const { return !ArmorData.IsNull(); }

	/** Returns the base stats from the DataAsset */
	const TMap<FGameplayTag, float>& GetBaseStats() const
	{
		static const TMap<FGameplayTag, float> Empty;
		static const TMap<FGameplayTag, float> Result = ArmorData.GetRow<FArmorDefinitionRow>("Found")->BaseStats;
		return (IsValid() ? Result : Empty);
	}

	/** Calculates how many stat slots are unlocked at the current level */
	int32 GetUnlockedSlots() const
	{
		if (ArmorData.IsNull()) return 0;

		int32 TotalSlots = 0;
		for (const auto& [TierLevel, SlotsUnlocked, Stats] : ArmorData.GetRow<FArmorDefinitionRow>("Found")->UnlockTiers)
		{
			if (Level >= TierLevel)
			{
				TotalSlots += SlotsUnlocked;
			}
		}
		return TotalSlots;
	}

	/** Returns how many slots are currently occupied */
	int32 GetUsedSlots() const
	{
		return DistributedPoints.Num();
	}

	/** Returns whether the player can assign more stats */
	bool CanAssignMoreStats() const
	{
		return GetUsedSlots() < GetUnlockedSlots();
	}

	/** Returns total value for a given stat (base + points) */
	float GetTotalValue(const FGameplayTag& StatTag) const
	{
		if (ArmorData.IsNull()) return 0.f;

		const float BaseValue = ArmorData.GetRow<FArmorDefinitionRow>("Found")->BaseStats.FindRef(StatTag);
		const int32 BonusPoints = DistributedPoints.FindRef(StatTag);
		return BaseValue + BonusPoints;
	}

	FArmorInstance()
	{
		ArmorData.DataTable = UTables::GetTable(ETablePath::E_ArmorsTable);
		ArmorData.RowName = NAME_None;
	}
	
	explicit FArmorInstance(const FName RowName)
	{
		ArmorData.DataTable = UTables::GetTable(ETablePath::E_ArmorsTable);
		ArmorData.RowName = RowName;
	}
};
