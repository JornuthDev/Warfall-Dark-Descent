#include "Core/BasePlayerController.h"
#include "Core/SaveGame/PlayerProfile.h"
#include "Core/SubSystems/SaveSystem.h"
#include "Net/UnrealNetwork.h"
#include "Tools/DarkHelpers.h"

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocalController())
	{
		if (USaveSystem* SaveSystem = GetGameInstance()->GetSubsystem<USaveSystem>())
		{
			SaveSystem->LoadPlayerProfile(this, "Loading");
		}
	
		ApplyInputs("Default");
	}
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
void ABasePlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ABasePlayerController, PlayedArchetype);
	DOREPLIFETIME(ABasePlayerController, PlayerLevel);
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
void ABasePlayerController::ApplyInputs(const FName RowName)
{
	const FInputsHelper Helper(GetLocalPlayer());
	Helper.ApplyInputs(RowName);
}

void ABasePlayerController::ApplyInputsArray(const TArray<FName>& RowNames)
{
	const FInputsHelper Helper(GetLocalPlayer());
	Helper.ApplyInputsArray(RowNames);
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
bool ABasePlayerController::SaveToPlayerProfile_Implementation(UPlayerProfile* SaveGame, const FString& Context)
{
	if (!SaveGame) return false;
	SaveGame->PlayedArchetype = PlayedArchetype;
	SaveGame->PlayerLevel = PlayerLevel;
	return true;
}

bool ABasePlayerController::LoadToPlayerProfile_Implementation(const UPlayerProfile* SaveGame, const FString& Context)
{
	if (!SaveGame) return false;

	PlayedArchetype = SaveGame->PlayedArchetype;
	PlayerLevel = SaveGame->PlayerLevel;
	
	if (GetNetMode() != NM_Standalone && !HasAuthority())
	{
		SynchronizeProperties_Internal(PlayedArchetype, PlayerLevel);
	}
	return true;
}

void ABasePlayerController::SwitchArchetype_Implementation()
{
	const UEnum* Enum = StaticEnum<EPlayerArchetype>();
	const int32 LastValidIndex = Enum->GetMaxEnumValue();

	int32 Index = static_cast<int32>(PlayedArchetype);
	Index++;

	if (Index <= 0 || Index > LastValidIndex - 1)
	{
		Index = 1;
	}
	PlayedArchetype = static_cast<EPlayerArchetype>(Index);	
	
	if (IsLocalController())
	{
		if (USaveSystem* SaveSystem = GetGameInstance()->GetSubsystem<USaveSystem>())
		{
			SaveSystem->SavePlayerProfile(this, "SwitchArchetype");
		}
	}
}

void ABasePlayerController::Call_SaveGame_Implementation(const bool bIsPlayerProfile)
{
	if (USaveSystem* SaveSystem = GetGameInstance()->GetSubsystem<USaveSystem>())
	{
		SaveSystem->SavePlayerProfile(this, "SwitchArchetype");
	}
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	check(InputComponent);
	if (LaunchMode::IsDevelopMode)
	{
		InputComponent->BindKey(EKeys::Dollar, IE_Pressed, this, &ABasePlayerController::SwitchArchetype);
	}
}



void ABasePlayerController::SynchronizeProperties_Internal_Implementation(const EPlayerArchetype InArchetype, const int32 InPlayerLevel)
{
	PlayedArchetype = InArchetype;
	PlayerLevel = InPlayerLevel;
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── OnRep_Functions (Internal) ────────────────────────────────────────────────

void ABasePlayerController::OnRep_PlayedArchetype()
{
	if (IsLocalController())
	{
		if (USaveSystem* SaveSystem = GetGameInstance()->GetSubsystem<USaveSystem>())
		{
			SaveSystem->SavePlayerProfile(this, "SwitchArchetype");
		}
	}
}