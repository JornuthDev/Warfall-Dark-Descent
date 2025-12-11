#include "Components/Progression.h"
#include "SaveGame/PlayerSaveGame.h"
#include "Net/UnrealNetwork.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
UProgression::UProgression()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UProgression::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UProgression, Archetype);
	DOREPLIFETIME(UProgression, PlayerLevel);
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
bool UProgression::SaveToPlayer_Implementation(UPlayerSaveGame* SaveGame, const FString& Context)
{
	SaveGame->Archetype = Archetype;
	SaveGame->PlayerLevel = PlayerLevel;
	return true;
}

bool UProgression::LoadFromPlayer_Implementation(UPlayerSaveGame* SaveGame, const FString& Context)
{
	Server_SynchronizeProperties(SaveGame->Archetype, SaveGame->PlayerLevel);
	return true;
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (replicates) ────────────────────────────────────────────────────
void UProgression::OnRep_PlayerSave()
{
	if (IsLocalController())
	{
		if (GetSaveSystem())
		{
			GetSaveSystem()->SaveGame(ESaveType::PlayerSave, this, "OnRep_PlayerSave");
		}
	}
}

void UProgression::SetClassArchetype_Implementation(const EClassArchetype NewArchetype)
{
	if (NewArchetype != EClassArchetype::None)
	{
		Archetype = NewArchetype;
		
		if (IsLocalController())
		{
			if (GetSaveSystem())
			{
				GetSaveSystem()->SaveGame(ESaveType::PlayerSave, this, "SetArchetype");
			}
		}
	}
}

void UProgression::SetPlayerLevel_Implementation(const int32 NewLevel)
{
	if (NewLevel >= 1)
	{
		PlayerLevel = NewLevel;
		
		if (IsLocalController())
		{
			if (GetSaveSystem())
			{
				GetSaveSystem()->SaveGame(ESaveType::PlayerSave, this, "SetPlayerLevel");
			}
		}
	}
}

void UProgression::Server_SynchronizeProperties_Implementation(const EClassArchetype InArchetype, const int32 InLevel)
{
	Archetype = InArchetype;
	PlayerLevel = InLevel;
	
	if (GetOwner()->HasAuthority() && OnPlayerDataSync.IsBound())
	{
		OnPlayerDataSync.Execute(InArchetype, InLevel);
	}
}

void UProgression::ClientLoadAndSendPlayerData_Implementation()
{
	if (USaveSystem* SaveSystem = GetSaveSystem())
	{
		SaveSystem->LoadGame(ESaveType::PlayerSave, this, "ClientLoadAndSendPlayerData");
	}
}

