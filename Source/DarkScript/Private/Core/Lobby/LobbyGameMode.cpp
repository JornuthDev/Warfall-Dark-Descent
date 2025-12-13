#include "Core/Lobby/LobbyGameMode.h"

#include "Components/Progression.h"
#include "Core/PlayerControllerBase.h"
#include "Core/CharacterBase.h"
#include "Core/PlayerStateBase.h"
#include "Core/Systems/OnlineSystem.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerState.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
ALobbyGameMode::ALobbyGameMode()
{
	PlayerControllerClass = APlayerControllerBase::StaticClass();
	PlayerStateClass = APlayerStateBase::StaticClass();
	DefaultPawnClass = nullptr;
	
	static ConstructorHelpers::FClassFinder<ACharacterBase> CharacterClassFinder(
		TEXT("/DarkScript/Core/BP_Player")
	);
    
	if (CharacterClassFinder.Succeeded())
	{
		ClassToSpawn = CharacterClassFinder.Class;
	}
}

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if (!NewPlayer)
	{
		return;
	}
	
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		OnlineSystem = GameInstance->GetSubsystem<UOnlineSystem>();
	}
	
	if (OnlineSystem && OnlineSystem->bInSession)
	{
		FScopeLock Lock(&SessionMutex);
		const int32 CurrentPlayers = GetPlayerCount();
		const FDarkSessionSettings Settings = OnlineSystem->SessionSettings;
		TWeakObjectPtr<APlayerController> WeakPlayer = NewPlayer;
		
		if (CurrentPlayers > Settings.MaxPlayers)
		{
			if (!IsHostPlayer(NewPlayer))
			{
				GetWorld()->GetTimerManager().SetTimerForNextTick([this, WeakPlayer] ()
				{
					if (WeakPlayer.IsValid())
					{
						GameSession->KickPlayer(WeakPlayer.Get(), GetKickReasonText(EKickReason::MaxPlayersReached));
					}
				});
				return;
			}
		}
		if (UProgression* Progression = Cast<UProgression>(NewPlayer->GetComponentByClass(UProgression::StaticClass())))
		{
			Progression->OnPlayerDataSync.BindLambda([this, NewPlayer, Progression](const EClassArchetype Archetype, const int32 Level)
			{
				OnPlayerDataReceived(NewPlayer, Progression, Archetype, Level);
			});
		}
		if (UProgression* Progression = Cast<UProgression>(NewPlayer->GetComponentByClass(UProgression::StaticClass())))
		{
			Progression->ClientLoadAndSendPlayerData();
		}
	}
	else
	{
		if (UProgression* Progression = Cast<UProgression>(NewPlayer->GetComponentByClass(UProgression::StaticClass())))
		{
			Progression->ClientLoadAndSendPlayerData();
			SpawnPlayerCharacter(NewPlayer);
		}
	}
}

void ALobbyGameMode::Logout(AController* ExitingController)
{
	if (const APlayerController* PC = Cast<APlayerController>(ExitingController))
	{
		if (const UProgression* Progression = Cast<UProgression>(PC->GetComponentByClass(UProgression::StaticClass())))
		{
			const EClassArchetype PlayerArchetype = Progression->GetClassArchetype();
			if (const UGameInstance* GameInstance = GetGameInstance())
			{
				if (OnlineSystem = GameInstance->GetSubsystem<UOnlineSystem>(); OnlineSystem)
				{
					if (OnlineSystem->bInSession)
					{
						FScopeLock Lock(&SessionMutex);
						if (FDarkSessionSettings Settings = OnlineSystem->SessionSettings; Settings.AllowedArchetypes.Contains(PlayerArchetype))
						{
							if (!Settings.AvailableArchetypes.Contains(PlayerArchetype))
							{
								Settings.AvailableArchetypes.Add(PlayerArchetype);
								OnlineSystem->UpdateSessionSettings(Settings);
							}
						}
					}
				}
			}
		}
	}
    
	Super::Logout(ExitingController);
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
bool ALobbyGameMode::IsHostPlayer(const APlayerController* PlayerController) const
{
	if (!PlayerController)
	{
		return false;
	}
	if (const AGameStateBase* GS = GetGameState<AGameStateBase>(); GS && GS->PlayerArray.Num() > 0)
	{
		return GS->PlayerArray[0]->GetPlayerController() == PlayerController;
	}
	return false;
}

void ALobbyGameMode::OnPlayerDataReceived(APlayerController* Player, UProgression* Progression, const EClassArchetype Archetype, const int32 Level)
{
	if (OnlineSystem && OnlineSystem->bInSession)
	{
		const bool bIsHost = IsHostPlayer(Player);
				
		TWeakObjectPtr<APlayerController> WeakPlayer = Player;
		FScopeLock Lock(&SessionMutex);
		FDarkSessionSettings Settings = OnlineSystem->SessionSettings;
				
		if (HasLevelRestriction(Settings))
		{
			if (Level < Settings.MinLevelRequired)
			{
				if (!bIsHost)
				{
					Progression->OnPlayerDataSync.Unbind();
					GetWorld()->GetTimerManager().SetTimerForNextTick([this, WeakPlayer] ()
					{
						if (WeakPlayer.IsValid())
						{
							GameSession->KickPlayer(WeakPlayer.Get(), GetKickReasonText(EKickReason::LevelTooLow));
						}
					});
					return;
				}
			}
		}
		if (HasArchetypeRestriction(Settings))
		{
			if (!Settings.AllowedArchetypes.Contains(Archetype))
			{
				if (!bIsHost)
				{
					Progression->OnPlayerDataSync.Unbind();
					GetWorld()->GetTimerManager().SetTimerForNextTick([this, WeakPlayer] ()
					{
						if (WeakPlayer.IsValid())
						{
							GameSession->KickPlayer(WeakPlayer.Get(), GetKickReasonText(EKickReason::ArchetypeNotAllowed));
						}
					});
					return;
				}
			}
			if (!Settings.AvailableArchetypes.Contains(Archetype))
			{
				if (!bIsHost)
				{
					Progression->OnPlayerDataSync.Unbind();
					GetWorld()->GetTimerManager().SetTimerForNextTick([this, WeakPlayer] ()
					{
						if (WeakPlayer.IsValid())
						{
							GameSession->KickPlayer(WeakPlayer.Get(), GetKickReasonText(EKickReason::InvalidArchetype));
						}
					});
					return;
				}
			}
			Settings.AvailableArchetypes.Remove(Archetype);
			OnlineSystem->UpdateSessionSettings(Settings);
		}
	}
	Progression->OnPlayerDataSync.Unbind();
	SpawnPlayerCharacter(Player);
}

void ALobbyGameMode::SpawnPlayerCharacter(APlayerController* Player) const
{
	if (!Player || !ClassToSpawn) return;

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);
	SpawnTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
	SpawnTransform.SetRotation(FRotator::ZeroRotator.Quaternion());

	// Utilise SpawnActorDeferred pour éviter les problèmes de collision
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
	ACharacterBase* NewCharacter = GetWorld()->SpawnActor<ACharacterBase>(
		ClassToSpawn, 
		SpawnTransform, 
		SpawnParams
	);

	if (NewCharacter)
	{
		Player->Possess(NewCharacter);
	}
}
