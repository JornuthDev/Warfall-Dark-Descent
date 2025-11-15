#include "Core/DarkPlayerController.h"
#include "Components/GuiController.h"
#include "Tools/Tables.h"
#include "EnhancedInputSubsystems.h"
#include "Core/DarkGameInstance.h"
#include "Core/SubSystems/ProxyInstance.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
ADarkPlayerController::ADarkPlayerController()
{
	GuiController = CreateDefaultSubobject<UGuiController>(TEXT("GuiController"));
	check (GuiController);
}

void ADarkPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	ApplyInput("Default");
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
void ADarkPlayerController::ApplyInput(const FName RowName, const int32 PriorityBase, const bool bClearExisting) const
{
	FInputsRow Row;
	if (!ResolveInputsRow(RowName, Row))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Input] ApplyInputRow: Row '%s' not found in Inputs table."), *RowName.ToString());
		return;
	}

	ApplyInputContexts(Row.Inputs, PriorityBase, bClearExisting);
}

void ADarkPlayerController::ApplyInputs(const TArray<FName>& RowNames, const int32 PriorityBase, const bool bClearExisting) const
{
	TArray<UInputMappingContext*> Contexts;
	Contexts.Reserve(8);

	for (const FName& RowName : RowNames)
	{
		FInputsRow Row;
		if (!ResolveInputsRow(RowName, Row))
		{
			UE_LOG(LogTemp, Warning, TEXT("[Input] ApplyInputRows: Row '%s' not found; skipping."), *RowName.ToString());
			continue;
		}

		for (UInputMappingContext* Imc : Row.Inputs)
		{
			if (Imc) Contexts.Add(Imc);
		}
	}

	ApplyInputContexts(Contexts, PriorityBase, bClearExisting);
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
bool ADarkPlayerController::ResolveInputsRow(const FName RowName, FInputsRow& OutRow)
{
	const UDataTable* InputsTable = UTables::GetTable(ETablePath::E_InputsTable);
	if (!InputsTable) return false;
	const FString Ctx(TEXT("Controller_ResolveInputsRow"));
	
	if (const FInputsRow* Direct = InputsTable->FindRow<FInputsRow>(RowName, Ctx))
	{
		OutRow = *Direct;
		return true;
	}
	
	for (TArray<FName> Rows = InputsTable->GetRowNames(); const FName& Rn : Rows)
	{
		if (const FInputsRow* R = InputsTable->FindRow<FInputsRow>(Rn, Ctx))
		{
			if (R->Name == RowName)
			{
				OutRow = *R;
				return true;
			}
		}
	}
	return false;
}

void ADarkPlayerController::ApplyInputContexts(const TArray<UInputMappingContext*>& Contexts, const int32 PriorityBase,
	const bool bClearExisting) const
{
	const ULocalPlayer* Lp = GetLocalPlayer();
	if (!Lp) return;

	UEnhancedInputLocalPlayerSubsystem* Subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Lp);
	if (!Subsys) return;

	FModifyContextOptions Opt;
	Opt.bForceImmediately = true;
	Opt.bIgnoreAllPressedKeysUntilRelease = true;

	if (bClearExisting)
	{
		Subsys->ClearAllMappings();
	}

	int32 Priority = PriorityBase;
	for (const UInputMappingContext* Imc : Contexts)
	{
		if (Imc)
		{
			Subsys->AddMappingContext(Imc, Priority++, Opt);
		}
	}
}

void ADarkPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	check(InputComponent);
}

void ADarkPlayerController::ClientWasKicked_Implementation(const FText& KickReason)
{
	Super::ClientWasKicked_Implementation(KickReason);

	if (const UProxyInstance* Proxy = GetGameInstance()->GetSubsystem<UProxyInstance>())
	{
		Proxy->DestroySession();
	}
	if (UDarkGameInstance* DarkGameInstance = Cast<UDarkGameInstance>(GetGameInstance()))
	{
		DarkGameInstance->OnKicked(KickReason.ToString());
	}	
	ClientTravel(TEXT("/Game/ThirdPerson/Lvl_ThirdPerson"), TRAVEL_Absolute);
}

