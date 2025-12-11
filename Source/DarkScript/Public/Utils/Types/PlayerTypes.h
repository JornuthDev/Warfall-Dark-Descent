#pragma once
#include "CoreMinimal.h"
#include "PlayerTypes.generated.h"

UENUM(BlueprintType)
enum class EClassArchetype : uint8
{
	None		UMETA(DisplayName = "None"),
	Tank		UMETA(DisplayName = "Tank"),
	Healer		UMETA(DisplayName = "Healer"),
	Ranger 		UMETA(DisplayName = "Ranger"),
	Wizard 		UMETA(DisplayName = "Wizard")
};

UENUM(BlueprintType)
enum class EResourceType : uint8
{
	None UMETA(DisplayName = "None"),
	Mana UMETA(DisplayName = "Mana"),
	Stamina UMETA(DisplayName = "Stamina"),
	Rage UMETA(DisplayName = "Rage"),
	Energy UMETA(DisplayName = "Energy"),
	Focus UMETA(DisplayName = "Focus"),
};

UENUM(BlueprintType)
enum class EDamageType : uint8
{
	None		UMETA(DisplayName = "None"),
	Flesh		UMETA(DisplayName = "Flesh"),
	Penetration UMETA(DisplayName = "Penetration"),
	Ice			UMETA(DisplayName = "Ice"),
	Fire		UMETA(DisplayName = "Fire"),
	Lightning	UMETA(DisplayName = "Lightning"),
	Holy		UMETA(DisplayName = "Holy"),
};

