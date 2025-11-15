#include "Core/DarkGameMode.h"

#include "Core/DarkGameState.h"
#include "Core/DarkPlayerController.h"
#include "Core/DarkPlayerState.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
ADarkGameMode::ADarkGameMode()
{
	PlayerControllerClass = ADarkPlayerController::StaticClass();
	GameStateClass = ADarkGameState::StaticClass();
	PlayerStateClass = ADarkPlayerState::StaticClass();
}

void ADarkGameMode::BeginPlay()
{
	Super::BeginPlay();
}
