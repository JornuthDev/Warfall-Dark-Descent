#include "Interfaces/SaveInterface.h"

#include "Core/Systems/SaveSystem.h"
#include "SaveGame/PlayerSave.h"
#include "Utils/Log.h"
#include "Utils/Helpers/SystemsHelper.h"

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Blueprint API)
// ═══════════════════════════════════════════════════════════════════════

bool ISaveInterface::SaveToPlayer_Implementation(UPlayerSave* SaveGame, const FString& Context)
{
    return false;
}

bool ISaveInterface::LoadFromPlayer_Implementation(UPlayerSave* SaveGame, const FString& Context)
{
    return false;
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Statics)
// ═══════════════════════════════════════════════════════════════════════

bool ISaveInterface::Save(const ESaveType Type, UObject* ObjectToSave, const FString& Context)
{
    if (!SaveSystem::Get(ObjectToSave) || !ObjectToSave)
    {
        UE_LOG(DarkScript, Error, TEXT("ISaveInterface::Save - Invalid SaveSystem or ObjectToSave"));
        return false;
    }
    
    USaveGame* SaveGameObject = SaveSystem::SaveGame(Type, ObjectToSave);
    if (!SaveGameObject)
    {
        UE_LOG(DarkScript, Error, TEXT("ISaveInterface::Save - SaveGame object not found for type %d"), (int32)Type);
        return false;
    }
    
    switch(Type)
    {
        case ESaveType::PlayerSave:
        {
            UPlayerSave* PlayerSaveGame = Cast<UPlayerSave>(SaveGameObject);
            if (!PlayerSaveGame)
            {
                UE_LOG(DarkScript, Error, TEXT("ISaveInterface::Save - Failed to cast to UPlayerSave"));
                return false;
            }
            return Execute_SaveToPlayer(ObjectToSave, PlayerSaveGame, Context);
        }
        
        default:
            UE_LOG(DarkScript, Warning, TEXT("ISaveInterface::Save - Unhandled save type %d"), (int32)Type);
            return false;
    }
}

bool ISaveInterface::Load(ESaveType Type, UObject* ObjectToLoad, const FString& Context)
{
    if (!SaveSystem::Get(ObjectToLoad) || !ObjectToLoad)
    {
        UE_LOG(DarkScript, Error, TEXT("ISaveInterface::Load - Invalid SaveSystem or ObjectToLoad"));
        return false;
    }
    
    USaveGame* SaveGameObject = SaveSystem::SaveGame(Type, ObjectToLoad);
    if (!SaveGameObject)
    {
        UE_LOG(DarkScript, Error, TEXT("ISaveInterface::Load - SaveGame object not found for type %d"), (int32)Type);
        return false;
    }
    
    switch(Type)
    {
        case ESaveType::PlayerSave:
        {
            UPlayerSave* PlayerSaveGame = Cast<UPlayerSave>(SaveGameObject);
            if (!PlayerSaveGame)
            {
                UE_LOG(DarkScript, Error, TEXT("ISaveInterface::Load - Failed to cast to UPlayerSave"));
                return false;
            }
            return Execute_LoadFromPlayer(ObjectToLoad, PlayerSaveGame, Context);
        }
        
        default:
            UE_LOG(DarkScript, Warning, TEXT("ISaveInterface::Load - Unhandled load type %d"), (int32)Type);
            return false;
    }
}