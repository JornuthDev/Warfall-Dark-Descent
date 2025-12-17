#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveSystem.generated.h"

class USaveGame;

UENUM(BlueprintType)
enum class ESaveType : uint8
{
    PlayerSave,
    // Ajoutez d'autres types ici
    
    MAX UMETA(Hidden) // Marqueur de fin pour l'itération
};

/**
 * Système de sauvegarde centralisé pour gérer les différents types de saves
 * Utilise un pattern de subsystem pour être accessible globalement
 */
UCLASS()
class DARKSCRIPT_API USaveSystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
    // ═══════════════════════════════════════════════════════════════════════
    // DELEGATES
    // ═══════════════════════════════════════════════════════════════════════
    
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnSaveComplete, bool);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoadComplete, bool);

public:
    // ═══════════════════════════════════════════════════════════════════════
    // LIFECYCLE
    // ═══════════════════════════════════════════════════════════════════════
    
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    
    /** Initialisation complète du système (appelée quand le LocalPlayer est prêt) */
    void InitializeSaveSlots(APlayerController* Controller);
    
    // ═══════════════════════════════════════════════════════════════════════
    // PUBLIC API
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * Demande une sauvegarde asynchrone
     * @param Type Le type de sauvegarde
     * @param ObjectToSave L'objet à sauvegarder (doit implémenter ISaveInterface)
     * @param Context Contexte optionnel pour la sauvegarde
     */
    UFUNCTION(BlueprintCallable, Category = "SaveSystem")
    void RequestSave(const ESaveType Type, UObject* ObjectToSave, const FString& Context = TEXT(""));
    
    /**
     * Demande un chargement
     * @param Type Le type de sauvegarde
     * @param ObjectToLoad L'objet dans lequel charger (doit implémenter ISaveInterface)
     * @param Context Contexte optionnel pour le chargement
     */
    UFUNCTION(BlueprintCallable, Category = "SaveSystem")
    void RequestLoad(const ESaveType Type, UObject* ObjectToLoad, const FString& Context = TEXT(""));
    
    /**
     * Récupère l'objet SaveGame pour un type donné
     * @param Type Le type de sauvegarde
     * @return L'objet USaveGame ou nullptr si non trouvé
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SaveSystem")
    USaveGame* GetSaveGameObject(const ESaveType Type) const;
    
    /**
     * Vérifie si un slot de sauvegarde existe
     * @param Type Le type de sauvegarde
     * @return true si le fichier de sauvegarde existe
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SaveSystem")
    bool DoesSaveExist(const ESaveType Type) const;
        
    // ═══════════════════════════════════════════════════════════════════════
    // DELEGATES (Public pour les bindings)
    // ═══════════════════════════════════════════════════════════════════════
    
    /** Appelé quand une sauvegarde asynchrone est terminée (succès ou échec) */
    FOnSaveComplete OnSaveComplete;
    
    /** Appelé quand un chargement est terminé (succès ou échec) */
    FOnLoadComplete OnLoadComplete;
    
private:
    // ═══════════════════════════════════════════════════════════════════════
    // INTERNAL FUNCTIONS
    // ═══════════════════════════════════════════════════════════════════════
        
    /** Crée ou charge un objet de sauvegarde pour le type spécifié */
    void Create(ESaveType Type);
    
    /** Implémentation interne de la sauvegarde */
    void SaveGame(const ESaveType Type, UObject* ObjectToSave, const FString& Context);
    
    /** Implémentation interne du chargement */
    void LoadGame(const ESaveType Type, UObject* ObjectToLoad, const FString& Context);
    
    /** Récupère le nom du slot de sauvegarde pour un type */
    FString GetSaveSlot(const ESaveType Type) const;
    
    // ═══════════════════════════════════════════════════════════════════════
    // VARIABLES (Private pour encapsulation)
    // ═══════════════════════════════════════════════════════════════════════
    
    /** Controller du joueur local */
    UPROPERTY()
    TObjectPtr<APlayerController> PlayerController;
    
    /** Map des objets de sauvegarde indexés par type */
    UPROPERTY()
    TMap<ESaveType, TObjectPtr<USaveGame>> SaveGames;
    
    /** Map des noms de slots indexés par type */
    TMap<ESaveType, FString> SaveSlots;
        
    // ═══════════════════════════════════════════════════════════════════════
    // FRIEND CLASSES (Pour l'accès depuis les helpers)
    // ═══════════════════════════════════════════════════════════════════════
    
    friend struct FOnlineHelper;
};