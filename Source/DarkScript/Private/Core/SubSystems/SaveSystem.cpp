#include "Core/SubSystems/SaveSystem.h"
#include "Core/SaveGame/PlayerProfile.h"
#include "Core/SaveGame/RunProgress.h"
#include "Interfaces/SaveableInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Tools/DarkHelpers.h"

void USaveSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	SteamID = GetSteamID();
	CreateSaveGames();
}

void USaveSystem::Deinitialize()
{
	Super::Deinitialize();
}

FString USaveSystem::GetSteamID() const
{
	if (LaunchMode::IsDevelopMode)
	{
		const UWorld* World = GetWorld();
		for (const FWorldContext& Context : GEngine->GetWorldContexts())
		{
			if (Context.World() == World)
			{
				return FString::Printf(TEXT("PIEInstance_%d"), Context.PIEInstance);
			}
		}
	}
	return FString(TEXT("DefaultPlayer"));
}

FString USaveSystem::GetPlayerSlotName() const
{
	return FString::Printf(TEXT("PlayerProfile_%s"), *GetSteamID());
}

FString USaveSystem::GetRunProgressSlotName() const
{
	return FString::Printf(TEXT("RunProgress_%s"), *GetSteamID());
}

void USaveSystem::CreateSaveGames()
{
	if (!PlayerProfile)
	{
		if (UGameplayStatics::DoesSaveGameExist(GetPlayerSlotName(), 0))
		{
			PlayerProfile = Cast<UPlayerProfile>(UGameplayStatics::LoadGameFromSlot(GetPlayerSlotName(), 0));
		}
		else
		{
			PlayerProfile = Cast<UPlayerProfile>(UGameplayStatics::CreateSaveGameObject(UPlayerProfile::StaticClass()));
		}
	}
	if (!RunProgress)
	{
		if (UGameplayStatics::DoesSaveGameExist(GetRunProgressSlotName(), 0))
		{
			RunProgress = Cast<URunProgress>(UGameplayStatics::LoadGameFromSlot(GetRunProgressSlotName(), 0));
		}
		else
		{
			RunProgress = Cast<URunProgress>(UGameplayStatics::CreateSaveGameObject(URunProgress::StaticClass()));
		}
	}
}

void USaveSystem::SavePlayerProfile(UObject* ObjectToSave, const FString& Context)
{

	if (!ObjectToSave || !ObjectToSave->GetClass()->ImplementsInterface(USaveableInterface::StaticClass()) || !PlayerProfile)
	{
		OnSaveCompleteHandle(GetPlayerSlotName(), 0, false);
		return;
	}
	if (ISaveableInterface::Execute_SaveToPlayerProfile(ObjectToSave, PlayerProfile, Context))
	{
		const FString SlotName = GetPlayerSlotName();
		UGameplayStatics::AsyncSaveGameToSlot(
			PlayerProfile,
			GetPlayerSlotName(),
			0,
			FAsyncSaveGameToSlotDelegate::CreateUObject(this, &USaveSystem::OnSaveCompleteHandle));
		return;
	}
	OnSaveCompleteHandle(GetPlayerSlotName(), 0, false);
}

void USaveSystem::LoadPlayerProfile(UObject* ObjectToLoad, const FString& Context)
{
	
	if (!ObjectToLoad || !ObjectToLoad->GetClass()->ImplementsInterface(USaveableInterface::StaticClass()) || !PlayerProfile)
	{
		OnLoadCompleteHandle(GetPlayerSlotName(), 0, false);
		return;
	}
	if (ISaveableInterface::Execute_LoadToPlayerProfile(ObjectToLoad, PlayerProfile, Context))
	{
		OnLoadCompleteHandle(GetPlayerSlotName(), 0, true);
	}
	else
	{
		OnLoadCompleteHandle(GetPlayerSlotName(), 0, false);
	}
}

void USaveSystem::SaveRunProgress(UObject* ObjectToSave, const FString& Context)
{
	if (!ObjectToSave || !ObjectToSave->GetClass()->ImplementsInterface(USaveableInterface::StaticClass()) || !RunProgress)
	{
		OnSaveCompleteHandle(GetRunProgressSlotName(), 0, false);
		return;
	}
	if (ISaveableInterface::Execute_SaveToRunProgress(ObjectToSave, RunProgress, Context))
	{
		UGameplayStatics::AsyncSaveGameToSlot(
			RunProgress,
			GetRunProgressSlotName(),
			0,
			FAsyncSaveGameToSlotDelegate::CreateUObject(this, &USaveSystem::OnSaveCompleteHandle));
		return;
	}
	OnSaveCompleteHandle(GetRunProgressSlotName(), 0, false);
}

void USaveSystem::LoadRunProgress(UObject* ObjectToLoad, const FString& Context)
{
	if (!ObjectToLoad || !ObjectToLoad->GetClass()->ImplementsInterface(USaveableInterface::StaticClass()) || !RunProgress)
	{
		OnLoadCompleteHandle(GetRunProgressSlotName(), 0, false);
		return;
	}
	if (ISaveableInterface::Execute_LoadToRunProgress(ObjectToLoad, RunProgress, Context))
	{
		OnLoadCompleteHandle(GetRunProgressSlotName(), 0, true);
	}
	else
	{
		OnLoadCompleteHandle(GetRunProgressSlotName(), 0, false);
	}
}

void USaveSystem::OnSaveCompleteHandle(const FString& SlotName, const int32 UserIndex, const bool bSuccess)
{
	OnSaveComplete.Broadcast(bSuccess);
}

void USaveSystem::OnLoadCompleteHandle(const FString& SlotName, const int32 UserIndex, const bool bSuccess)
{
	OnLoadComplete.Broadcast(bSuccess);
}