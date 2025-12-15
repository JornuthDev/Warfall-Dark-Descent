#include "Core/Framework/Base_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "Utils/Helpers/SystemsHelper.h"
#include "Utils/Types/UtilsTypes.h"

// ═══════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════

ABase_PlayerController::ABase_PlayerController()
{
}

void ABase_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
}

void ABase_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (ApplyInput("Default"))
		{
			
		}
	}
}

void ABase_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (SaveSystem::Get(this))
	{
		SaveSystem::Get(this)->InitializeSaveSlots(this);
	}
	else
	{
		UE_LOG(DarkScript, Error, TEXT("Base_PlayerController::BeginPlay: SaveSystem is not valid!"))
	}
	
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Blueprint API)
// ═══════════════════════════════════════════════════════════════════════

bool ABase_PlayerController::ApplyInput(const FName RowName, const int32 PriorityBase, const bool bClearExisting) const
{
	if (!IsLocalController()) return false;

	if (const TArray<FName> RowNames({RowName}); !FInputsRow::ApplyInputsContexts(GetLocalPlayer(), RowNames, PriorityBase, bClearExisting))
	{
		UE_LOG(DarkScript, Error, TEXT("[Input] Base_PlayerController::ApplyInputsContexts: Failed."));
		return false;
	}
	return true;
}

bool ABase_PlayerController::ApplyInputs(const TArray<FName>& RowNames, const int32 PriorityBase, const bool bClearExisting) const
{
	if (!IsLocalController()) return false;
	
	if (!FInputsRow::ApplyInputsContexts(GetLocalPlayer(), RowNames, PriorityBase, bClearExisting))
	{
		UE_LOG(DarkScript, Error, TEXT("[Input] Base_PlayerController::ApplyInputsContexts: Failed."));
		return false;
	}
	return true;
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Override)
// ═══════════════════════════════════════════════════════════════════════

void ABase_PlayerController::ClientWasKicked_Implementation(const FText& KickReason)
{
	Super::ClientWasKicked_Implementation(KickReason);
}
