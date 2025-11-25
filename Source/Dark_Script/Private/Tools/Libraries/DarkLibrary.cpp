#include "Tools/Libraries/DarkLibrary.h"
#include "Components/GuiController.h"
#include "Core/DarkGameInstance.h"
#include "Core/Game/DarkGameMode.h"
#include "Core/Game/DarkGameState.h"
#include "Core/DarkPlayerController.h"
#include "Core/SubSystems/NavigationInstance.h"
#include "Kismet/GameplayStatics.h"
#include "User Interfaces/NavigationRoot.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Blueprint API) ─────────────────────────────────────────────────

class ADarkPlayerController* UDarkLibrary::GetPlayerController(const UObject* WorldContextObject, const int32 PlayerIndex)
{
	return Cast<ADarkPlayerController>(UGameplayStatics::GetPlayerController(WorldContextObject, PlayerIndex));
}

class UGuiController* UDarkLibrary::GetGuiController(const UObject* WorldContextObject, const int32 PlayerIndex)
{
	const ADarkPlayerController* PC = GetPlayerController(WorldContextObject, PlayerIndex);
	if (!PC) return nullptr;
	
	return Cast<UGuiController>(PC->GuiController);
}

class ADarkGameMode* UDarkLibrary::GetGameMode(const UObject* WorldContextObject)
{
	return Cast<ADarkGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
}

class ADarkGameState* UDarkLibrary::GetGameState(const UObject* WorldContextObject)
{
	return Cast<ADarkGameState>(UGameplayStatics::GetGameState(WorldContextObject));
}

class UDarkGameInstance* UDarkLibrary::GetGameInstance(const UObject* WorldContextObject)
{
	return Cast<UDarkGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
}

void UDarkLibrary::SetNavigationRoot(UObject* WorldContextObject, UNavigationRoot* InNavigationRoot)
{
	if (!InNavigationRoot) return ;
	const UDarkGameInstance* GameInstance = GetGameInstance(WorldContextObject);
	if (!GameInstance) return;
	UNavigationInstance* Navigation = GameInstance->GetSubsystem<UNavigationInstance>();
	if (!Navigation) return;
	
	Navigation->SetNavigationRoot(InNavigationRoot);
}
