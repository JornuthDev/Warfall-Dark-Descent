#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Save/SaveableInterface.h"
#include "DarkCharacter.generated.h"

UCLASS()
class DARK_SCRIPT_API ADarkCharacter : public ACharacter, public ISaveableInterface
{
	GENERATED_BODY()
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	ADarkCharacter();
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
};
