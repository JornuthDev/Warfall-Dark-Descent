#pragma once
#include "CoreMinimal.h"
#include "OnlineKeyValuePair.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "Utils/Helpers/SystemsHelper.h"
#include "UtilsTypes.generated.h"

UENUM()
enum class EBlueprintResultSwitch : uint8
{
	Success,
	Failure,
};

UENUM()
enum class EBlueprintResultBool : uint8
{
	OnTrue,
	OnFalse
};

USTRUCT(BlueprintType)
struct FPropertyKeyPair
{
	GENERATED_BODY()
	FName Key;
	FVariantData Data;
	FPropertyKeyPair() = default;
};

namespace ColorDesign
{
	static constexpr FLinearColor ButtonNormal = FLinearColor(0.15f, 0.15f, 0.15f, 1.0f);
	static constexpr FLinearColor ButtonHover = FLinearColor(0.22f, 0.22f, 0.22f, 1.0f);
}

USTRUCT(BlueprintType)
struct FInputsRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UInputMappingContext*> Inputs;
	
	FInputsRow() = default;
	
	static bool ResolveInputsRow(const FName RowName, FInputsRow& Row)
	{
		const UDataTable* Table = Content::GetTable(Tables::Inputs);
		if (!Table) return false;
		const FString Ctx(TEXT("Helper_ResolveInputsRow"));
		
		if (const FInputsRow* Direct = Table->FindRow<FInputsRow>(RowName, Ctx))
		{
			Row = *Direct;
			return true;
		}
		return false;
	}
	
	static bool ApplyInputsContexts(const ULocalPlayer* LocalPlayer, const TArray<FName>& RowNames,
		const int32 PriorityBase, const bool bClearExisting)
	{
		if (!LocalPlayer) return false;
		
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
		if (!Subsystem) return false;
		
		TArray<UInputMappingContext*> Contexts;
		Contexts.Reserve(RowNames.Num());
		
		FModifyContextOptions Options;
		Options.bForceImmediately = true;
		Options.bIgnoreAllPressedKeysUntilRelease = true;
		
		for (const FName& RowName : RowNames)
		{
			FInputsRow Row;
			if (!ResolveInputsRow(RowName, Row))
			{
				UE_LOG(DarkScript, Warning, TEXT("[Input] ApplyInputRows: Row '%s' not found; skipping."), *RowName.ToString());
				continue;
			}
			for (UInputMappingContext* Context : Row.Inputs)
			{
				if (Context) Contexts.Add(Context);
			}
		}
		
		if (bClearExisting)
		{
			Subsystem->ClearAllMappings();
		}
		
		int32 Priority = PriorityBase;
		for (const UInputMappingContext* Context : Contexts)
		{
			if (Context)
			{
				Subsystem->AddMappingContext(Context, Priority++, Options);
			}
		}
		return true;
	}
};