#include "Utils/Libraries/OnlineLibrary.h"
#include "Utils/Helpers/OnlineHelpers.h"

// ═══════════════════════════════════════════════════════════════════════════════
// ═══ FUNCTIONS (BLUEPRINT API) ═════════════════════════════════════════════════
// ═══════════════════════════════════════════════════════════════════════════════

FString UOnlineLibrary::GetPlayerSteamID(const UObject* WorldContextObject, const APlayerController* PlayerController)
{
	FOnlineHelper Helper = FOnlineHelper(WorldContextObject);
	Helper.QueryIDFromPlayerController(PlayerController);
	return Helper.UserSteamID;
}

FString UOnlineLibrary::GetPieInstance(const UObject* WorldContextObject, const APlayerController* PlayerController)
{
	FOnlineHelper Helper = FOnlineHelper(WorldContextObject);
	Helper.QueryIDFromPlayerController(PlayerController, true);
	return Helper.UserSteamID;
}
