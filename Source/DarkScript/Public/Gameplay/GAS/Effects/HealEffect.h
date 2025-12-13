#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "HealEffect.generated.h"

/**
 * 
 */
UCLASS()
class DARKSCRIPT_API UHealEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	UHealEffect();
};
