#pragma once
#include "CoreMinimal.h"
#include "DarkScript.h"
#include "EnhancedInputSubsystems.h"
#include "Utils/Types/UtilsTypes.h"

namespace TablePaths
{
	static const FString Inputs = TEXT("/Script/Engine.DataTable'/DarkScript/Data/Tables/InputsTable.InputsTable'");
	static const FString Items = TEXT("");
	static const FString Modules = TEXT("");
	static const FString Armors = TEXT("");
	static const FString DefaultGameplay = TEXT("/Script/Engine.DataTable'/DarkScript/Data/Tables/ArchetypesTable.ArchetypesTable'");
}

namespace LevelPaths
{
	static const FString Menu = TEXT("/DarkScript/Levels/Menu/Menu");
	static const FString Lobby = TEXT("/DarkScript/Levels/Lobby/Lobby");
	static const FString Dungeon = TEXT("");
}

namespace CurvePaths
{
	static const FString HealthScaling = TEXT("/DarkScript/Data/Curves/AI/Curve_HealthScaling");
	static const FString DamageScaling = TEXT("/DarkScript/Data/Curves/AI/Curve_DamageScaling");
	static const FString ResourceScaling = TEXT("/DarkScript/Data/Curves/AI/Curve_ResourceScaling");
	static const FString IntelligenceScaling = TEXT("/DarkScript/Data/Curves/AI/Curve_IntelligenceScaling");
}

namespace InputAction
{
	static const FString LightAttack = TEXT("/Script/EnhancedInput.InputAction'/DarkScript/Core/Inputs/IA_LightAttack.IA_LightAttack'");
	static const FString HeavyAttack = TEXT("/Script/EnhancedInput.InputAction'/DarkScript/Core/Inputs/IA_HeavyAttack.IA_HeavyAttack'");
}

namespace ColorDesign
{
	static constexpr FLinearColor ButtonNormal = FLinearColor(0.15f, 0.15f, 0.15f, 1.0f);
	static constexpr FLinearColor ButtonHover = FLinearColor(0.22f, 0.22f, 0.22f, 1.0f);
}

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

inline const FSlateBrush* GetIcon(const FName& IconName)
{
	const FSlateBrush* Brush = FDarkScriptModule::GetDarkSlate(*FString::Printf(TEXT("DarkSlate.%s"), *IconName.ToString()));
	if (Brush && Brush != FStyleDefaults::GetNoBrush())
	{
		return Brush;
	}
	return FAppStyle::GetBrush("Icons.Help");
}

inline bool ResolveInputsRow(const FName RowName, FInputsRow& Row)
{
	const UDataTable* InputsTable = GetTable(TablePaths::Inputs);
	if (!InputsTable) return false;
	const FString Ctx(TEXT("Helper_ResolveInputsRow"));
	
	if (const FInputsRow* Direct = InputsTable->FindRow<FInputsRow>(RowName, Ctx))
	{
		Row = *Direct;
		return true;
	}
	return false;
}

inline void ApplyInputContexts(const ULocalPlayer* LocalPlayer, const TArray<UInputMappingContext*>& Contexts, const int32 PriorityBase,
	const bool bClearExisting)
{
	if (!LocalPlayer) return;
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	if (!Subsystem) return;
	
	FModifyContextOptions Options;
	Options.bForceImmediately = true;
	Options.bIgnoreAllPressedKeysUntilRelease = true;
	
	if (bClearExisting)
	{
		Subsystem->ClearAllMappings();
	}
	
	int32 Priority = PriorityBase;
	for (const UInputMappingContext* Imc : Contexts)
	{
		if (Imc)
		{
			Subsystem->AddMappingContext(Imc, Priority++, Options);
		}
	}
}