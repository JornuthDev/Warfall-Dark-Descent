#include "Core/Lobby/LobbyGameMode.h"

#include "AbilitySystemComponent.h"
#include "Core/PlayerControllerBase.h"
#include "Core/CharacterBase.h"
#include "Core/PlayerStateBase.h"
#include "Core/Systems/OnlineSystem.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/AbilitySystem/GlobalAttributeSet.h"

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
	const APlayerStateBase* PState = Cast<APlayerStateBase>(NewPlayer->PlayerState);
	if (!PState)
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
		FDarkSessionSettings Settings = OnlineSystem->SessionSettings;
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
		const bool bIsHost = IsHostPlayer(NewPlayer);
		const int32 PlayerLevel = static_cast<int32>(PState->GetAbilitySystemComponent()->GetNumericAttribute(UGlobalAttributeSet::GetLevelAttribute()));
		const EClassArchetype PlayerArchetype = PState->CurrentArchetype;
		if (HasLevelRestriction(Settings))
		{
			if (PlayerLevel < Settings.MinLevelRequired)
			{
				if (!bIsHost)
				{
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
			if (!Settings.AllowedArchetypes.Contains(PlayerArchetype))
			{
				if (!bIsHost)
				{
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
			if (!Settings.AvailableArchetypes.Contains(PlayerArchetype))
			{
				if (!bIsHost)
				{
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
			Settings.AvailableArchetypes.Remove(PlayerArchetype);
			OnlineSystem->UpdateSessionSettings(Settings);
		}
	}
	SpawnPlayerCharacter(NewPlayer);
}

void ALobbyGameMode::Logout(AController* ExitingController)
{
	if (const AController* PC = Cast<APlayerController>(ExitingController))
	{
		if (APlayerStateBase* PState = Cast<APlayerStateBase>(PC->PlayerState))
		{
			const EClassArchetype PlayerArchetype = PState->CurrentArchetype;
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
