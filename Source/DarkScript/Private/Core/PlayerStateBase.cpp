#include "Core/PlayerStateBase.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
APlayerStateBase::APlayerStateBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// PlayerState réplique par défaut
	bReplicates = true;
	SetNetUpdateFrequency(100.f);
}
