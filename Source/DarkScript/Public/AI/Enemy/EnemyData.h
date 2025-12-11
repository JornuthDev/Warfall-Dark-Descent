#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Utils/Helpers/ContentHelpers.h"
#include "Utils/Types/PlayerTypes.h"
#include "EnemyData.generated.h"

/**
 * Structure de données pour un ennemi standard
 * Utilisée dans une DataTable (DT_StandardEnemies)
 */
USTRUCT(BlueprintType)
struct FStandardEnemyRow : public FTableRowBase
{
    GENERATED_BODY()

    // ═══════════════════════════════════════════════════════════════
    // IDENTIFICATION
    // ═══════════════════════════════════════════════════════════════

    /** Nom affiché de l'ennemi */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
    FText DisplayName;

    /** Description de l'ennemi (optionnel, pour documentation) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
    FText Description;

    /** Blueprint de l'ennemi à spawner */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
    TSubclassOf<class ACharacter> EnemyClass;

    /** Thèmes assignés (ex: Fire, Ice, Poison, Undead, Forest) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
    TArray<FName> AssignedThemes;

    // ═══════════════════════════════════════════════════════════════
    // COMPORTEMENTS (GameplayTags)
    // ═══════════════════════════════════════════════════════════════

    /** Tags définissant le comportement de l'IA */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Behavior", Meta = (Categories = "AI.Behavior"))
    FGameplayTagContainer BehaviorTags;

    // ═══════════════════════════════════════════════════════════════
    // BEHAVIOR TREE
    // ═══════════════════════════════════════════════════════════════

    /** Behavior Tree à utiliser pour cette IA */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    class UBehaviorTree* BehaviorTree;

    // ═══════════════════════════════════════════════════════════════
    // ABILITIES ⭐ NOUVEAU
    // ═══════════════════════════════════════════════════════════════

    /** Abilities accordées à cet ennemi (rows depuis DT_Abilities) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
    TArray<FName> AbilityRows;
    
    // ═══════════════════════════════════════════════════════════════
    // ATTRIBUTS DE BASE
    // ═══════════════════════════════════════════════════════════════

    /** Points de vie de base */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
    float BaseHealth = 100.0f;

    /** Dégâts de base */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
    float BaseDamage = 10.0f;

    /** Vitesse de déplacement de base */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
    float BaseMoveSpeed = 400.0f;
        
    // ═══════════════════════════════════════════════════════════════
    // RESISTANCES ⭐ NOUVEAU
    // ═══════════════════════════════════════════════════════════════

    /** Armure de base */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
    float BaseArmor = 0.0f;
    
    /** Résistance au froid (-100 = faiblesse, +100 = immunité) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resistances", Meta = (ClampMin = "-100", ClampMax = "100"))
    float BaseIceResistance = 0.0f;

    /** Résistance au feu */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resistances", Meta = (ClampMin = "-100", ClampMax = "100"))
    float BaseFireResistance = 0.0f;

    /** Résistance à la foudre */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resistances", Meta = (ClampMin = "-100", ClampMax = "100"))
    float BaseLightningResistance = 0.0f;

    /** Résistance au sacré */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resistances", Meta = (ClampMin = "-100", ClampMax = "100"))
    float BaseHolyResistance = 0.0f;

    // ═══════════════════════════════════════════════════════════════
    // RESSOURCES ⭐ NOUVEAU
    // ═══════════════════════════════════════════════════════════════

    /** Type de ressource utilisé par cet ennemi */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources")
    EResourceType ResourceType = EResourceType::None;

    /** Quantité maximale de la ressource */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources", Meta = (EditCondition = "ResourceType != EResourceType::None", EditConditionHides))
    float BaseResourceMax = 100.0f;

    /** Ressource de départ (% du max, 0.0 à 1.0) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources", Meta = (EditCondition = "ResourceType != EResourceType::None", EditConditionHides, ClampMin = "0.0", ClampMax = "1.0"))
    float StartingResourcePercent = 1.0f;

    /** Vitesse de régénération naturelle (unités par seconde) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources", Meta = (EditCondition = "ResourceType != EResourceType::None", EditConditionHides))
    float ResourceRegenRate = 5.0f;

    /** La régénération fonctionne-t-elle en combat ? */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources", Meta = (EditCondition = "ResourceType != EResourceType::None", EditConditionHides))
    bool bRegenInCombat = false;

    /** Délai avant régénération après utilisation d'une capacité (secondes) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources", Meta = (EditCondition = "ResourceType != EResourceType::None", EditConditionHides))
    float RegenDelay = 0.0f;

    /** Seuil de ressource "basse" (% du max, déclenche comportements économes) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources", Meta = (EditCondition = "ResourceType != EResourceType::None", EditConditionHides, ClampMin = "0.0", ClampMax = "1.0"))
    float LowResourceThreshold = 0.3f;

    // ═══════════════════════════════════════════════════════════════
    // GESTION INTELLIGENTE DES RESSOURCES (IA) ⭐ NOUVEAU
    // ═══════════════════════════════════════════════════════════════

    /** L'IA conserve-t-elle toujours une réserve pour les urgences ? */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Resource Management", Meta = (EditCondition = "ResourceType != EResourceType::None", EditConditionHides))
    bool bConserveResourceForEmergency = true;

    /** % de ressource minimum à toujours garder en réserve (0.0 à 1.0) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Resource Management", Meta = (EditCondition = "bConserveResourceForEmergency", EditConditionHides, ClampMin = "0.0", ClampMax = "1.0"))
    float EmergencyResourceReserve = 0.2f;

    /** L'IA priorise-t-elle les capacités low-cost quand ressource < LowResourceThreshold ? */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Resource Management", Meta = (EditCondition = "ResourceType != EResourceType::None", EditConditionHides))
    bool bUseLowCostAbilitiesWhenLow = true;

    /** L'IA arrête-t-elle d'utiliser des capacités si ressource trop basse ? */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Resource Management", Meta = (EditCondition = "ResourceType != EResourceType::None", EditConditionHides))
    bool bStopUsingAbilitiesWhenEmpty = true;

    // ═══════════════════════════════════════════════════════════════
    // SCALING PAR PALIER (Curves)
    // ═══════════════════════════════════════════════════════════════

    /** Courbe de scaling de la santé selon le palier */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scaling")
    class UCurveFloat* HealthScalingCurve;

    /** Courbe de scaling des dégâts selon le palier */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scaling")
    class UCurveFloat* DamageScalingCurve;

    /** Courbe de scaling de la ressource max selon le palier */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scaling", Meta = (EditCondition = "ResourceType != EResourceType::None", EditConditionHides))
    class UCurveFloat* ResourceScalingCurve;

    /** Courbe de scaling de l'intelligence (réactivité) selon le palier */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scaling")
    class UCurveFloat* IntelligenceScalingCurve;

    // ═══════════════════════════════════════════════════════════════
    // PERCEPTION
    // ═══════════════════════════════════════════════════════════════

    /** Rayon de vision (distance en unités Unreal) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perception")
    float SightRadius = 1000.0f;

    /** Rayon d'ouïe (distance en unités Unreal) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perception")
    float HearingRadius = 800.0f;

    /** Angle de vision périphérique (en degrés, 180 = vision complète devant) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perception", Meta = (ClampMin = "0.0", ClampMax = "180.0"))
    float PeripheralVisionAngle = 90.0f;

    /** Durée de perte de vue avant d'oublier la cible (secondes) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perception")
    float LoseSightDuration = 5.0f;

    // ═══════════════════════════════════════════════════════════════
    // PARAMÈTRES DE COMPORTEMENT
    // ═══════════════════════════════════════════════════════════════

    /** % HP en dessous duquel l'IA fuit (si tag Flee présent) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Behavior Parameters", Meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float FleeHealthThreshold = 0.2f;

    /** Rayon pour appeler des renforts (si tag CallHelp présent) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Behavior Parameters")
    float ReinforcementCallRange = 1000.0f;

    /** Distance de combat préférée (l'IA essaie de maintenir cette distance) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Behavior Parameters")
    float PreferredCombatRange = 300.0f;

    /** Distance minimale acceptable (ne va jamais plus proche) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Behavior Parameters")
    float MinimumCombatRange = 100.0f;

    /** Distance maximale acceptable (ne va jamais plus loin) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Behavior Parameters")
    float MaximumCombatRange = 1500.0f;

    // ═══════════════════════════════════════════════════════════════
    // PATROUILLE
    // ═══════════════════════════════════════════════════════════════

    /** Vitesse de patrouille */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Patrol")
    float PatrolSpeed = 200.0f;

    /** Temps d'idle minimum entre deux déplacements (secondes) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Patrol")
    float IdleTimeMin = 2.0f;

    /** Temps d'idle maximum entre deux déplacements (secondes) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Patrol")
    float IdleTimeMax = 5.0f;

    /** Rayon de patrouille aléatoire autour du point de spawn */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Patrol")
    float PatrolRadius = 500.0f;

    // ═══════════════════════════════════════════════════════════════
    // INTERACTIONS ENVIRONNEMENT
    // ═══════════════════════════════════════════════════════════════

    /** Classes d'interactables valides pour cette IA (si tag Interactive présent) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactions")
    TArray<TSubclassOf<class AActor>> ValidInteractableClasses;

    /** Rayon de recherche d'interactables */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactions")
    float InteractableSearchRadius = 1000.0f;

    /** Durée moyenne d'une interaction (secondes) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactions")
    float AverageInteractionDuration = 5.0f;

    // ═══════════════════════════════════════════════════════════════
    // ANIMATIONS
    // ═══════════════════════════════════════════════════════════════

    /** Animations idle variées (jouées aléatoirement) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animations")
    TArray<class UAnimMontage*> IdleAnimations;

    /** Animations de patrouille (ex: regarder autour, s'étirer) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animations")
    TArray<class UAnimMontage*> PatrolAnimations;

    /** Animation de mort */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animations")
    class UAnimMontage* DeathAnimation;

    // ═══════════════════════════════════════════════════════════════
    // VISUEL
    // ═══════════════════════════════════════════════════════════════

    /** Mesh squelettique de l'ennemi */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
    class USkeletalMesh* Mesh;

    /** Animation Blueprint à utiliser */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
    TSubclassOf<class UAnimInstance> AnimClass;

    // ═══════════════════════════════════════════════════════════════
    // AUDIO
    // ═══════════════════════════════════════════════════════════════

    /** Sons joués lors de l'aggro */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    TArray<class USoundBase*> AggroSounds;

    /** Sons joués lors de la mort */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    TArray<class USoundBase*> DeathSounds;

    /** Sons joués durant le combat (aléatoires) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    TArray<class USoundBase*> CombatSounds;
    
    /*FStandardEnemyRow() 
    :DisplayName(FText::GetEmpty())
    ,Description(FText::GetEmpty())
    ,EnemyClass(nullptr)
    ,AssignedThemes(TArray<FName>())
    ,BehaviorTags(FGameplayTagContainer())
    ,BehaviorTree(nullptr)
    ,AbilityRows(TArray<FName>())
    ,HealthScalingCurve(GetFloatCurve(CurvePaths::HealthScaling))
    ,DamageScalingCurve(GetFloatCurve(CurvePaths::DamageScaling))
    ,ResourceScalingCurve(GetFloatCurve(CurvePaths::ResourceScaling))
    ,IntelligenceScalingCurve(GetFloatCurve(CurvePaths::IntelligenceScaling))
    ,ValidInteractableClasses(TArray<TSubclassOf<class AActor>>())
    ,IdleAnimations(TArray<class UAnimMontage*>())
    ,PatrolAnimations(TArray<class UAnimMontage*>())
    ,DeathAnimation(nullptr)
    ,Mesh(nullptr)
    ,AnimClass(nullptr)
    ,AggroSounds(TArray<class USoundBase*>())
    ,DeathSounds(TArray<class USoundBase*>())
    ,CombatSounds(TArray<class USoundBase*>())
    {};*/
};