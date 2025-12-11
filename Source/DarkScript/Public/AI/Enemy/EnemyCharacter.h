// EnemyCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "Core/CharacterBase.h"
#include "AI/Enemy/EnemyData.h"
#include "AbilitySystemInterface.h"
#include "EnemyCharacter.generated.h"

class UAbilitySystemComponent;
class UDarkAttributeSet;

/**
 * Base class for all standard enemies in Dark Descent
 * - Uses GAS (Gameplay Ability System) for stats and abilities
 * - Configured via DataTable (DT_StandardEnemies)
 * - Supports floor-based scaling
 * - Resource management (Mana, Stamina, Rage, etc.)
 */
UCLASS()
class DARKSCRIPT_API AEnemyCharacter : public ACharacterBase, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    // ═══════════════════════════════════════════════════════════════
    // CONSTRUCTION
    // ═══════════════════════════════════════════════════════════════
    
    AEnemyCharacter();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void PossessedBy(AController* NewController) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // ═══════════════════════════════════════════════════════════════
    // GAMEPLAY ABILITY SYSTEM
    // ═══════════════════════════════════════════════════════════════
    
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    
    UFUNCTION(BlueprintPure, Category = "Enemy|GAS")
    UDarkAttributeSet* GetAttributeSet() const { return AttributeSet; }

    // ═══════════════════════════════════════════════════════════════
    // INITIALIZATION
    // ═══════════════════════════════════════════════════════════════
    
    /** Initialize l'ennemi depuis une row de la DataTable */
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void InitializeFromDataTable(FName RowName, int32 FloorLevel = 1);

    /** Applique les stats de base (sans scaling) */
    void ApplyBaseStats(const FStandardEnemyRow* EnemyData) const;

    /** Applique le scaling selon le palier */
    void ApplyFloorScaling(const FStandardEnemyRow* EnemyData, int32 FloorLevel) const;

    /** Grant toutes les abilities depuis AbilityRows */
    void GrantAbilities(const FStandardEnemyRow* EnemyData);

    /** Initialise le système de ressources */
    void InitializeResource(const FStandardEnemyRow* EnemyData);

    // ═══════════════════════════════════════════════════════════════
    // RESOURCE MANAGEMENT
    // ═══════════════════════════════════════════════════════════════
    
    UFUNCTION(BlueprintPure, Category = "Enemy|Resource")
    EResourceType GetResourceType() const { return ResourceType; }

    UFUNCTION(BlueprintPure, Category = "Enemy|Resource")
    float GetCurrentResource() const;

    UFUNCTION(BlueprintPure, Category = "Enemy|Resource")
    float GetMaxResource() const;

    UFUNCTION(BlueprintPure, Category = "Enemy|Resource")
    float GetResourcePercent() const;

    UFUNCTION(BlueprintCallable, Category = "Enemy|Resource")
    bool ConsumeResource(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Enemy|Resource")
    void AddResource(float Amount);

    void HandleResourceRegeneration(float DeltaTime);

    UFUNCTION(BlueprintPure, Category = "Enemy|Resource")
    bool HasEnoughResource(float Cost) const;

    // ═══════════════════════════════════════════════════════════════
    // COMBAT STATE
    // ═══════════════════════════════════════════════════════════════
    
    /** Set combat state (called by AI controller) */
    UFUNCTION(BlueprintCallable, Category = "Enemy|Combat")
    void SetInCombat(bool bInCombat) { bIsInCombat = bInCombat; }

    UFUNCTION(BlueprintPure, Category = "Enemy|Combat")
    bool IsInCombat() const { return bIsInCombat; }

    // ═══════════════════════════════════════════════════════════════
    // DATA ACCESS (for AI Controller)
    // ═══════════════════════════════════════════════════════════════
    
    /** Get enemy data row (for AI controller perception setup) */
    UFUNCTION(BlueprintPure, Category = "Enemy|Data", meta=(DisplayName="GetEnemyData"))
    const FStandardEnemyRow& GetEnemyData_BP() const { return *GetEnemyData(); }
    const FStandardEnemyRow* GetEnemyData() const { return CachedEnemyData; }
    
    UFUNCTION(BlueprintPure, Category = "Enemy|Data")
    FVector GetSpawnLocation() const { return SpawnLocation; }
    // ═══════════════════════════════════════════════════════════════
    // GETTERS (Stats via AttributeSet)
    // ═══════════════════════════════════════════════════════════════
    
    UFUNCTION(BlueprintPure, Category = "Enemy|Stats")
    float GetHealth() const;

    UFUNCTION(BlueprintPure, Category = "Enemy|Stats")
    float GetMaxHealth() const;

    UFUNCTION(BlueprintPure, Category = "Enemy|Stats")
    float GetDamage() const;

    UFUNCTION(BlueprintPure, Category = "Enemy|Stats")
    float GetArmor() const;

    UFUNCTION(BlueprintPure, Category = "Enemy|Stats")
    FGameplayTagContainer GetBehaviorTags() const { return BehaviorTags; }

    // ═══════════════════════════════════════════════════════════════
    // DEBUG
    // ═══════════════════════════════════════════════════════════════
    
    UFUNCTION(BlueprintCallable, Category = "Enemy|Debug")
    void DebugPrintStats() const;

protected:
    // ═══════════════════════════════════════════════════════════════
    // COMPONENTS
    // ═══════════════════════════════════════════════════════════════
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
    UAbilitySystemComponent* AbilitySystemComponent;

    UPROPERTY()
    UDarkAttributeSet* AttributeSet;

    // ═══════════════════════════════════════════════════════════════
    // ENEMY DATA
    // ═══════════════════════════════════════════════════════════════
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy", Meta = (ExposeOnSpawn = "true"))
    FName EnemyRowName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy", Meta = (ExposeOnSpawn = "true"))
    int32 CurrentFloor = 1;

    FVector SpawnLocation = FVector::ZeroVector;
    
    UPROPERTY(BlueprintReadOnly, Category = "Enemy")
    FGameplayTagContainer BehaviorTags;

    /** Cached enemy data pointer (for AI controller access) */
    const FStandardEnemyRow* CachedEnemyData = nullptr;
    

    // ═══════════════════════════════════════════════════════════════
    // RESOURCE SYSTEM (non-GAS properties)
    // ═══════════════════════════════════════════════════════════════
    
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Resource")
    EResourceType ResourceType;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Resource")
    bool bRegenInCombat;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Resource")
    float RegenDelay;

    float RegenDelayTimer;

    UPROPERTY(BlueprintReadOnly, Category = "Resource")
    bool bIsInCombat;

    // ═══════════════════════════════════════════════════════════════
    // AI RESOURCE MANAGEMENT
    // ═══════════════════════════════════════════════════════════════
    
    UPROPERTY(BlueprintReadOnly, Category = "AI")
    bool bConserveResourceForEmergency;

    UPROPERTY(BlueprintReadOnly, Category = "AI")
    float EmergencyResourceReserve;

    UPROPERTY(BlueprintReadOnly, Category = "AI")
    float LowResourceThreshold;
};