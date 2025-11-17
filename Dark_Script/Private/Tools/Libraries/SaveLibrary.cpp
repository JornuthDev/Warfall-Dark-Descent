#include "Tools/Libraries/SaveLibrary.h"
#include "Core/SubSystems/SaveInstance.h"
#include "Kismet/GameplayStatics.h"

bool USaveLibrary::SaveGame(UObject* WorldContextObject, const bool bAsync, const FString& Instructions)
{
	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	if (!GameInstance) return false;
	USaveInstance* Save = GameInstance->GetSubsystem<USaveInstance>();
	if (!Save) return false;
	
	Save->NativeSaveGame(bAsync, WorldContextObject, Instructions);
	return true;
}

bool USaveLibrary::LoadGame(UObject* WorldContextObject, const FString& Instructions)
{
	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	if (!GameInstance) return false;
	USaveInstance* Save = GameInstance->GetSubsystem<USaveInstance>();
	if (!Save) return false;
	
	Save->NativeLoadGame(WorldContextObject, Instructions);
	return true;
}