// DynamicEffectConfig.h - Structure de configuration pour les effets dynamiques
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayModMagnitudeCalculation.h"
#include "DynamicEffectConfig.generated.h"

/**
 * Configuration d'un modifier d'attribut
 */
USTRUCT(BlueprintType)
struct FDynamicModifierConfig
{
	GENERATED_BODY()

	/** Attribut à modifier */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	FGameplayAttribute Attribute;

	/** Opération de modification */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	TEnumAsByte<EGameplayModOp::Type> ModifierOp = EGameplayModOp::Additive;

	/** Magnitude fixe (si pas de calcul) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	float Magnitude = 0.f;

	/** Classe de calcul optionnelle (pour calculs complexes) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	TSubclassOf<UGameplayModMagnitudeCalculation> CalculationClass;

	/** Tag SetByCaller pour magnitude dynamique */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	FGameplayTag SetByCallerTag;

	/** Utiliser SetByCaller ? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	bool bUseSetByCaller = false;

	FDynamicModifierConfig() = default;

	FDynamicModifierConfig(
		const FGameplayAttribute& InAttribute,
		const float InMagnitude,
		const EGameplayModOp::Type InOp = EGameplayModOp::Additive)
		: Attribute(InAttribute)
		, ModifierOp(InOp)
		, Magnitude(InMagnitude)
		, bUseSetByCaller(false)
	{}

	FDynamicModifierConfig(
		const FGameplayAttribute& InAttribute,
		const TSubclassOf<UGameplayModMagnitudeCalculation> InCalcClass,
		const EGameplayModOp::Type InOp = EGameplayModOp::Additive)
		: Attribute(InAttribute)
		, ModifierOp(InOp)
		, CalculationClass(InCalcClass)
		, bUseSetByCaller(false)
	{}
};

/**
 * Configuration complète d'un GameplayEffect dynamique
 */
USTRUCT(BlueprintType)
struct FDynamicEffectConfig
{
	GENERATED_BODY()

	/** Liste des modifiers à appliquer */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TArray<FDynamicModifierConfig> Modifiers;

	/** Politique de durée */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|Duration")
	EGameplayEffectDurationType DurationPolicy = EGameplayEffectDurationType::Instant;

	/** Durée (si HasDuration) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|Duration", meta = (EditCondition = "DurationPolicy == EGameplayEffectDurationType::HasDuration"))
	float Duration = 1.0f;

	/** Tag SetByCaller pour la durée */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|Duration")
	FGameplayTag DurationSetByCallerTag;

	/** Utiliser SetByCaller pour la durée ? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|Duration")
	bool bUseDurationSetByCaller = false;

	/** Tags accordés par cet effet */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|Tags")
	FGameplayTagContainer GrantedTags;

	/** Tags requis pour appliquer cet effet */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|Tags")
	FGameplayTagContainer ApplicationRequiredTags;

	/** Tags qui bloquent l'application */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|Tags")
	FGameplayTagContainer ApplicationBlockedTags;

	/** Identifiant unique (optionnel, pour debug) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FString Identifier;

	/** Période pour les effets périodiques */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|Periodic")
	float Period = 0.f;

	/** Exécuter à l'application ? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|Periodic")
	bool bExecutePeriodicEffectOnApplication = false;

	FDynamicEffectConfig() = default;

	// Constructeur simple pour un effet instant
	FDynamicEffectConfig(
		const FGameplayAttribute& Attribute,
		const float Magnitude,
		const EGameplayModOp::Type Op = EGameplayModOp::Additive)
		: DurationPolicy(EGameplayEffectDurationType::Instant)
	{
		Modifiers.Add(FDynamicModifierConfig(Attribute, Magnitude, Op));
	}

	// Constructeur pour effet avec calcul
	FDynamicEffectConfig(
		const FGameplayAttribute& Attribute,
		const TSubclassOf<UGameplayModMagnitudeCalculation> CalcClass,
		const EGameplayModOp::Type Op = EGameplayModOp::Additive)
		: DurationPolicy(EGameplayEffectDurationType::Infinite)
	{
		Modifiers.Add(FDynamicModifierConfig(Attribute, CalcClass, Op));
	}

	// Constructeur pour multiples modifiers
	explicit FDynamicEffectConfig(const TArray<FDynamicModifierConfig>& InModifiers)
		: Modifiers(InModifiers)
		, DurationPolicy(EGameplayEffectDurationType::Instant)
	{}
};