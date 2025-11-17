#pragma once
#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "ProxyTypes.generated.h"

UENUM(BlueprintType)
enum class EOnJoinSessionResult : uint8
{
	E_Success UMETA(DisplayName = "Success"),
	E_SessionFull UMETA(DisplayName = "Session Full"),
	E_WrongCode UMETA(DisplayName = "Wrong Code"),
	E_AlreadyInSession UMETA(DisplayName = "Already In Session")
};

UENUM(BlueprintType)
enum class ESessionVisibility : uint8
{
	E_Public UMETA(DisplayName = "Public"),
	E_Private UMETA(DisplayName = "Private"),
	E_All UMETA(DisplayName = "All")
};

USTRUCT(BlueprintType)
struct FSessionPropertyKeyPair
{
	GENERATED_USTRUCT_BODY()
	
	FName Key;
	FVariantData Data;
};

struct FSessionResultHelper
{
public:
	explicit FSessionResultHelper(const EOnJoinSessionCompleteResult::Type InResult)
		: Result(InResult)
	{
	}
	
	EOnJoinSessionResult GetResult() const
	{
		switch (Result)
		{
		case EOnJoinSessionCompleteResult::Success:
			{
				break;
			}
		case EOnJoinSessionCompleteResult::SessionIsFull:
			{
				return EOnJoinSessionResult::E_SessionFull;
			}
		case EOnJoinSessionCompleteResult::SessionDoesNotExist:
			{
				return EOnJoinSessionResult::E_WrongCode;
			}
		case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
			{
				return EOnJoinSessionResult::E_WrongCode;
			}
		case EOnJoinSessionCompleteResult::AlreadyInSession:
			{
				return EOnJoinSessionResult::E_AlreadyInSession;
			}
			
		case EOnJoinSessionCompleteResult::UnknownError:
			{
				return EOnJoinSessionResult::E_WrongCode;
			}
		}
		return EOnJoinSessionResult::E_Success;
	};
private:
	EOnJoinSessionCompleteResult::Type Result;
};