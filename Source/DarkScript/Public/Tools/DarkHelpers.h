#pragma once
#include "CoreMinimal.h"
#include "InputMappingContext.h"   
#include "DarkHelpers.generated.h"

namespace LaunchMode
{
	static constexpr bool IsDevelopMode = true;
}

namespace TablePaths
{
	static const FString Inputs = TEXT("/Script/Engine.DataTable'/DarkScript/Data/Tables/InputsTable.InputsTable'");
	static const FString Items = TEXT("");
	static const FString Modules = TEXT("");
	static const FString Armors = TEXT("");
}

namespace LevelPaths
{
	static const FString Menu = TEXT("");
	static const FString Lobby = TEXT("");
	static const FString Dungeon = TEXT("");
}


namespace ContentObject
{
	static UDataTable* GetTable(const FString& Path)
	{
		const FString* PathPtr = &Path;
		if (!PathPtr) return nullptr;
		return Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, **PathPtr));
	}
}

USTRUCT()
struct FInputsRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UInputMappingContext*> Inputs;

	FInputsRow() :
	 Name(NAME_None)
	,Inputs(TArray<UInputMappingContext*>()) {}
};

struct FInputsHelper
{
	explicit FInputsHelper(ULocalPlayer* InLocal)
		: LocalPlayer(InLocal)
	{}

	static bool ResolveInputsRow(FName RowName, struct FInputsRow& OutRow);
	void ApplyInputContexts(const TArray<UInputMappingContext*>& Contexts, int32 PriorityBase, bool bClearExisting) const;
	void ApplyInputs(const FName RowName) const;
	void ApplyInputsArray(const TArray<FName>& RowNames) const;
	
	
	ULocalPlayer* LocalPlayer;
};