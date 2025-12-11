#pragma once
#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Utils/Types/OnlineTypes.h"

struct FOnlineHelper
{
	explicit FOnlineHelper(const UObject* WorldContextObject,const FName SystemName = NAME_None)
		:World(GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		,OnlineSub(Online::GetSubsystem(World, SystemName))
	{
	}
	
	void QueryIDFromPlayerController(const APlayerController* PlayerController, const bool bIsDevelopMode = false)
	{
		if (bIsDevelopMode)
		{
			if (!World)
			{
				UserSteamID = "Default_User_SteamID";
				return;
			}
			for (const FWorldContext& Context : GEngine->GetWorldContexts())
			{
				if (Context.World() == World)
				{
					UserSteamID = FString::Printf(TEXT("PIE_Instance_%d"), Context.PIEInstance);
					return;
				}
			}
			UserSteamID = "Default_User_SteamID";
			return;
		}
		if (!PlayerController || !OnlineSub)
		{
			UserSteamID = "Default_User_SteamID";
			return;
		}
		if (const IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface(); Identity.IsValid())
		{
			const TSharedPtr<const FUniqueNetId> UniqueID = PlayerController->GetLocalPlayer()
				? PlayerController->GetLocalPlayer()->GetPreferredUniqueNetId().GetUniqueNetId()
				: nullptr;
			if (UniqueID.IsValid())
			{
				UserSteamID = UniqueID->ToString();
				return;
			}
		}
		UserSteamID = "Default_User_SteamID";
	}
private:
	UWorld* World;
public:	
	FString UserSteamID;
	IOnlineSubsystem* const OnlineSub;
};

inline FText GetSessionJoinErrorText(const ESessionJoinError Error)
{
	switch (Error)
	{
	case ESessionJoinError::None:
		return FText::FromString(TEXT("No error"));
	case ESessionJoinError::NetworkError:
		return FText::FromString(TEXT("Network error occurred"));
	case ESessionJoinError::SearchFailed:
		return FText::FromString(TEXT("Failed to search for sessions"));
	case ESessionJoinError::NoSessionsFound:
		return FText::FromString(TEXT("No sessions found"));
	case ESessionJoinError::InvalidSessionCode:
		return FText::FromString(TEXT("Invalid session code"));
	case ESessionJoinError::SessionNotFound:
		return FText::FromString(TEXT("Session not found"));
	case ESessionJoinError::SessionFull:
		return FText::FromString(TEXT("Session is full"));
	case ESessionJoinError::ArchetypeNotAvailable:
		return FText::FromString(TEXT("Your selected archetype is not available"));
	case ESessionJoinError::ArchetypeNotAllowed:
		return FText::FromString(TEXT("Your archetype is not allowed in this session"));
	case ESessionJoinError::PlayerLevelTooLow:
		return FText::FromString(TEXT("Your level is too low for this session"));
	case ESessionJoinError::SessionAlreadyStarted:
		return FText::FromString(TEXT("Session has already started"));
	case ESessionJoinError::SessionInProgress:
		return FText::FromString(TEXT("Players are already in the dungeon"));
	case ESessionJoinError::JoinFailed:
		return FText::FromString(TEXT("Failed to join session"));
	case ESessionJoinError::ConnectionFailed:
		return FText::FromString(TEXT("Connection to session failed"));
	case ESessionJoinError::Unknown:
	default:
		return FText::FromString(TEXT("Unknown error occurred"));
	}
}

inline ESessionType IntToSessionType(const int32 SessionType)
{
	return static_cast<ESessionType>(SessionType);	
}

inline FString ArchetypesToString(const TArray<EClassArchetype>& Archetypes)
{
	FString Result;
	for (int32 i = 0; i < Archetypes.Num(); ++i)
	{
		Result += FString::FromInt(static_cast<int32>(Archetypes[i]));
		if (i < Archetypes.Num() - 1)
		{
			Result += TEXT(",");
		}
	}
	return Result;
}

inline TArray<EClassArchetype> StringToArchetypes(const FString& ArchetypesString)
{
	TArray<EClassArchetype> Result;
	TArray<FString> StringArray;
	ArchetypesString.ParseIntoArray(StringArray, TEXT(","), true);
	
	static UEnum* EnumPtr = StaticEnum<EClassArchetype>();
	const int32 Count = EnumPtr->NumEnums();
	
	for (const FString& String : StringArray)
	{
		if (const int32 ArchetypeValue = FCString::Atoi(*String); ArchetypeValue >= 0 && ArchetypeValue <= Count -1)
		{
			Result.Add(static_cast<EClassArchetype>(ArchetypeValue));
		}
	}
	return Result;
}

inline TArray<EClassArchetype> GetAllArchetypes()
{
	TArray<EClassArchetype> Result;
	
	static UEnum* EnumPtr = StaticEnum<EClassArchetype>();
	const int32 Count = EnumPtr->NumEnums();
	for (int32 i = 1; i < Count -1; i++)
	{
		Result.Add(static_cast<EClassArchetype>(i));
	}
	return Result;
}