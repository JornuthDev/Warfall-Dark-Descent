#include "Core/Lobby/LobbyGameMode.h"

#include "Core/Framework/Base_PlayerController.h"
#include "Core/Framework/Base_PlayerState.h"
#include "Core/Systems/OnlineSystem.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/GameStateBase.h"
#include "Gameplay/AbilitySystem/GlobalAbilitySystem.h"
#include "Gameplay/AbilitySystem/GlobalAttributeSet.h"
#include "Utils/Log.h"

// ═══════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════

ALobbyGameMode::ALobbyGameMode()
{
	PlayerControllerClass = ABase_PlayerController::StaticClass();
	PlayerStateClass = ABase_PlayerState::StaticClass();
	Settings = nullptr;
	
	// DefaultPawn Class
}

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Blueprint API)
// ═══════════════════════════════════════════════════════════════════════

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if (!NewPlayer) return;
	
	if (IsHostPlayer(NewPlayer))
	{
		OnlineSystem = UOnlineSystem::GetSystem(NewPlayer);
		Settings = &OnlineSystem->SessionSettings;
	}
	
	{
		FScopeLock Lock(&SessionMutex);
		
		if (OnlineSystem && OnlineSystem->bInSession)
		{
			// ReSharper disable once CppTooWideScopeInitStatement
			EClassArchetype PlayerArchetype;
			// ReSharper disable once CppTooWideScopeInitStatement
			int32 PlayerLevel;
			const TWeakObjectPtr<APlayerController> WeakPlayer = NewPlayer;
			
			if (GetConnectInfos(NewPlayer, PlayerArchetype, PlayerLevel) && !IsHostPlayer(NewPlayer))
			{
				CheckingPlayer(WeakPlayer, PlayerArchetype, PlayerLevel);
			}
			else
			{
				if (const UGlobalAbilitySystem* System = UGlobalAbilitySystem::GetSystem(NewPlayer))
				{
					PlayerArchetype = System->GetClassArchetype();
					PlayerLevel = System->GetNumericAttribute(UGlobalAttributeSet::GetLevelAttribute());
				
					CheckingPlayer(WeakPlayer, PlayerArchetype, PlayerLevel, true);
				}
				else
				{
					OnlineSystem->LeaveSession();
				}
			}
		}
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Helpers)
// ═══════════════════════════════════════════════════════════════════════

bool ALobbyGameMode::GetConnectInfos(const APlayerController* NewPlayer, EClassArchetype& NewArchetype, int32& NewLevel)
{
	const UNetConnection* NetConnection = Cast<UNetConnection>(NewPlayer->Player);
	if (!NetConnection)
	{
		if (NewPlayer->GetGameInstance()->GetSubsystem<UOnlineSystem>()->bIsHost)
		{
			return false;
		}
		
		UE_LOG(Lobby, Warning, TEXT("NetConnection is null for player"));
		return false;
	}
	const FString PlayerOptionsString = NetConnection->RequestURL;

	// ✅ Parse Archetype
	EClassArchetype PlayerArchetype;
	if (FString ArchetypeStr; FParse::Value(*PlayerOptionsString, TEXT("Archetype="), ArchetypeStr))
	{
		int32 PlayerArchetypeIndex = FCString::Atoi(*ArchetypeStr);
		PlayerArchetype = static_cast<EClassArchetype>(PlayerArchetypeIndex);
	}
	else
	{
		return false;
	}
	
	// ✅ Parse Level
	int32 PlayerLevel;
	if (FString LevelStr; FParse::Value(*PlayerOptionsString, TEXT("Level="), LevelStr))
	{
		PlayerLevel = FCString::Atoi(*LevelStr);
	}
	else
	{
		return false;
	}
	
	NewArchetype = PlayerArchetype;
	NewLevel = PlayerLevel;
	return true;
}

bool ALobbyGameMode::IsHostPlayer(const APlayerController* NewPlayer) const
{
	if (!NewPlayer)
	{
		return false;
	}
	if (const AGameStateBase* GS = GetGameState<AGameStateBase>(); GS && GS->PlayerArray.Num() > 0)
	{
		return GS->PlayerArray[0]->GetPlayerController() == NewPlayer;
	}
	return false;
}

void ALobbyGameMode::CheckingPlayer(TWeakObjectPtr<APlayerController> NewPlayer, const EClassArchetype InArchetype,
	const int32 Level, const bool bIsHost) const
{
	if (!bIsHost)
	{
		auto KickPlayer = [this, NewPlayer] (const EKickReason& Reason)
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick([this, NewPlayer, Reason]
			{
				if (NewPlayer.IsValid())
				{
					GameSession->KickPlayer(NewPlayer.Get(), GetKickReasonText(Reason));
				}
			});
		};
		
		struct FRestrictions
		{
			FString Name;
			bool Result;
		};
		
		TArray<FRestrictions> Restrictions{
			{"Level", Settings->MinLevelRequired > 1},
			{"Archetype", Settings->AllowedArchetypes.Num() > 0}
		};
		
		auto HasRestrictions = [Restrictions] (const FString& Name ) -> bool
		{
			const FRestrictions* Found = Restrictions.FindByPredicate([&](const FRestrictions& Restriction)
			{
				return Restriction.Name == Name;
			});	
			if (Found)
			{
				return Found->Result;
			}
			return false;
		};
		
		if (GetPlayerCount() > Settings->MaxPlayers)
		{
			KickPlayer(EKickReason::MaxPlayersReached);
			return;
		}
		
		if (HasRestrictions("Level"))
		{
			if (Level < Settings->MinLevelRequired)
			{
				KickPlayer(EKickReason::LevelTooLow);
				return;
			}
			
		}
		
		if (HasRestrictions("Archetype"))
		{
			if (!Settings->AllowedArchetypes.Contains(InArchetype))
			{
				KickPlayer(EKickReason::ArchetypeNotAllowed);
				return;
			}
			if (!Settings->AvailableArchetypes.Contains(InArchetype))
			{
				KickPlayer(EKickReason::InvalidArchetype);
				return;
			}
		}
	}
	Settings->AvailableArchetypes.Remove(InArchetype);
	OnlineSystem->UpdateSessionSettings(*Settings);
}

int32 ALobbyGameMode::GetPlayerCount() const
{
	const AGameStateBase* GS = GetGameState<AGameStateBase>();
	return GS ? GS->PlayerArray.Num() : 0;
}

