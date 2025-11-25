#include "Core/BasePlayerController.h"

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
	/*ClientTravel(LevelPaths::Menu, TRAVEL_Absolute);*/
}
