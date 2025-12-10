#include "Utils/Libraries/DarkLibrary.h"
#include "Components/Progression.h"
#include "Core/BasePlayerController.h"
#include "Kismet/GameplayStatics.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
ABasePlayerController* UDarkLibrary::GetPlayerController(const UObject* WorldContextObject, int32 PlayerIndex)
{
	return Cast<ABasePlayerController>(UGameplayStatics::GetPlayerController(WorldContextObject, PlayerIndex));
}

EClassArchetype UDarkLibrary::GetArchetype(const UObject* WorldContextObject)
{
	if (GetProgressionComponent(WorldContextObject))
	{
		return GetProgressionComponent(WorldContextObject)->GetClassArchetype();
	}
	return EClassArchetype::None;
}

int32 UDarkLibrary::GetPlayerLevel(const UObject* WorldContextObject)
{
	if (GetProgressionComponent(WorldContextObject))
	{
		return GetProgressionComponent(WorldContextObject)->GetPlayerLevel();
	}
	return INDEX_NONE;
}

void UDarkLibrary::SetArchetype(const UObject* WorldContextObject, const EClassArchetype Archetype)
{
	if (GetProgressionComponent(WorldContextObject))
	{
		GetProgressionComponent(WorldContextObject)->SetClassArchetype(Archetype);
	}
}

void UDarkLibrary::SetPlayerLevel(const UObject* WorldContextObject, const int32 PlayerLevel)
{
	if (GetProgressionComponent(WorldContextObject))
	{
		GetProgressionComponent(WorldContextObject)->SetPlayerLevel(PlayerLevel);
	}
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
UProgression* UDarkLibrary::GetProgressionComponent(const UObject* WorldContextObject)
{
	if (const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		return Cast<UProgression>(PlayerController->GetComponentByClass(UProgression::StaticClass()));
	}
	return nullptr;
}
