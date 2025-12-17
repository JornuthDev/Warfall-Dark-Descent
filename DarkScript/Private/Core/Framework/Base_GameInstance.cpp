#include "Core/Framework/Base_GameInstance.h"

#include "Utils/Types/OnlineTypes.h"

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Blueprint API)
// ═══════════════════════════════════════════════════════════════════════

void UBase_GameInstance::OnKicked_Implementation(const EKickReason KickReason)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, 
						FString::Printf(TEXT("KICK REASON = %s"), *GetKickReasonText(KickReason).ToString()));
}
