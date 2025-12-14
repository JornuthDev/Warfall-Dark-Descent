#include "Utils/BlueprintAsync/LeaveSessionOnlineSystem.h"

#include "Core/Systems/OnlineSystem.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
ULeaveSessionOnlineSystem* ULeaveSessionOnlineSystem::Create(const UObject* WorldContextObject)
{
	ULeaveSessionOnlineSystem* Node = NewObject<ULeaveSessionOnlineSystem>();
	Node->WorldPtr = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	return Node;
}

	
// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
void ULeaveSessionOnlineSystem::Activate()
{
	Super::Activate();
	
	if (!WorldPtr.IsValid())
	{
		OnSessionDestroyedComplete(false);
		return;
	}
	const UGameInstance* GameInstance = WorldPtr->GetGameInstance();
	if (!GameInstance)
	{
		OnSessionDestroyedComplete(false);
		return;
	}
	UOnlineSystem* OnlineSystem = GameInstance->GetSubsystem<UOnlineSystem>();
	if (!OnlineSystem)
	{
		OnSessionDestroyedComplete(false);
		return;
	}
	OnlineSystem->OnSessionDestroyed.AddDynamic(this, &ULeaveSessionOnlineSystem::OnSessionDestroyedComplete);
	OnlineSystem->LeaveSession();
}

void ULeaveSessionOnlineSystem::OnSessionDestroyedComplete(const bool bSuccess)
{
	if (bSuccess)
	{
		OnSuccess.Broadcast();
	}
	else
	{
		OnFailure.Broadcast();
	}
	OnSuccess.Clear();
	OnFailure.Clear();
	SetReadyToDestroy();
}
