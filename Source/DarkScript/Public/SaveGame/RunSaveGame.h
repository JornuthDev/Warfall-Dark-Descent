#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RunSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class DARKSCRIPT_API URunSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RunSave", SaveGame)
	int32 Index;
};
