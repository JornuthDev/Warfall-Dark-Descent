#pragma once
#include "CoreMinimal.h"
#include "OnlineKeyValuePair.h"
#include "InputMappingContext.h"
#include "UtilsTypes.generated.h"

UENUM()
enum class EBlueprintResultSwitch : uint8
{
	Success,
	Failure,
};

UENUM()
enum class EBlueprintResultBool : uint8
{
	OnTrue,
	OnFalse
};

USTRUCT(BlueprintType)
struct FPropertyKeyPair
{
	GENERATED_BODY()
	FName Key;
	FVariantData Data;
	FPropertyKeyPair() = default;
};

USTRUCT(BlueprintType)
struct FInputsRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UInputMappingContext*> Inputs;
	
	FInputsRow() = default;
};