#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Tools/Types/ProxyTypes.h"
#include "DarkGameMode.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, NotBlueprintable)
class DARK_SCRIPT_API ADarkGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	ADarkGameMode();
	virtual void BeginPlay() override;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
};
