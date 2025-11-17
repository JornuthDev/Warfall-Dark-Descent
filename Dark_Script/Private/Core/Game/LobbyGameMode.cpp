#include "Core/Game/LobbyGameMode.h"

#include "Core/DarkPlayerController.h"
#include "Core/SubSystems/ProxyInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	PlayerControllers.AddUnique(NewPlayer);
	WaitingBeforeChecking(1.f);
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	if (APlayerController* PC = Cast<APlayerController>(Exiting))
	{
		PlayerControllers.Remove(PC);
	}
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
void ALobbyGameMode::WaitingBeforeChecking(const float Duration)
{
	const UWorld* World = GetWorld();
	if (!World) return;

	// Cancel previous delay
	World->GetTimerManager().ClearTimer(CheckingPlayerHandle);

	// Start a new delay
	World->GetTimerManager().SetTimer(
		CheckingPlayerHandle,
		this,
		&ALobbyGameMode::CheckingPlayers,
		Duration,
		false
	);
}

void ALobbyGameMode::CheckingPlayers()
{
	if (UProxyInstance* Proxy = GetGameInstance()->GetSubsystem<UProxyInstance>(); Proxy && Proxy->IsOnline())
	{
		// ReSharper disable once CppTooWideScopeInitStatement
		int32 NumPrivateConnections = 0;
		// ReSharper disable once CppTooWideScopeInitStatement
		bool bIsLAN = false;
		// ReSharper disable once CppTooWideScopeInitStatement
		bool bIsAnticheat = false;
		// ReSharper disable once CppTooWideScopeInitStatement
		int32 BuildUniqueID = 0;
		if (Proxy->GetSessionSettings(NumConnections, NumPrivateConnections, bIsLAN, bAllowInvites, bAllowJoinInProgress, bIsAnticheat, BuildUniqueID, ExtraSettings))
		{
			if (GameState->PlayerArray.Num() > NumConnections)
			{
				for (int32 i = PlayerControllers.Num(); i-- > 0; )
				{
					if (i >= NumConnections)
					{
						APlayerController* PC = PlayerControllers[i];
						if (PC->GetCharacter())
						{
							PC->GetCharacter()->Destroy();
						}
						GameSession->KickPlayer(PC, FText::FromString(KickReason::SessionFull));
					}
				}
			}
			for (APlayerController* PC : PlayerControllers)
			{
				if (ADarkPlayerController* DarkPC = Cast<ADarkPlayerController>(PC); DarkPC && !DarkPC->GetCharacter())
				{
					DarkPC->SpawnPlayer_Request();
				}
			}
			Proxy->UpdateSession(NumConnections, NumPrivateConnections, bIsLAN, 
				bAllowInvites, bAllowJoinInProgress, false, true, 
				true, false, ExtraSettings, true);
		}
	}
	else
	{
		NumConnections = 4;
		if (GameState->PlayerArray.Num() > NumConnections)
		{
			for (int32 i = PlayerControllers.Num(); i-- > 0; )
			{
				if (i >= NumConnections)
				{
					APlayerController* PC = PlayerControllers[i];
					if (PC->GetCharacter())
					{
						PC->GetCharacter()->Destroy();
					}
					GameSession->KickPlayer(PC, FText::FromString(KickReason::SessionFull));
				}
			}
		}
		for (APlayerController* PC : PlayerControllers)
		{
			if (ADarkPlayerController* DarkPC = Cast<ADarkPlayerController>(PC); DarkPC && !DarkPC->GetCharacter())
			{
				DarkPC->SpawnPlayer_Request();
			}
		}
	}
}



