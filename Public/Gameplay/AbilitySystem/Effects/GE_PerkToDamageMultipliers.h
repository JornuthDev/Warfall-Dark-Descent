// Copyright Dark Script - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_PerkToDamageMultipliers.generated.h"

/**
 * GameplayEffect permanent qui calcule les Damage Multipliers depuis les Perks
 *
 * Modifie :
 * - PhysicDamageMultiplier (via MMC_PhysicDamageMultiplier)
 * - MagicDamageMultiplier (via MMC_MagicDamageMultiplier)
 * - AccuracyDamageMultiplier (via MMC_AccuracyDamageMultiplier)
 */
UCLASS()
class DARKSCRIPT_API UGE_PerkToDamageMultipliers : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UGE_PerkToDamageMultipliers();
};
