// Copyright Dark Script - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_PerkToRegenRates.generated.h"

/**
 * GameplayEffect permanent qui calcule les RegenRates depuis les Perks et MaxAttributes
 *
 * Modifie :
 * - HealthRegenRate (via MMC_HealthRegenRate)
 * - ManaRegenRate (via MMC_ManaRegenRate)
 * - StaminaRegenRate (via MMC_StaminaRegenRate)
 * - FocusRegenRate (via MMC_FocusRegenRate)
 *
 * IMPORTANT: Doit être appliqué APRÈS GE_PerkToMaxAttributes car les RegenRates dépendent des MaxAttributes
 */
UCLASS(NotBlueprintType, NotBlueprintable)
class DARKSCRIPT_API UGE_PerkToRegenRates : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UGE_PerkToRegenRates();
};
