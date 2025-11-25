// ═══════════════════════════════════════════════════════════════════════════
// VISUAL ITEM - IMPLEMENTATION
// ═══════════════════════════════════════════════════════════════════════════

#include "Inventory/Actors/VisualItem.h"
#include "Inventory/Items/GameItem.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

AVisualItem::AVisualItem()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bAlwaysRelevant = false;

	// Create mesh component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
	MeshComponent->SetIsReplicated(true);
}

void AVisualItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AVisualItem, OwningGameItem);
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

void AVisualItem::SetOwningGameItem(UGameItem* InGameItem)
{
	OwningGameItem = InGameItem;

	// Call Blueprint event
	if (AActor* OwnerActor = GetOwner())
	{
		if (ACharacter* Character = Cast<ACharacter>(OwnerActor))
		{
			OnVisualSpawned(Character);
		}
	}
}