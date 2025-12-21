// DynamicGameplayEffect.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "DynamicGameplayEffect.generated.h"

/**
 * UDynamicGameplayEffect
 * 
 * Classe GameplayEffect réutilisable qui peut être configurée dynamiquement
 * Contrairement aux NewObject, cette classe peut être instanciée et répliquée correctement
 * 
 * Usage:
 *   UDynamicGameplayEffect* Effect = UDynamicGameplayEffect::CreateDynamicEffect(WorldContext, Config);
 */
UCLASS()
class DARKSCRIPT_API UDynamicGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	/**
	 * Crée un effet dynamique configuré à partir d'une structure
	 * Cette méthode crée un VRAI objet UGameplayEffect qui peut être répliqué
	 */
	static UDynamicGameplayEffect* CreateDynamicEffect(
		const UObject* WorldContextObject,
		const struct FDynamicEffectConfig& Config);

	/**
	 * Configure cet effet avec les paramètres fournis
	 */
	void ConfigureFromStruct(const struct FDynamicEffectConfig& Config);

	/**
	 * Identifiant unique pour cet effet (pour debug)
	 */
	UPROPERTY()
	FString EffectIdentifier;
};