// ═══════════════════════════════════════════════════════════════════════════
// INVENTORY TYPES - IMPLEMENTATION
// ═══════════════════════════════════════════════════════════════════════════

#include "Tools/Types/InventoryTypes.h"
#include "Inventory/Inventory.h"
#include "Engine/DataTable.h"


// ───────────────────────────────────────────────────────────────────────────────
// ─── FGameItemDataEntry (FastArraySerializer callbacks) ────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

void FGameItemDataEntry::PreReplicatedRemove(const FGameItemDataArray& InArraySerializer)
{
	// Called before item is removed from replicated array
	// Can be used for cleanup if needed
}

void FGameItemDataEntry::PostReplicatedAdd(const FGameItemDataArray& InArraySerializer)
{
	// Called after item is added to replicated array
	// Can be used to initialize client-side state
}

void FGameItemDataEntry::PostReplicatedChange(const FGameItemDataArray& InArraySerializer)
{
	// Called after item is modified in replicated array
	// Can be used to update client-side state
}
