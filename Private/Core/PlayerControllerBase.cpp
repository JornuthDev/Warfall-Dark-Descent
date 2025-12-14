#include "Core/PlayerControllerBase.h"
#include "EnhancedInputComponent.h"
#include "Core/DarkGameInstance.h"
#include "Core/PlayerStateBase.h"
#include "Core/Systems/OnlineSystem.h"
#include "Utils/Helpers/ContentHelpers.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
APlayerControllerBase::APlayerControllerBase()
{

}

void APlayerControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	// ═══════════════════════════════════════════════════════════════════════════════
	// ACTIVATION DU SYSTÈME - Attend le prochain tick pour s'assurer du setup
	// ═══════════════════════════════════════════════════════════════════════════════
	
	GetWorld()->GetTimerManager().SetTimerForNextTick([this, InPawn]()
	{
		
	});
}

void APlayerControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (Cast<UEnhancedInputComponent>(InputComponent))
	{
		ApplyInput("Default");
	}
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
void APlayerControllerBase::ApplyInput(const FName RowName, const int32 PriorityBase, const bool bClearExisting) const
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

void APlayerControllerBase::ApplyInputs(const TArray<FName>& RowNames, const int32 PriorityBase, const bool bClearExisting) const
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

UAbilitySystemComponent* APlayerControllerBase::GetAbilitySystemComponent() const
{
	// ═══════════════════════════════════════════════════════════════════════════════
	// Retourne le système actif du PlayerState
	// ═══════════════════════════════════════════════════════════════════════════════
	
	if (APlayerStateBase* PState = Cast<APlayerStateBase>(PlayerState))
	{
		return PState->GetAbilitySystemComponent();
	}
	return nullptr;
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Other Functions ───────────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────
void APlayerControllerBase::ClientWasKicked_Implementation(const FText& KickReason)
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

