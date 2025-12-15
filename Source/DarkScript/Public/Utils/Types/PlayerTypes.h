#pragma once
#include "CoreMinimal.h"
#include "PlayerTypes.generated.h"

UENUM(BlueprintType)
enum class EClassArchetype : uint8
{
	None	UMETA(DisplayName="None"),
	Tank	UMETA(DisplayName="Tank"),
	Healer	UMETA(DisplayName="Healer"),
	Ranger	UMETA(DisplayName="Ranger"),
	Wizard	UMETA(DisplayName="Wizard"),
};