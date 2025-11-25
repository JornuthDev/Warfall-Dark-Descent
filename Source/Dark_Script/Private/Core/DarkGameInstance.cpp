#include "Core/DarkGameInstance.h"

#include "Core/SubSystems/ProxyInstance.h"


// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
void UDarkGameInstance::Init()
{
	Super::Init();
}

void UDarkGameInstance::Shutdown()
{
	if (UProxyInstance* Proxy = GetSubsystem<UProxyInstance>())
	{
		Proxy->LeaveSession();
	}
	
	Super::Shutdown();
}

void UDarkGameInstance::OnKicked_Implementation(const FString& KickReason)
{
}
