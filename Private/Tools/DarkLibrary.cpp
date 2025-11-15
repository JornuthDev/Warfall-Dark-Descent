#include "Tools/DarkLibrary.h"

#include "Components/GuiController.h"
#include "Core/DarkGameInstance.h"
#include "Core/DarkGameMode.h"
#include "Core/DarkGameState.h"
#include "Core/DarkPlayerController.h"
#include "Core/SubSystems/SaveInstance.h"
#include "Core/SubSystems/NavigationInstance.h"
#include "Core/SubSystems/ProxyInstance.h"
#include "GameFramework/GameSession.h"
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

bool UDarkLibrary::SaveGame(UObject* WorldContextObject, const bool bAsync)
{
	const UDarkGameInstance* GameInstance = GetGameInstance(WorldContextObject);
	if (!GameInstance) return false;
	USaveInstance* Save = GameInstance->GetSubsystem<USaveInstance>();
	if (!Save) return false;
	
	Save->NativeSaveGame(bAsync, WorldContextObject);
	return true;
}

bool UDarkLibrary::LoadGame(UObject* WorldContextObject)
{
	const UDarkGameInstance* GameInstance = GetGameInstance(WorldContextObject);
	if (!GameInstance) return false;
	USaveInstance* Save = GameInstance->GetSubsystem<USaveInstance>();
	if (!Save) return false;
	
	Save->NativeLoadGame(WorldContextObject);
	return true;
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

FString UDarkLibrary::GetSessionCode(const UObject* WorldContextObject)
{
	const UDarkGameInstance* GameInstance = GetGameInstance(WorldContextObject);
	if (!GameInstance) return "";
	const UProxyInstance* Proxy = GameInstance->GetSubsystem<UProxyInstance>();
	if (!Proxy) return "";
	
	return Proxy->GetSessionCode();
}

bool UDarkLibrary::IsPublic(const UObject* WorldContextObject)
{
	const UDarkGameInstance* GameInstance = GetGameInstance(WorldContextObject);
	if (!GameInstance) return false;
	const UProxyInstance* Proxy = GameInstance->GetSubsystem<UProxyInstance>();
	if (!Proxy) return false;
	
	return Proxy->IsPublic();
}

FString UDarkLibrary::GetSessionCode_FromSession(const FBlueprintSessionResult& Session)
{
	FString Code;
	if (Session.OnlineResult.Session.SessionSettings.Get("SESSION_CODE", Code))
	{
		return Code;
	}
	return "";
}

bool UDarkLibrary::KickPlayer(const UObject* WorldContextObject, APlayerController* PlayerToKick, const FString KickReason)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (const AGameModeBase* GameMode = World->GetAuthGameMode())
		{
			if (GameMode->GameSession)
			{
				return GameMode->GameSession->KickPlayer(PlayerToKick, FText::FromString(KickReason));
			}
		}
	}

	return false;
}
