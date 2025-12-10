#include "Core/PlayerStateBase.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
APlayerStateBase::APlayerStateBase()
{
	PrimaryActorTick.bCanEverTick = true;
}
