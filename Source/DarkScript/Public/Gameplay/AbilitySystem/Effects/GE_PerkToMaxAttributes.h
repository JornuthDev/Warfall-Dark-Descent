// Copyright Dark Script - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_PerkToMaxAttributes.generated.h"

/**
 * GameplayEffect permanent qui calcule les Max Attributes depuis les Perks
 *
 * Modifie :
 * - MaxHealth (via MMC_MaxHealth)
 * - MaxMana (via MMC_MaxMana)
 * - MaxStamina (via MMC_MaxStamina)
 * - MaxFocus (via MMC_MaxFocus)
 */
UCLASS(NotBlueprintType, NotBlueprintable)
class DARKSCRIPT_API UGE_PerkToMaxAttributes : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UGE_PerkToMaxAttributes();
};
