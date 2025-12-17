#pragma once
#include "CoreMinimal.h"
#include "DarkScript.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Core/Systems/SaveSystem.h"
#include "Utils/Log.h"
#include "Utils/Types/OnlineTypes.h"

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

// ═══════════════════════════════════════════════════════════════════════
// CONTENT HELPERS
// ═══════════════════════════════════════════════════════════════════════

namespace LevelPaths
{
	static const FString Menu = TEXT("/DarkScript/Levels/Menu/Menu");
	static const FString Lobby = TEXT("/DarkScript/Levels/Lobby/Lobby");
	static const FString Dungeon = TEXT("");
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

inline const FSlateBrush* GetIcon(const FName& IconName)
{
    const FSlateBrush* Brush = FDarkScriptModule::GetDarkSlate(*FString::Printf(TEXT("DarkSlate.%s"), *IconName.ToString()));
    if (Brush && Brush != FStyleDefaults::GetNoBrush())
    {
        return Brush;
    }
    return FAppStyle::GetBrush("Icons.Help");
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

inline FText GetSessionJoinErrorText(const ESessionJoinError Error)
{
	switch (Error)
	{
	case ESessionJoinError::None:
		return FText::FromString(TEXT("No error"));
	case ESessionJoinError::NetworkError:
		return FText::FromString(TEXT("Network error occurred"));
	case ESessionJoinError::SearchFailed:
		return FText::FromString(TEXT("Failed to search for sessions"));
	case ESessionJoinError::NoSessionsFound:
		return FText::FromString(TEXT("No sessions found"));
	case ESessionJoinError::InvalidSessionCode:
		return FText::FromString(TEXT("Invalid session code"));
	case ESessionJoinError::SessionNotFound:
		return FText::FromString(TEXT("Session not found"));
	case ESessionJoinError::SessionFull:
		return FText::FromString(TEXT("Session is full"));
	case ESessionJoinError::ArchetypeNotAvailable:
		return FText::FromString(TEXT("Your selected archetype is not available"));
	case ESessionJoinError::ArchetypeNotAllowed:
		return FText::FromString(TEXT("Your archetype is not allowed in this session"));
	case ESessionJoinError::PlayerLevelTooLow:
		return FText::FromString(TEXT("Your level is too low for this session"));
	case ESessionJoinError::SessionAlreadyStarted:
		return FText::FromString(TEXT("Session has already started"));
	case ESessionJoinError::SessionInProgress:
		return FText::FromString(TEXT("Players are already in the dungeon"));
	case ESessionJoinError::JoinFailed:
		return FText::FromString(TEXT("Failed to join session"));
	case ESessionJoinError::ConnectionFailed:
		return FText::FromString(TEXT("Connection to session failed"));
	case ESessionJoinError::Unknown:
	default:
		return FText::FromString(TEXT("Unknown error occurred"));
	}
}

inline ESessionType IntToSessionType(const int32 SessionType)
{
	return static_cast<ESessionType>(SessionType);	
}

inline FString ArchetypesToString(const TArray<EClassArchetype>& Archetypes)
{
	FString Result;
	for (int32 i = 0; i < Archetypes.Num(); ++i)
	{
		Result += FString::FromInt(static_cast<int32>(Archetypes[i]));
		if (i < Archetypes.Num() - 1)
		{
			Result += TEXT(",");
		}
	}
	return Result;
}

inline TArray<EClassArchetype> StringToArchetypes(const FString& ArchetypesString)
{
	TArray<EClassArchetype> Result;
	TArray<FString> StringArray;
	ArchetypesString.ParseIntoArray(StringArray, TEXT(","), true);
	
	static UEnum* EnumPtr = StaticEnum<EClassArchetype>();
	const int32 Count = EnumPtr->NumEnums();
	
	for (const FString& String : StringArray)
	{
		if (const int32 ArchetypeValue = FCString::Atoi(*String); ArchetypeValue >= 0 && ArchetypeValue <= Count -1)
		{
			Result.Add(static_cast<EClassArchetype>(ArchetypeValue));
		}
	}
	return Result;
}

inline TArray<EClassArchetype> GetAllArchetypes()
{
	TArray<EClassArchetype> Result;
	
	static UEnum* EnumPtr = StaticEnum<EClassArchetype>();
	const int32 Count = EnumPtr->NumEnums();
	for (int32 i = 1; i < Count -1; i++)
	{
		Result.Add(static_cast<EClassArchetype>(i));
	}
	return Result;
}