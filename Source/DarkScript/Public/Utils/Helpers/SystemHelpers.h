#pragma once
#include "CoreMinimal.h"
#include "Gameplay/AbilitySystem/AttributeTypes.h"

namespace Content
{
	inline UDataTable* GetTable(const FString& Path)
	{
		const FString* PathPtr = &Path;
		if (!PathPtr) return nullptr;
		return Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, **PathPtr));
	}
	inline UMaterialInterface* GetMaterial(const FString& Path)
	{
		const FString* PathPtr = &Path;
		if (!PathPtr) return nullptr;
		return Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, **PathPtr));
	}
}

namespace Tables
{
	static const FString Attributes = TEXT("/Script/Engine.DataTable'/DarkScript/Data/Tables/AttributesTable.AttributesTable'");
	static const FString Archetypes = TEXT("/Script/Engine.DataTable'/DarkScript/Data/Tables/ArchetypesTable.ArchetypesTable'");
}

namespace AbilitySystem
{
	inline FDefaultAttributeRow* GetDefaultAttributes(const FName& RowName, const FString& Ctx = "")
	{
		if (const UDataTable* Table = Content::GetTable(Tables::Attributes))
		{
			if (FDefaultAttributeRow* Direct = Table->FindRow<FDefaultAttributeRow>(RowName, Ctx))
			{
				return Direct;
			}
		}
		return nullptr;
	}
	inline FAdvancedAttributeRow* GetPlayerAttributes(const FName& RowName, const FString& Ctx = "")
	{
		if (const UDataTable* Table = Content::GetTable(Tables::Archetypes))
		{
			if (FAdvancedAttributeRow* Direct = Table->FindRow<FAdvancedAttributeRow>(RowName, Ctx))
			{
				return Direct;
			}
		}
		return nullptr;
	}
}
