#pragma once
#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "PlayerTypes.h"
#include "OnlineTypes.generated.h"

enum class EClassArchetype : uint8;

UENUM(BlueprintType)
enum class ESessionType : uint8
{
	None UMETA(DisplayName = "None"),
	Private UMETA(DisplayName = "Private"),
	Public UMETA(DisplayName = "Public"),
	QuickMatch UMETA(DisplayName = "QuickMatch")
};

UENUM(BlueprintType)
enum class EKickReason : uint8
{
	None UMETA(DisplayName = "None"),
	HostLeft UMETA(DisplayName = "Host Left"),
	InvalidArchetype UMETA(DisplayName = "Invalid Archetype"),
	MaxPlayersReached UMETA(DisplayName = "Max Players Reached"),
	ArchetypeNotAllowed UMETA(DisplayName = "Archetype Not Allowed"),
	LevelTooLow UMETA(DisplayName = "Level Too Low"),
	ManualKick UMETA(DisplayName = "Manual Kick"),
	SessionStarted UMETA(DisplayName = "Session Started")
};

UENUM(BlueprintType)
enum class ESessionJoinError : uint8
{
	None UMETA(DisplayName = "None"),
	// Network/Search errors
	NetworkError UMETA(DisplayName = "Network Error"),
	SearchFailed UMETA(DisplayName = "Search Failed"),
	NoSessionsFound UMETA(DisplayName = "No Sessions Found"),
	// Code-specific errors
	InvalidSessionCode UMETA(DisplayName = "Invalid Session Code"),
	SessionNotFound UMETA(DisplayName = "Session Not Found"),
	// Eligibility errors
	SessionFull UMETA(DisplayName = "Session Full"),
	ArchetypeNotAvailable UMETA(DisplayName = "Archetype Not Available"),
	ArchetypeNotAllowed UMETA(DisplayName = "Archetype Not Allowed"),
	PlayerLevelTooLow UMETA(DisplayName = "Player Level Too Low"),
	// Session state errors
	SessionAlreadyStarted UMETA(DisplayName = "Session Already Started"),
	SessionInProgress UMETA(DisplayName = "Session In Progress"),
	// Join errors
	JoinFailed UMETA(DisplayName = "Join Failed"),
	ConnectionFailed UMETA(DisplayName = "Connection Failed"),
	Unknown UMETA(DisplayName = "Unknown Error")
};

USTRUCT()
struct FSessionFilters
{
	GENERATED_BODY()
	
	UPROPERTY()
	ESessionType SessionType = ESessionType::None;
	UPROPERTY()
	FString SessionCode;
	UPROPERTY()
	TArray<EClassArchetype> Archetypes;
	UPROPERTY()
	int32 MinPlayers = 1;
	UPROPERTY()
	int32 MinLevelRequired = 1;
};

USTRUCT()
struct FDarkSessionSettings
{
	GENERATED_BODY()
	
	UPROPERTY()
	ESessionType SessionType = ESessionType::None;
	UPROPERTY()
	FString SessionName;
	UPROPERTY()
	FString SessionCode;
	UPROPERTY()
	int32 MaxPlayers = 4;
	UPROPERTY()
	TArray<EClassArchetype> AllowedArchetypes;
	UPROPERTY()
	int32 MinLevelRequired = 1;
	UPROPERTY()
	TArray<EClassArchetype> AvailableArchetypes;
};

USTRUCT(BlueprintType)
struct FSessionSearchResult
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Category = "Session")
	FString SessionName;
		
	UPROPERTY(BlueprintReadOnly, Category = "Session")
	int32 CurrentPlayers = 0;
	
	UPROPERTY(BlueprintReadOnly, Category = "Session")
	int32 MaxPlayers = 4;
	
	UPROPERTY(BlueprintReadOnly, Category = "Session")
	int32 Ping = 0;
	
	UPROPERTY(BlueprintReadOnly, Category = "Session")
	TArray<EClassArchetype> AvailableArchetypes;
	
	ESessionType SessionType = ESessionType::None;
	FString SessionCode;
	int32 MinPlayers = 1;
	int32 MinLevelRequired = 1;
	TArray<EClassArchetype> AllowedArchetypes;
	FOnlineSessionSearchResult OnlineResults;
};

// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionCreated, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSessionJoined, bool, bWasSuccessful, ESessionJoinError, ErrorReason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionDestroyed, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSessionSearchComplete, const TArray<FSessionSearchResult>&, Results, bool, bWasSuccessful, ESessionJoinError, ErrorReason);

namespace SessionSettingsKeys
{
	const FName SESSION_TYPE = FName(TEXT("SESSION_TYPE"));
	const FName SESSION_CUSTOM_NAME = FName(TEXT("SESSION_CUSTOM_NAME"));
	const FName SESSION_CODE = FName(TEXT("SESSION_CODE"));
	const FName ALLOWED_ARCHETYPES = FName(TEXT("ALLOWED_ARCHETYPES"));
	const FName MIN_LEVEL_REQUIRED = FName(TEXT("MIN_LEVEL_REQUIRED"));
	const FName AVAILABLE_ARCHETYPES = FName(TEXT("AVAILABLE_ARCHETYPES"));
}

inline FText GetKickReasonText(const EKickReason Reason)
{
	switch (Reason)
	{
	case EKickReason::HostLeft:
		return FText::FromString(TEXT("Host left session"));
	case EKickReason::InvalidArchetype:
		return FText::FromString(TEXT("Archetype already taken by another player"));
	case EKickReason::MaxPlayersReached:
		return FText::FromString(TEXT("Session is full"));
	case EKickReason::ArchetypeNotAllowed:
		return FText::FromString(TEXT("This archetype is not allowed in this session"));
	case EKickReason::LevelTooLow:
		return FText::FromString(TEXT("Your level is too low for this session"));
	case EKickReason::ManualKick:
		return FText::FromString(TEXT("Kicked by host"));
	case EKickReason::SessionStarted:
		return FText::FromString(TEXT("Session has already started"));
	default:
		return FText::FromString(TEXT("Kicked from lobby"));
	}
}

inline EKickReason TextToKickReason(const FString& ReasonText)
{
	if (ReasonText == TEXT("Host left session"))
		return EKickReason::HostLeft;
	if (ReasonText == TEXT("Archetype already taken by another player"))
		return EKickReason::InvalidArchetype;
	if (ReasonText == TEXT("Session is full"))
		return EKickReason::MaxPlayersReached;
	if (ReasonText == TEXT("This archetype is not allowed in this session"))
		return EKickReason::ArchetypeNotAllowed;
	if (ReasonText == TEXT("Your level is too low for this session"))
		return EKickReason::LevelTooLow;
	if (ReasonText == TEXT("Kicked by host"))
		return EKickReason::ManualKick;
	if (ReasonText == TEXT("Session has already started"))
		return EKickReason::SessionStarted;
	
	return EKickReason::None;
}