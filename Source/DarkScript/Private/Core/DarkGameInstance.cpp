#include "Core/DarkGameInstance.h"

#include "Utils/Types/OnlineTypes.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
void UDarkGameInstance::OnKicked_Implementation(EKickReason KickReason)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, 
						FString::Printf(TEXT("KICK REASON = %s"), *GetKickReasonText(KickReason).ToString()));
}
