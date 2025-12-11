#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Utils/Types/PlayerTypes.h"
#include "EnemyAbilityData.generated.h"

/**
 * Type de ciblage d'une ability
 */
UENUM(BlueprintType)
enum class EAbilityTargetType : uint8
{
    Self        UMETA(DisplayName = "Self (se cible lui-même)"),
    Enemy       UMETA(DisplayName = "Enemy (cible un ennemi)"),
    Ally        UMETA(DisplayName = "Ally (cible un allié)"),
    Ground      UMETA(DisplayName = "Ground (cible le sol)"),
    Direction   UMETA(DisplayName = "Direction (direction)"),
    None        UMETA(DisplayName = "None (pas de cible)")
};

/**
 * Priorité d'utilisation d'une ability par l'IA
 */
UENUM(BlueprintType)
enum class EAbilityPriority : uint8
{
    VeryLow     UMETA(DisplayName = "Very Low"),
    Low         UMETA(DisplayName = "Low"),
    Normal      UMETA(DisplayName = "Normal"),
    High        UMETA(DisplayName = "High"),
    Critical    UMETA(DisplayName = "Critical (Emergency)")
};

/**
 * Structure de données pour une Ability
 * Utilisée dans DT_Abilities
 */
USTRUCT(BlueprintType)
struct FEnemyAbilityRow : public FTableRowBase
{
    GENERATED_BODY()

    // ═══════════════════════════════════════════════════════════════
    // IDENTIFICATION
    // ═══════════════════════════════════════════════════════════════

    /** Nom affiché de l'ability */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
    FText DisplayName;

    /** Description de l'ability */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
    FText Description;

    /** Classe de GameplayAbility à activer */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
    TSubclassOf<UGameplayAbility> AbilityClass;

    /** Tags de l'ability (ex: Ability.Magic.Fireball) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity", Meta = (Categories = "Ability"))
    FGameplayTagContainer AbilityTags;

    // ═══════════════════════════════════════════════════════════════
    // COÛT & RESSOURCE
    // ═══════════════════════════════════════════════════════════════

    /** Type de ressource consommée */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cost")
    EResourceType ResourceType = EResourceType::None;

    /** Coût en ressource */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cost", Meta = (EditCondition = "ResourceType != EResourceType::None", EditConditionHides))
    float ResourceCost = 0.0f;

    /** Cooldown de base (secondes) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cost")
    float Cooldown = 0.0f;

    /** Temps de cast (secondes, 0 = instant) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cost")
    float CastTime = 0.0f;

    // ═══════════════════════════════════════════════════════════════
    // CIBLAGE & PORTÉE
    // ═══════════════════════════════════════════════════════════════

    /** Type de ciblage */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting")
    EAbilityTargetType TargetType = EAbilityTargetType::Enemy;

    /** Portée maximale (0 = melee) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting")
    float MaxRange = 0.0f;

    /** Portée minimale (pour garder distance) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting")
    float MinRange = 0.0f;

    /** Rayon d'effet (AoE, 0 = single target) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting")
    float EffectRadius = 0.0f;

    /** Nécessite une ligne de vue (Line of Sight) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting")
    bool bRequiresLineOfSight = true;

    // ═══════════════════════════════════════════════════════════════
    // EFFETS DE BASE (pour IA simple)
    // ═══════════════════════════════════════════════════════════════

    /** Dégâts de base (si ability inflige dégâts) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    float BaseDamage = 0.0f;

    /** Soins de base (si ability soigne) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    float BaseHealing = 0.0f;

    /** Durée des effets (DoT, buff, debuff en secondes) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    float EffectDuration = 0.0f;

    // ═══════════════════════════════════════════════════════════════
    // IA - SCORING & USAGE
    // ═══════════════════════════════════════════════════════════════

    /** Priorité de base pour l'IA (plus haut = préféré) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Usage")
    EAbilityPriority BasePriority = EAbilityPriority::Normal;

    /** Score de base (0-100, l'IA choisit le meilleur score) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Usage", Meta = (ClampMin = "0", ClampMax = "100"))
    float BaseScore = 50.0f;

    /** L'IA peut-elle utiliser cette ability en mouvement ? */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Usage")
    bool bCanUseWhileMoving = false;

    /** HP% minimum de la cible pour utiliser (ex: execute sous 20%) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Usage", Meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float TargetHealthThresholdMin = 0.0f;

    /** HP% maximum de la cible pour utiliser (ex: heal uniquement si < 70%) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Usage", Meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float TargetHealthThresholdMax = 1.0f;

    /** HP% minimum du SELF pour utiliser (ex: shield si < 50%) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Usage", Meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float SelfHealthThresholdMin = 0.0f;

    /** Nombre minimum d'ennemis dans l'AoE pour utiliser */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Usage", Meta = (EditCondition = "EffectRadius > 0", EditConditionHides))
    int32 MinTargetsForAoE = 2;

    /** L'IA utilise uniquement en combo avec un autre tag ? */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Usage", Meta = (Categories = "Ability"))
    FGameplayTagContainer RequiredComboTags;

    // ═══════════════════════════════════════════════════════════════
    // ANIMATIONS & VISUELS (optionnel pour l'instant)
    // ═══════════════════════════════════════════════════════════════

    /** Montage d'animation à jouer */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
    UAnimMontage* CastAnimation;

    /** Effet visuel (particules, VFX) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
    UParticleSystem* VisualEffect;

    /** Son de cast */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    USoundBase* CastSound;

    // ═══════════════════════════════════════════════════════════════
    // CONSTRUCTOR
    // ═══════════════════════════════════════════════════════════════

    FEnemyAbilityRow()
        : DisplayName(FText::FromString("New Ability"))
        , Description(FText::FromString(""))
        , AbilityClass(nullptr)
        , ResourceType(EResourceType::None)
        , ResourceCost(0.0f)
        , Cooldown(0.0f)
        , CastTime(0.0f)
        , TargetType(EAbilityTargetType::Enemy)
        , MaxRange(0.0f)
        , MinRange(0.0f)
        , EffectRadius(0.0f)
        , bRequiresLineOfSight(true)
        , BaseDamage(0.0f)
        , BaseHealing(0.0f)
        , EffectDuration(0.0f)
        , BasePriority(EAbilityPriority::Normal)
        , BaseScore(50.0f)
        , bCanUseWhileMoving(false)
        , TargetHealthThresholdMin(0.0f)
        , TargetHealthThresholdMax(1.0f)
        , SelfHealthThresholdMin(0.0f)
        , MinTargetsForAoE(2)
        , CastAnimation(nullptr)
        , VisualEffect(nullptr)
        , CastSound(nullptr)
    {}
};