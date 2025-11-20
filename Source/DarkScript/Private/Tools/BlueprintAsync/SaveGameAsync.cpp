#include "Tools/BlueprintAsync/SaveGameAsync.h"

#include "Core/SubSystems/SaveSystem.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
USaveGameAsync* USaveGameAsync::SavePlayerProfile(UObject* ObjectToSave, const FString& Context)
{
	USaveGameAsync* Node = NewObject<USaveGameAsync>();
	Node->Object = ObjectToSave;
	Node->Context = Context;
	Node->bIsPlayerSaveGame.Key = true;
	Node->bIsPlayerSaveGame.Value = true;
	return Node;
}

USaveGameAsync* USaveGameAsync::SavePlayerProfileFrom(UObject* From, const FString& Context)
{
	USaveGameAsync* Node = NewObject<USaveGameAsync>();
	Node->Object = From;
	Node->Context = Context;
	Node->bIsPlayerSaveGame.Key = true;
	Node->bIsPlayerSaveGame.Value = true;
	return Node;
}

USaveGameAsync* USaveGameAsync::SaveRunProgress(UObject* ObjectToSave, const FString& Context)
{
	USaveGameAsync* Node = NewObject<USaveGameAsync>();
	Node->Object = ObjectToSave;
	Node->Context = Context;
	Node->bIsPlayerSaveGame.Key = false;
	Node->bIsPlayerSaveGame.Value = true;
	return Node;
}

USaveGameAsync* USaveGameAsync::SaveRunProgressFrom(UObject* From, const FString& Context)
{
	USaveGameAsync* Node = NewObject<USaveGameAsync>();
	Node->Object = From;
	Node->Context = Context;
	Node->bIsPlayerSaveGame.Key = false;
	Node->bIsPlayerSaveGame.Value = true;
	return Node;
}

USaveGameAsync* USaveGameAsync::LoadPlayerProfile(UObject* ObjectToLoad, const FString& Context)
{
	USaveGameAsync* Node = NewObject<USaveGameAsync>();
	Node->Object = ObjectToLoad;
	Node->Context = Context;
	Node->bIsPlayerSaveGame.Key = true;
	Node->bIsPlayerSaveGame.Value = false;
	return Node;
}

USaveGameAsync* USaveGameAsync::LoadPlayerProfileFrom(UObject* From, const FString& Context)
{
	USaveGameAsync* Node = NewObject<USaveGameAsync>();
	Node->Object = From;
	Node->Context = Context;
	Node->bIsPlayerSaveGame.Key = true;
	Node->bIsPlayerSaveGame.Value = false;
	return Node;
}

USaveGameAsync* USaveGameAsync::LoadRunProgress(UObject* ObjectToLoad, const FString& Context)
{
	USaveGameAsync* Node = NewObject<USaveGameAsync>();
	Node->Object = ObjectToLoad;
	Node->Context = Context;
	Node->bIsPlayerSaveGame.Key = false;
	Node->bIsPlayerSaveGame.Value = false;
	return Node;
}

USaveGameAsync* USaveGameAsync::LoadRunProgressFrom(UObject* From, const FString& Context)
{
	USaveGameAsync* Node = NewObject<USaveGameAsync>();
	Node->Object = From;
	Node->Context = Context;
	Node->bIsPlayerSaveGame.Key = false;
	Node->bIsPlayerSaveGame.Value = false;
	return Node;
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
void USaveGameAsync::Activate()
{
	Super::Activate();
	
	const UWorld* World = GEngine->GetWorldFromContextObject(Object.Get(), EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		OnCompleted(false);
		return;
	}
	USaveSystem* System = World->GetGameInstance()->GetSubsystem<USaveSystem>();
	if (!System)
	{
		OnCompleted(false);
		return;
	}
	if (bIsPlayerSaveGame.Key)
	{
		if (bIsPlayerSaveGame.Value)
		{
			System->OnSaveComplete.AddUObject(this, &USaveGameAsync::OnCompleted);
			System->SavePlayerProfile(Object.Get(), Context);
			return;
		}
			System->OnLoadComplete.AddUObject(this, &USaveGameAsync::OnCompleted);
			System->LoadPlayerProfile(Object.Get(), Context);
			return;
	}
	if (bIsPlayerSaveGame.Value)
	{
		System->OnSaveComplete.AddUObject(this, &USaveGameAsync::OnCompleted);
		System->SaveRunProgress(Object.Get(), Context);
		return;
	}
	System->OnLoadComplete.AddUObject(this, &USaveGameAsync::OnCompleted);
	System->LoadRunProgress(Object.Get(), Context);
}

void USaveGameAsync::OnCompleted(const bool bSuccess)
{
	if (bSuccess)
	{
		OnSuccess.Broadcast();
		OnFailure.Clear();
		SetReadyToDestroy();
		return;
	}
	OnFailure.Broadcast();
	OnSuccess.Clear();
	SetReadyToDestroy();
}
