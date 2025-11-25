// ═══════════════════════════════════════════════════════════════════════════
// WEAPON BASE - IMPLEMENTATION
// ═══════════════════════════════════════════════════════════════════════════

#include "Inventory/Actors/WeaponBase.h"
#include "Inventory/Items/GameItem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Tools/Types/InventoryTypes.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

AWeaponBase::AWeaponBase()
{
	// Create skeletal mesh component for animated weapons
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(MeshComponent);
	SkeletalMeshComponent->SetIsReplicated(true);
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

float AWeaponBase::GetWeaponDamage() const
{
	if (!OwningGameItem)
	{
		return 0.0f;
	}

	FItemRowDetails* RowDetails = OwningGameItem->GetRowDetails();
	if (!RowDetails)
	{
		return 0.0f;
	}

	return RowDetails->WeaponDamage;
}

float AWeaponBase::GetResourceCost() const
{
	if (!OwningGameItem)
	{
		return 0.0f;
	}

	FItemRowDetails* RowDetails = OwningGameItem->GetRowDetails();
	if (!RowDetails)
	{
		return 0.0f;
	}

	return RowDetails->ResourceCost;
}