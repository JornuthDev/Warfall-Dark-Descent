// ReSharper disable CppMemberFunctionMayBeConst
#include "Core/Systems/SaveSystem.h"

#include "Interfaces/SaveableInterface.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/PlayerSaveGame.h"
#include "SaveGame/RunSaveGame.h"
#include "Utils/Helpers/OnlineHelpers.h"

void USaveSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	FOnlineHelper Helper = FOnlineHelper(this);
	Helper.QueryIDFromPlayerController(UGameplayStatics::GetPlayerController(this, 0), true);
	
	static UEnum* EnumPtr = StaticEnum<ESaveType>();
	const int32 Count = EnumPtr->NumEnums();
	
	for (uint8 i = 0; i < Count - 1; i++)
	{
		const ESaveType SaveType = static_cast<ESaveType>(EnumPtr->GetValueByIndex(i));
		
		FString EnumName = EnumPtr->GetNameStringByIndex(i);
		EnumName.RemoveFromStart(TEXT("ESaveType::"));
		
		FString SlotName = FString::Printf(TEXT("%s_%s"), *EnumName, *Helper.UserSteamID);
		
		Slots.Add(SaveType, SlotName);
		SaveGames.Add(SaveType, nullptr);
	}
	
	Create();
}

void USaveSystem::Deinitialize()
{
	Super::Deinitialize();
}

void USaveSystem::Create()
{
	for (auto& It : SaveGames)
	{
		if (UGameplayStatics::DoesSaveGameExist(GetSlot(It.Key), 0))
		{
			It.Value = UGameplayStatics::LoadGameFromSlot(GetSlot(It.Key), 0);
		}
		else
		{
			const TSubclassOf<USaveGame> Class = GetSaveClass(It.Key);
			It.Value = UGameplayStatics::CreateSaveGameObject(Class);
		}
	}
}

void USaveSystem::SaveGame(const ESaveType Type, UObject* ObjectToSave, const FString& Context)
{
	if (!ObjectToSave || !ObjectToSave->GetClass()->ImplementsInterface(USaveableInterface::StaticClass()) || !SaveGames[Type])
	{
		OnSaveCompleteHandle(GetSlot(Type), 0, false);
		return;
	}
	switch (Type)
	{
		case ESaveType::PlayerSave:
			{
				if (ISaveableInterface::Execute_SaveToPlayer(ObjectToSave, Cast<UPlayerSaveGame>(SaveGames[Type]), Context))
				{
					UGameplayStatics::AsyncSaveGameToSlot(
						SaveGames[Type], 
						GetSlot(Type), 
						0,
						FAsyncSaveGameToSlotDelegate::CreateUObject(this, &USaveSystem::OnSaveCompleteHandle));
					return;
				}
				OnSaveCompleteHandle(GetSlot(Type), 0, false);
				return;
			}
		case ESaveType::RunSave:
			{
				if (ISaveableInterface::Execute_SaveToRun(ObjectToSave, Cast<URunSaveGame>(SaveGames[Type]), Context))
				{
					UGameplayStatics::AsyncSaveGameToSlot(
						SaveGames[Type], 
						GetSlot(Type), 
						0,
						FAsyncSaveGameToSlotDelegate::CreateUObject(this, &USaveSystem::OnSaveCompleteHandle));
					return;
				}
				OnSaveCompleteHandle(GetSlot(Type), 0, false);
				return;
			}
	}
	OnSaveCompleteHandle(GetSlot(Type), 0, false);
}

void USaveSystem::LoadGame(const ESaveType Type, UObject* ObjectToLoad, const FString& Context)
{
	if (!ObjectToLoad || !ObjectToLoad->GetClass()->ImplementsInterface(USaveableInterface::StaticClass()) || !SaveGames[Type])
	{
		OnLoadCompleteHandle(GetSlot(Type), 0, false);
		return;
	}
	switch (Type)
	{
	case ESaveType::PlayerSave:
		{
			if (ISaveableInterface::Execute_LoadFromPlayer(ObjectToLoad, Cast<UPlayerSaveGame>(SaveGames[Type]), Context))
			{
				OnLoadCompleteHandle(GetSlot(Type), 0, true);
				return;
			}
			OnLoadCompleteHandle(GetSlot(Type), 0, false);
			return;
		}
	case ESaveType::RunSave:
		{
			if (ISaveableInterface::Execute_LoadFromRun(ObjectToLoad, Cast<URunSaveGame>(SaveGames[Type]), Context))
			{
				OnLoadCompleteHandle(GetSlot(Type), 0, true);
				return;
			}
			OnLoadCompleteHandle(GetSlot(Type), 0, false);
			return;
		}
	}
	OnLoadCompleteHandle(GetSlot(Type), 0, false);
}

void USaveSystem::OnSaveCompleteHandle(const FString& SlotName, const int32 UserIndex, const bool bSuccess)
{
	OnSaveComplete.Broadcast(bSuccess);
}

void USaveSystem::OnLoadCompleteHandle(const FString& SlotName, const int32 UserIndex, const bool bSuccess)
{
	OnLoadComplete.Broadcast(bSuccess);
}

TSubclassOf<USaveGame> USaveSystem::GetSaveClass(const ESaveType Type)
{
	switch (Type)
	{
	case ESaveType::PlayerSave:
		{
			return UPlayerSaveGame::StaticClass();
		}
	case ESaveType::RunSave:
		{
			return URunSaveGame::StaticClass();
		}
	}
	return USaveGame::StaticClass();
}

FString USaveSystem::GetSlot(const ESaveType Type)
{
	if (FString* FoundSlot = Slots.Find(Type))
	{
		return *FoundSlot;
	}
	UE_LOG(LogTemp, Error, TEXT("Slot not found for type %d!"), static_cast<uint8>(Type));
	return TEXT("InvalidSlot");
}
