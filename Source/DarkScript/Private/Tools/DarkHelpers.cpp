#include "Tools/DarkHelpers.h"
#include "EnhancedInputSubsystems.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── FInputsHelpers ────────────────────────────────────────────────────────────
bool FInputsHelper::ResolveInputsRow(const FName RowName, struct FInputsRow& OutRow)
{
	const UDataTable* Table = ContentObject::GetTable(TablePaths::Inputs);
	if (!Table) return false;
	const FString Ctx(TEXT("ResolveInputsRow"));
	
	if (const FInputsRow* Direct = Table->FindRow<FInputsRow>(RowName, Ctx))
	{
		OutRow = *Direct;
		return true;
	}
	
	for (const FName& Row : Table->GetRowNames())
	{
		if (const FInputsRow* Direct = Table->FindRow<FInputsRow>(Row, Ctx))
		{
			if (Direct->Name == RowName)
			{
				OutRow = *Direct;
				return true;
			}
		}
	}
	return false;
}

void FInputsHelper::ApplyInputContexts(const TArray<UInputMappingContext*>& Contexts, const int32 PriorityBase,
	const bool bClearExisting) const
{
	if (!LocalPlayer) return;

	UEnhancedInputLocalPlayerSubsystem* SubSys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	if (!SubSys) return;
	
	FModifyContextOptions Opt;
	Opt.bForceImmediately = true;
	Opt.bIgnoreAllPressedKeysUntilRelease = true;
	
	if (bClearExisting)
	{
		SubSys->ClearAllMappings();
	}
	
	int32 Priority = PriorityBase;
	for (const UInputMappingContext* Imc : Contexts)
	{
		if (Imc)
		{
			SubSys->AddMappingContext(Imc, Priority++, Opt);
		}
	}
}

void FInputsHelper::ApplyInputs(const FName RowName) const
{
	FInputsRow Row;
	if (!ResolveInputsRow(RowName, Row)) return;
	ApplyInputContexts(Row.Inputs, 0, true);
}

void FInputsHelper::ApplyInputsArray(const TArray<FName>& RowNames) const
{
	TArray<UInputMappingContext*> Contexts;
	Contexts.Reserve(8);
	
	for (const FName& RowName : RowNames)
	{
		FInputsRow Row;
		if (!ResolveInputsRow(RowName, Row)) continue;
		for (UInputMappingContext* Imc : Row.Inputs)
		{
			if (Imc) Contexts.Add(Imc);
		}
	}
	ApplyInputContexts(Contexts, 0, true);
}
