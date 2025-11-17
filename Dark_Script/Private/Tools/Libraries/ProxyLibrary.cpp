#include "Tools/Libraries/ProxyLibrary.h"

#include "Core/SubSystems/ProxyInstance.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameSession.h"
#include "Kismet/GameplayStatics.h"

FString UProxyLibrary::GetSessionCode(const UObject* WorldContextObject)
{
	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	if (!GameInstance) return "";
	const UProxyInstance* Proxy = GameInstance->GetSubsystem<UProxyInstance>();
	if (!Proxy) return "";
	
	return Proxy->GetSessionCode();
}

bool UProxyLibrary::IsPublic(const UObject* WorldContextObject)
{
	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	if (!GameInstance) return false;
	const UProxyInstance* Proxy = GameInstance->GetSubsystem<UProxyInstance>();
	if (!Proxy) return false;
	
	return Proxy->IsPublic();
}

FString UProxyLibrary::GetSessionCode_FromSession(const FBlueprintSessionResult& Session)
{
	FString Code;
	if (Session.OnlineResult.Session.SessionSettings.Get("SESSION_CODE", Code))
	{
		return Code;
	}
	return "";
}

TArray<FSessionPropertyKeyPair> UProxyLibrary::GetExtraSettings(const UObject* WorldContextObject,
	const FBlueprintSessionResult& Session)
{
	FSessionPropertyKeyPair NewSetting;
	TArray<FSessionPropertyKeyPair> Results;
	for (auto& Elem : Session.OnlineResult.Session.SessionSettings.Settings)
	{
		NewSetting.Key = Elem.Key;
		NewSetting.Data = Elem.Value.Data;
		Results.Add(NewSetting);
	}
	return Results;
	
}

void UProxyLibrary::GetSessionSettings(UObject* WorldContextObject, int32& NumConnections, int32& NumPrivateConnections,
	bool& bIsLAN, bool& bAllowInvites, bool& bAllowJoinInProgress, bool& bIsAnticheatEnabled, int32& BuildUniqueID,
	TArray<FSessionPropertyKeyPair>& ExtraSettings, EBlueprintResultSwitch& Result)
{
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}
	UProxyInstance* Proxy = World->GetGameInstance()->GetSubsystem<UProxyInstance>();
	if (!Proxy)
	{
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	if (Proxy->GetSessionSettings(NumConnections, NumPrivateConnections, bIsLAN, bAllowInvites, bAllowJoinInProgress, bIsAnticheatEnabled, BuildUniqueID, ExtraSettings))
	{
		Result = EBlueprintResultSwitch::OnSuccess;
		return;
	}
	Result = EBlueprintResultSwitch::OnFailure;
}

FSessionPropertyKeyPair UProxyLibrary::MakeLiteralIntegerSetting(const FName Key,
                                                                 const int32 Integer)
{
	FSessionPropertyKeyPair KeyPair;
	KeyPair.Key = Key;
	KeyPair.Data.SetValue(Integer);
	return KeyPair;
}

FSessionPropertyKeyPair UProxyLibrary::MakeLiteralBoolSetting(const FName Key, const bool Bool)
{
	FSessionPropertyKeyPair KeyPair;
	KeyPair.Key = Key;
	KeyPair.Data.SetValue(Bool);
	return KeyPair;
}

FSessionPropertyKeyPair UProxyLibrary::MakeLiteralStringSetting(const FName Key, const FString String)
{
	FSessionPropertyKeyPair KeyPair;
	KeyPair.Key = Key;
	KeyPair.Data.SetValue(String);
	return KeyPair;
}

bool UProxyLibrary::KickPlayer(const UObject* WorldContextObject, APlayerController* PlayerToKick, const FString KickReason)
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
