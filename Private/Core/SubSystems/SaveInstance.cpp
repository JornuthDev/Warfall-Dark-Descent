// ReSharper disable CppMemberFunctionMayBeConst
#include "Core/SubSystems/SaveInstance.h"

#include "Core/Save/DarkSaveGame.h"
#include "Core/Save/SaveableInterface.h"
#include "Kismet/GameplayStatics.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
void USaveInstance::SaveGame(const bool bAsync, UObject* ObjectToSave)
{
	NativeSaveGame(bAsync, ObjectToSave);
}

void USaveInstance::LoadGame(UObject* ObjectToLoad)
{
	NativeLoadGame(ObjectToLoad);
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
void USaveInstance::OnSaveFinished(const FString& SlotName, const int32 UserIndex, const bool bSuccess)
{
	OnSaveCompleted.Broadcast(bSuccess);
}

bool USaveInstance::CreateSaveGame()
{
	if (!SaveGameRef)
	{
		if (UGameplayStatics::DoesSaveGameExist(SaveSlot, 0))
		{
			SaveGameRef = Cast<UDarkSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlot, 0));
		}
		else
		{
			SaveGameRef = Cast<UDarkSaveGame>(UGameplayStatics::CreateSaveGameObject(UDarkSaveGame::StaticClass()));
		}
	}
	return IsValid(SaveGameRef);
}

void USaveInstance::NativeSaveGame(const bool bAsync, UObject* ObjectToSave)
{
	if (!ObjectToSave) return;
	if (!CreateSaveGame()) return;
	if (!ObjectToSave->GetClass()->ImplementsInterface(USaveableInterface::StaticClass())) return;
	
	ISaveableInterface::Execute_SaveObject(ObjectToSave, SaveGameRef);
	
	if (bAsync)
	{
		UGameplayStatics::AsyncSaveGameToSlot(
			SaveGameRef,
			SaveSlot,
			0,
			FAsyncSaveGameToSlotDelegate::CreateUObject(this, &USaveInstance::OnSaveFinished));
		return;
	}
	UGameplayStatics::SaveGameToSlot(SaveGameRef, SaveSlot, 0);
	OnSaveFinished(SaveSlot, 0, true);
}

void USaveInstance::NativeLoadGame(UObject* ObjectToLoad)
{
	if (!ObjectToLoad) return;
	
	if (CreateSaveGame() && ObjectToLoad->GetClass()->ImplementsInterface(USaveableInterface::StaticClass()))
	{
		ISaveableInterface::Execute_LoadObject(ObjectToLoad, SaveGameRef);
	}
}
