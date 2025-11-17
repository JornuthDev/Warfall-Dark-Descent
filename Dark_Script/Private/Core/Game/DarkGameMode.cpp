#include "Core/Game/DarkGameMode.h"

#include "Core/Game/DarkGameState.h"
#include "Core/DarkPlayerController.h"
#include "Core/DarkPlayerState.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
ADarkGameMode::ADarkGameMode()
{
	PlayerControllerClass = ADarkPlayerController::StaticClass();
	GameStateClass = ADarkGameState::StaticClass();
	PlayerStateClass = ADarkPlayerState::StaticClass();
	bUseSeamlessTravel = true;
}

void ADarkGameMode::BeginPlay()
{
	Super::BeginPlay();
}


