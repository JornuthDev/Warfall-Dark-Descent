#pragma once
#include "CoreMinimal.h"
#include "Core/Framework/Base_Character.h"
#include "SaveTypes.generated.h"

USTRUCT(NotBlueprintType)
struct FGlobalAbilitySave
{
	GENERATED_BODY()
	
	EClassArchetype Archetype;
	int32 Level;
	float CurrentExp;
	
	FGlobalAbilitySave()
		:Archetype(EClassArchetype::Tank)
		,Level(1)
		,CurrentExp(0)
	{}
};