#pragma once
#include "CoreMinimal.h"
#include "ProfileTypes.generated.h"

UENUM(BlueprintType)
enum class EPlayerArchetype : uint8
{
	None,
	Tank,
	Healer,
	Ranger,
	Mage
};