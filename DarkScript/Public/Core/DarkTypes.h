#pragma once
#include "CoreMinimal.h"
#include "DarkTypes.generated.h"

UENUM()
enum class EBlueprintResultSwitch : uint8
{
	OnSuccess,
	OnFailure,
};


// GESTION DES PATHS

namespace TablePaths
{
	static const FString Inputs = TEXT("/Script/Engine.DataTable'/Dark_Script/Data/Tables/InputsTable.InputsTable'");
	static const FString Items = TEXT("/Script/Engine.DataTable'/Dark_Script/Data/Tables/ItemsTable.ItemsTable'");
	static const FString Modules = TEXT("/Script/Engine.DataTable'/Dark_Script/Data/Tables/ModulesTable.ModulesTable'");
	static const FString Armors = TEXT("/Script/Engine.DataTable'/Dark_Script/Data/Tables/ArmorsTable.ArmorsTable'");
}

// 

namespace FindContent
{
	static UDataTable* GetTable(const FString& Path)
	{
		const FString* PathPtr = &Path;
		if (!PathPtr) return nullptr;
		
		return Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, **PathPtr));
	}
}