// Copyright Dark Script - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_PerkToCriticals.generated.h"

/**
 * GameplayEffect permanent qui calcule les Critical attributes depuis les Perks
 *
 * Modifie :
 * - CriticalChance (via MMC_CriticalChance)
 * - CriticalDamage (via MMC_CriticalDamage)
 */
UCLASS(NotBlueprintType, NotBlueprintable)
class DARKSCRIPT_API UGE_PerkToCriticals : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UGE_PerkToCriticals();
};
