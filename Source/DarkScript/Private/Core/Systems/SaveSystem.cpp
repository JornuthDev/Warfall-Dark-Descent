#include "Core/Systems/SaveSystem.h"

#include "Interfaces/SaveInterface.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/PlayerSave.h"
#include "Utils/Helpers/SystemsHelper.h"
#include "TimerManager.h"
#include "Utils/Log.h"

// ═══════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════

void USaveSystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
}

void USaveSystem::InitializeSaveSlots(APlayerController* Controller)
{
    UE_LOG(DarkScript, Log, TEXT("USaveSystem::InitializeSaveSlots - Initializing save slots..."));
    
    if (!Controller)
    {
        UE_LOG(DarkScript, Error, TEXT("USaveSystem::InitializeSaveSlots - Controller is not valid"));
        return;
    }
    
    PlayerController = Controller;
    
    // Récupération de l'ID utilisateur
    FOnlineHelper Helper = FOnlineHelper(this);
    Helper.QueryIDFromPlayerController(PlayerController.Get());
    
    UE_LOG(DarkScript, Log, TEXT("USaveSystem::InitializeSaveSlots - UserID: %s"), *Helper.UserID);
    
    // Initialisation des slots de sauvegarde
    static UEnum* EnumPtr = StaticEnum<ESaveType>();
    if (!EnumPtr)
    {
        UE_LOG(DarkScript, Error, TEXT("USaveSystem::InitializeSaveSlots - Failed to get ESaveType enum"));
        return;
    }
    
    const int32 Count = EnumPtr->NumEnums() - 1; // -1 pour exclure MAX
    
    for (int32 i = 0; i < Count; i++)
    {
        const int64 EnumValue = EnumPtr->GetValueByIndex(i);
        const ESaveType Type = static_cast<ESaveType>(EnumValue);
        
        // Vérification de la validité du type
        if (EnumValue < 0 || EnumValue >= static_cast<int64>(ESaveType::MAX))
        {
            continue;
        }
        
        FString EnumName = EnumPtr->GetNameStringByValue(EnumValue);
        EnumName.RemoveFromStart(TEXT("ESaveType::"));
        FString SlotName = FString::Printf(TEXT("%s_%s"), *EnumName, *Helper.UserID);
        
        SaveSlots.Add(Type, SlotName);
        Create(Type);
        
        UE_LOG(DarkScript, Log, TEXT("USaveSystem::InitializeSaveSlots - Created save slot: %s"), *SlotName);
    }
    UE_LOG(DarkScript, Log, TEXT("USaveSystem::InitializeSaveSlots - Initialization complete!"));
}

void USaveSystem::Deinitialize()
{
    Super::Deinitialize();
    
    // Nettoyage
    SaveGames.Empty();
    SaveSlots.Empty();
    UE_LOG(DarkScript, Log, TEXT("USaveSystem::Deinitialize - Cleanup complete"));
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Statics)
// ═══════════════════════════════════════════════════════════════════════

inline TSubclassOf<USaveGame> GetSaveClass(const ESaveType Type)
{
    switch (Type)
    {
        case ESaveType::PlayerSave:
            return UPlayerSave::StaticClass();
            
        default:
            UE_LOG(DarkScript, Warning, TEXT("GetSaveClass - Unknown save type %d"), (int32)Type);
            return USaveGame::StaticClass();
    }
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Internal)
// ═══════════════════════════════════════════════════════════════════════

void USaveSystem::Create(const ESaveType Type)
{
    const TSubclassOf<USaveGame> NewClass = GetSaveClass(Type);
    const FString SlotName = GetSaveSlot(Type);
    
    TObjectPtr<USaveGame> NewInstance = nullptr;
    
    // Tentative de chargement si le fichier existe
    if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
    {
        NewInstance = UGameplayStatics::LoadGameFromSlot(SlotName, 0);
        if (NewInstance)
        {
            UE_LOG(DarkScript, Log, TEXT("USaveSystem::Create - Loaded existing save: %s"), *SlotName);
        }
        else
        {
            UE_LOG(DarkScript, Warning, TEXT("USaveSystem::Create - Failed to load save, creating new: %s"), *SlotName);
        }
    }
    
    // Création d'une nouvelle instance si nécessaire
    if (!NewInstance)
    {
        NewInstance = UGameplayStatics::CreateSaveGameObject(NewClass);
        UE_LOG(DarkScript, Log, TEXT("USaveSystem::Create - Created new save: %s"), *SlotName);
    }
    
    if (NewInstance)
    {
        SaveGames.Add(Type, NewInstance);
    }
    else
    {
        UE_LOG(DarkScript, Error, TEXT("USaveSystem::Create - Failed to create save game for type %d"), (int32)Type);
    }
}

void USaveSystem::SaveGame(const ESaveType Type, UObject* ObjectToSave, const FString& Context)
{
    // Validation des paramètres
    if (!ObjectToSave)
    {
        UE_LOG(DarkScript, Error, TEXT("USaveSystem::SaveGame - ObjectToSave is null"));
        OnSaveComplete.Broadcast(false);
        return;
    }
    
    if (!ObjectToSave->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
    {
        UE_LOG(DarkScript, Error, TEXT("USaveSystem::SaveGame - Object does not implement ISaveInterface"));
        OnSaveComplete.Broadcast(false);
        return;
    }
    
    TObjectPtr<USaveGame>* SaveGamePtr = SaveGames.Find(Type);
    if (!SaveGamePtr || !(*SaveGamePtr))
    {
        UE_LOG(DarkScript, Error, TEXT("USaveSystem::SaveGame - SaveGame not found for type %d"), (int32)Type);
        OnSaveComplete.Broadcast(false);
        return;
    }
    
    // Appel de la sauvegarde via l'interface
    if (!ISaveInterface::Save(Type, ObjectToSave, Context))
    {
        UE_LOG(DarkScript, Error, TEXT("USaveSystem::SaveGame - ISaveInterface::Save returned false"));
        OnSaveComplete.Broadcast(false);
        return;
    }
    
    // Sauvegarde asynchrone sur disque
    UGameplayStatics::AsyncSaveGameToSlot(
        *SaveGamePtr,
        GetSaveSlot(Type),
        0,
        FAsyncSaveGameToSlotDelegate::CreateLambda([this, Type](const FString& SlotName, [[maybe_unused]] const int32 UserIndex, bool bSuccess)
        {
            if (bSuccess)
            {
                UE_LOG(DarkScript, Log, TEXT("USaveSystem::SaveGame - Successfully saved to slot: %s"), *SlotName);
            }
            else
            {
                UE_LOG(DarkScript, Error, TEXT("USaveSystem::SaveGame - Failed to save to slot: %s"), *SlotName);
            }
            OnSaveComplete.Broadcast(bSuccess);
        }));
}

void USaveSystem::LoadGame(const ESaveType Type, UObject* ObjectToLoad, const FString& Context)
{
    // Validation des paramètres
    if (!ObjectToLoad)
    {
        UE_LOG(DarkScript, Error, TEXT("USaveSystem::LoadGame - ObjectToLoad is null"));
        OnLoadComplete.Broadcast(false);
        return;
    }
    
    if (!ObjectToLoad->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
    {
        UE_LOG(DarkScript, Error, TEXT("USaveSystem::LoadGame - Object does not implement ISaveInterface"));
        OnLoadComplete.Broadcast(false);
        return;
    }
    
    TObjectPtr<USaveGame>* SaveGamePtr = SaveGames.Find(Type);
    if (!SaveGamePtr || !(*SaveGamePtr))
    {
        UE_LOG(DarkScript, Error, TEXT("USaveSystem::LoadGame - SaveGame not found for type %d"), (int32)Type);
        OnLoadComplete.Broadcast(false);
        return;
    }
    
    // Chargement via l'interface
    const bool bSuccess = ISaveInterface::Load(Type, ObjectToLoad, Context);
    
    if (bSuccess)
    {
        UE_LOG(DarkScript, Log, TEXT("USaveSystem::LoadGame - Successfully loaded for type %d"), (int32)Type);
    }
    else
    {
        UE_LOG(DarkScript, Error, TEXT("USaveSystem::LoadGame - Failed to load for type %d"), (int32)Type);
    }
    
    OnLoadComplete.Broadcast(bSuccess);
}

FString USaveSystem::GetSaveSlot(const ESaveType Type) const
{
    if (const FString* FoundSlot = SaveSlots.Find(Type))
    {
        return *FoundSlot;
    }
    
    UE_LOG(DarkScript, Warning, TEXT("USaveSystem::GetSaveSlot - No slot found for type %d, system may not be initialized"), (int32)Type);
    return TEXT("InvalidSlot");
}

bool USaveSystem::DoesSaveExist(const ESaveType Type) const
{
    return UGameplayStatics::DoesSaveGameExist(GetSaveSlot(Type), 0);
}

// ═══════════════════════════════════════════════════════════════════════
// PUBLIC API
// ═══════════════════════════════════════════════════════════════════════

void USaveSystem::RequestSave(const ESaveType Type, UObject* ObjectToSave, const FString& Context)
{
    SaveGame(Type, ObjectToSave, Context);
}

void USaveSystem::RequestLoad(const ESaveType Type, UObject* ObjectToLoad, const FString& Context)
{
    LoadGame(Type, ObjectToLoad, Context);
}

USaveGame* USaveSystem::GetSaveGameObject(const ESaveType Type) const
{
    if (const TObjectPtr<USaveGame>* SaveGamePtr = SaveGames.Find(Type))
    {
        return *SaveGamePtr;
    }
    return nullptr;
}