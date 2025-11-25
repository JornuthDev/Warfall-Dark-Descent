#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"   
#include "Paths.h"
#include "User Interfaces/WindowRoot.h"
#include "Tables.generated.h"

enum class ETablePath
{
	E_None,
	E_InputsTable,
	E_ItemsTable,
	E_ModulesTable,
	E_ArmorsTable,
};

class UWindowRoot;

UCLASS(Abstract)
class DARK_SCRIPT_API UTables : public UObject
{
	GENERATED_BODY()

public:

	static UDataTable* GetTable(const ETablePath& Table)
	{
		static const TMap<ETablePath, FString> TableMap =
		{
			{ETablePath::E_InputsTable, INPUTS_TABLE_PATH},
			{ETablePath::E_ItemsTable, ITEMS_TABLE_PATH},
			{ETablePath::E_ModulesTable, MODULES_TABLE_PATH},
			{ETablePath::E_ArmorsTable, ARMORS_TABLE_PATH},
		};

		const FString* PathPtr = TableMap.Find(Table);
		if (!PathPtr) return nullptr;

		return Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, **PathPtr));
	}
};

USTRUCT(BlueprintType)
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

USTRUCT(BlueprintType)
struct FRootModuleRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<class UWindowRoot> ModuleClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ZOrder;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EModuleDisplayType Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="Type == EModuleDisplayType::E_Hud", EditConditionHides), Category = "Conditions")
	bool bAutoShow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="Type == EModuleDisplayType::E_Fullscreen", EditConditionHides), Category = "Conditions")
	bool bHideShortcut;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conditions")
	bool bCloseOnEsc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conditions")
	bool bCloseOnDeath;

	FRootModuleRow() :
	 Name(NAME_None)
	,ZOrder(0)
	,Type(EModuleDisplayType::E_Hud)
	,bAutoShow(true)
	,bHideShortcut(true)
	,bCloseOnEsc(true)
	,bCloseOnDeath(true)
	{}
	
};