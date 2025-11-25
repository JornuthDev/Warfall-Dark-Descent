#pragma once
#include "CoreMinimal.h"

namespace TablePaths
{
	static const FString Inputs = TEXT("/Script/Engine.DataTable'/DarkScript/Data/Tables/InputsTable.InputsTable'");
	static const FString Items = TEXT("");
	static const FString Modules = TEXT("");
	static const FString Armors = TEXT("");
}

namespace LevelPaths
{
	static const FString Menu = TEXT("/DarkScript/Levels/Menu/Menu");
	static const FString Lobby = TEXT("/DarkScript/Levels/Lobby/Lobby");
	static const FString Dungeon = TEXT("");
}

inline UDataTable* GetTable(const FString& Path)
{
	const FString* PathPtr = &Path;
	if (!PathPtr) return nullptr;
	return Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, **PathPtr));
}