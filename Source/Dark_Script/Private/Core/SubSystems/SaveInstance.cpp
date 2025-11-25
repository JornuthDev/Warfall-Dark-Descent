// ReSharper disable CppMemberFunctionMayBeConst
#include "Core/SubSystems/SaveInstance.h"

#include "Core/Save/DarkSaveGame.h"
#include "Core/Save/SaveableInterface.h"
#include "Kismet/GameplayStatics.h"


// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
void USaveInstance::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UWorld* World = GetWorld();
	int32 FoundIndex = -1;
    
	for (const FWorldContext& Context : GEngine->GetWorldContexts())
	{
		if (Context.World() == World)
		{
			FoundIndex = Context.PIEInstance;
			break;
		}
	}

	if (FoundIndex != -1)
	{
		SaveSlot = FString::Printf(TEXT("%s_%d"), *SaveSlot, FoundIndex);
		UE_LOG(LogTemp, Warning, TEXT("SaveSlot for this instance: %s"), *SaveSlot);
	}

}

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

void USaveInstance::NativeSaveGame(const bool bAsync, UObject* ObjectToSave, FString Instructions)
{
	if (!ObjectToSave) return;
	if (!CreateSaveGame()) return;
	if (!ObjectToSave->GetClass()->ImplementsInterface(USaveableInterface::StaticClass())) return;

	if (const bool bSuccess = ISaveableInterface::Execute_SaveObject(ObjectToSave, SaveGameRef, Instructions); !bSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to save %s"), *ObjectToSave->GetName())
		return;
	}
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

void USaveInstance::NativeLoadGame(UObject* ObjectToLoad, FString Instructions)
{
	if (!ObjectToLoad) return;
	
	if (CreateSaveGame() && ObjectToLoad->GetClass()->ImplementsInterface(USaveableInterface::StaticClass()))
	{
		if (ISaveableInterface::Execute_LoadObject(ObjectToLoad, SaveGameRef, Instructions)) return;
		UE_LOG(LogTemp, Warning, TEXT("Failed to load %s"), *ObjectToLoad->GetName())
	}
}
