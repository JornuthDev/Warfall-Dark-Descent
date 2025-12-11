#include "Core/BasePlayerController.h"
#include "EnhancedInputComponent.h"
#include "Components/Progression.h"
#include "Core/DarkGameInstance.h"
#include "Core/Systems/OnlineSystem.h"
#include "Utils/Helpers/ContentHelpers.h"


// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
ABasePlayerController::ABasePlayerController()
{
	Progression = CreateDefaultSubobject<UProgression>(TEXT("Progression"));
	check(Progression);
}

void ABasePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		ApplyInput("Default");
	}
	
}

void ABasePlayerController::ApplyInput(const FName RowName, const int32 PriorityBase, const bool bClearExisting) const
{
	if (!IsLocalController()) return;
	
	FInputsRow Row;
	if (!ResolveInputsRow(RowName, Row))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Input] ApplyInputRow: Row '%s' not found in Inputs table."), *RowName.ToString());
		return;
	}
	ApplyInputContexts(GetLocalPlayer(), Row.Inputs, PriorityBase, bClearExisting);
}

void ABasePlayerController::ApplyInputs(const TArray<FName>& RowNames, const int32 PriorityBase, const bool bClearExisting) const
{
	if (!IsLocalController()) return;
	
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
	ApplyInputContexts(GetLocalPlayer(), Contexts, PriorityBase, bClearExisting);
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
void ABasePlayerController::ClientWasKicked_Implementation(const FText& KickReason)
{
	Super::ClientWasKicked_Implementation(KickReason);
	
	if (const UOnlineSystem* OnlineSystem = GetGameInstance()->GetSubsystem<UOnlineSystem>())
	{
		if (OnlineSystem->SessionInterface)
		{
			OnlineSystem->SessionInterface->DestroySession(NAME_GameSession);
		}
	}
	if (UDarkGameInstance* DarkGameInstance = Cast<UDarkGameInstance>(GetGameInstance()))
	{
		const EKickReason Reason = TextToKickReason(KickReason.ToString());
		DarkGameInstance->OnKicked(Reason);
	}
}
