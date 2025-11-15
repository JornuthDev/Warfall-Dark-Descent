#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GlobalTypes.generated.h"

/**
 * Defines the player's main archetype category.
 */
UENUM(BlueprintType)
enum class EPlayerArchetype : uint8
{
	E_Tank UMETA(DisplayName = "Tank"),
	E_Healer UMETA(DisplayName = "Healer"),
	E_Ranger UMETA(DisplayName = "Ranger"),
	E_Mage UMETA(DisplayName = "Mage"),
};

/**
 * Defines a statistic entry that can be used in character systems.
 */
USTRUCT(BlueprintType)
struct FStatDefinition
{
	GENERATED_BODY()
	
	/** Tag de la statistique ou attribut disponible (ex: Attributes.Strength) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="Attributes;Statistics"))
	FGameplayTag StatTag;

	/** Valeur minimale et maximale autorisée (facultatif, pour les équilibrages) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector2D ValueRange = FVector2D(0.f, 100.f);

	/** Poids de cette statistique dans les choix aléatoires (facultatif) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight = 1.f;
};