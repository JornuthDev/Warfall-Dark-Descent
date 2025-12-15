#pragma once
#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Core/Systems/SaveSystem.h"
#include "Utils/Log.h"

inline bool IsEditorMode()
{
    return GIsEditor;
}

// ═══════════════════════════════════════════════════════════════════════
// SAVE SYSTEM HELPERS
// ═══════════════════════════════════════════════════════════════════════

namespace SaveSystem
{
    /**
     * Récupère l'instance du SaveSystem depuis le GameInstance
     * @param WorldContext Optionnel - contexte world pour récupérer le GameInstance
     * @return Pointeur vers USaveSystem ou nullptr si non disponible
     */
    inline USaveSystem* Get(const UObject* WorldContext = nullptr)
    {
        const UWorld* World;
        
        // Si un contexte est fourni, l'utiliser en priorité
        if (WorldContext)
        {
            World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
        }
        else
        {
            // Sinon, tenter de récupérer le premier monde valide
            // ⚠️ ATTENTION: GEngine->GetWorld() peut retourner le mauvais monde en multi-PIE
            World = GEngine->GetWorld();
        }
        
        if (!World)
        {
            UE_LOG(DarkScript, Warning, TEXT("SaveSystem::Get - No valid World found"));
            return nullptr;
        }
        
        UGameInstance* GameInstance = World->GetGameInstance();
        if (!GameInstance)
        {
            UE_LOG(DarkScript, Warning, TEXT("SaveSystem::Get - No GameInstance found"));
            return nullptr;
        }
        
        return GameInstance->GetSubsystem<USaveSystem>();
    }
    
    /**
     * Récupère un objet SaveGame par son type
     * @param Type Le type de sauvegarde
     * @param WorldContext Optionnel - contexte pour récupérer le SaveSystem
     * @return Pointeur vers USaveGame ou nullptr si non trouvé
     */
    inline USaveGame* SaveGame(const ESaveType Type, const UObject* WorldContext = nullptr)
    {
        USaveSystem* System = Get(WorldContext);
        if (!System)
        {
            UE_LOG(DarkScript, Warning, TEXT("SaveSystem::SaveGame - SaveSystem not available"));
            return nullptr;
        }
        
        // Utiliser le getter public au lieu d'accéder directement au membre
        return System->GetSaveGameObject(Type);
    }
}

namespace Content
{
    inline UDataTable* GetTable(const FString& Path)
    {
        const FString* PathPtr = &Path;
        if (!PathPtr) return nullptr;
        return Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, **PathPtr));
    }
    inline UMaterialInterface* GetMaterial(const FString& Path)
    {
        const FString* PathPtr = &Path;
        if (!PathPtr) return nullptr;
        return Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, **PathPtr));
    }
}

namespace Tables
{
    static const FString Inputs = TEXT("/Script/Engine.DataTable'/DarkScript/Data/Tables/InputsTable.InputsTable'");
    static const FString Attributes = TEXT("/Script/Engine.DataTable'/DarkScript/Data/Tables/AttributesTable.AttributesTable'");
    static const FString ArchetypeData = TEXT("/Script/Engine.DataTable'/DarkScript/Data/Tables/ArchetypesTable.ArchetypesTable'");
}

// ═══════════════════════════════════════════════════════════════════════
// ONLINE SYSTEM
// ═══════════════════════════════════════════════════════════════════════

struct FOnlineHelper
{
    /**
     * Constructeur
     * @param WorldContextObject Objet avec contexte world
     * @param SystemName Nom du subsystem online (NAME_None pour le défaut)
     */
    explicit FOnlineHelper(const UObject* WorldContextObject, const FName SystemName = NAME_None)
        : UserID(TEXT("Default_UserID"))
        , OnlineSub(nullptr)
        , WorldContext(nullptr)
    {
        if (!WorldContextObject)
        {
            UE_LOG(DarkScript, Warning, TEXT("FOnlineHelper - WorldContextObject is null"));
            return;
        }
        
        WorldContext = GEngine->GetWorldFromContextObject(
            WorldContextObject, 
            EGetWorldErrorMode::LogAndReturnNull
        );
        
        if (!WorldContext)
        {
            UE_LOG(DarkScript, Warning, TEXT("FOnlineHelper - Failed to get World from context"));
            return;
        }
        
        OnlineSub = Online::GetSubsystem(WorldContext, SystemName);
        if (!OnlineSub)
        {
            UE_LOG(DarkScript, Log, TEXT("FOnlineHelper - OnlineSubsystem not available (may be normal in PIE)"));
        }
    }
    
    /**
     * Récupère l'ID utilisateur depuis le PlayerController
     * Format:
     * - En PIE: "PIE_Instance_X"
     * - Avec OSS: ID unique de la plateforme
     * - Fallback: "Default_UserID"
     * 
     * @param PlayerController Le contrôleur du joueur
     */
    void QueryIDFromPlayerController(const APlayerController* PlayerController)
    {
        // Réinitialiser à la valeur par défaut
        UserID = TEXT("Default_UserID");
        
        // ═══════════════════════════════════════════════════════════════
        // CAS 1: Mode éditeur (PIE) - Utiliser l'instance PIE
        // ═══════════════════════════════════════════════════════════════
        if (IsEditorMode())
        {
            if (!WorldContext)
            {
                UE_LOG(DarkScript, Warning, TEXT("FOnlineHelper::QueryID - WorldContext is null in PIE"));
                return;
            }
            
            // Rechercher le contexte PIE correspondant
            for (const FWorldContext& Context : GEngine->GetWorldContexts())
            {
                if (Context.World() == WorldContext)
                {
                    UserID = FString::Printf(TEXT("PIE_Instance_%d"), Context.PIEInstance);
                    UE_LOG(DarkScript, Log, TEXT("FOnlineHelper::QueryID - PIE UserID: %s"), *UserID);
                    return;
                }
            }
            
            UE_LOG(DarkScript, Warning, TEXT("FOnlineHelper::QueryID - Failed to find PIE context"));
            return;
        }
        
        // ═══════════════════════════════════════════════════════════════
        // CAS 2: Build packagé - Utiliser l'Online Subsystem
        // ═══════════════════════════════════════════════════════════════
        if (!PlayerController)
        {
            UE_LOG(DarkScript, Warning, TEXT("FOnlineHelper::QueryID - PlayerController is null"));
            return;
        }
        
        if (!OnlineSub)
        {
            UE_LOG(DarkScript, Warning, TEXT("FOnlineHelper::QueryID - OnlineSubsystem is null"));
            return;
        }
        
        IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();
        if (!Identity.IsValid())
        {
            UE_LOG(DarkScript, Warning, TEXT("FOnlineHelper::QueryID - Identity interface not valid"));
            return;
        }
        
        // Récupérer le LocalPlayer
        ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
        if (!LocalPlayer)
        {
            UE_LOG(DarkScript, Warning, TEXT("FOnlineHelper::QueryID - LocalPlayer is null"));
            return;
        }
        
        // Récupérer l'ID unique
        TSharedPtr<const FUniqueNetId> UniqueID = LocalPlayer->GetPreferredUniqueNetId().GetUniqueNetId();
        if (!UniqueID.IsValid())
        {
            UE_LOG(DarkScript, Warning, TEXT("FOnlineHelper::QueryID - UniqueNetId is not valid"));
            return;
        }
        
        UserID = UniqueID->ToString();
        UE_LOG(DarkScript, Log, TEXT("FOnlineHelper::QueryID - Online UserID: %s"), *UserID);
    }
    
    /**
     * Vérifie si l'helper est correctement initialisé
     */
    bool IsValid() const
    {
        return WorldContext != nullptr;
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // VARIABLES PUBLIQUES
    // ═══════════════════════════════════════════════════════════════════════
    
    /** ID utilisateur récupéré */
    FString UserID;
    
    /** Pointeur vers le subsystem online */
    IOnlineSubsystem* OnlineSub;
    
private:
    /** Contexte world */
    UWorld* WorldContext;
};