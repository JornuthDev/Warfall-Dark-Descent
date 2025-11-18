#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "ProxySubsystem.generated.h"

USTRUCT(BlueprintType)
struct FSessionPropertyKeyPair
{
	GENERATED_BODY()
	
	FName Key;
	FVariantData Data;
};

/**
 * 
 */
UCLASS()
class DARKSCRIPT_API UProxySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
};
