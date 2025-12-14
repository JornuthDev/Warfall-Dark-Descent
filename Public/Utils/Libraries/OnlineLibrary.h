#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OnlineLibrary.generated.h"

/**
 * Online Library
 * Bibliothèque de fonctions pour gérer les fonctionnalités en ligne
 */
UCLASS()
class DARKSCRIPT_API UOnlineLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// ═══════════════════════════════════════════════════════════════════════════════
	// ═══ FUNCTIONS (BLUEPRINT API) ═════════════════════════════════════════════════
	// ═══════════════════════════════════════════════════════════════════════════════

	/**
	 * Récupère l'identifiant Steam d'un joueur
	 * @param WorldContextObject Contexte du monde
	 * @param PlayerController Contrôleur du joueur dont on veut récupérer l'ID
	 * @return L'identifiant Steam du joueur sous forme de FString
	 */
	UFUNCTION(BlueprintPure, Category="Online", meta=(WorldContext="WorldContextObject"))
	static FString GetPlayerSteamID(const UObject* WorldContextObject, const APlayerController* PlayerController);

	/**
	 * Récupère l'instance PIE (Play In Editor) d'un joueur
	 * Utile pour identifier les différentes instances de joueur en mode éditeur
	 * @param WorldContextObject Contexte du monde
	 * @param PlayerController Contrôleur du joueur
	 * @return L'identifiant de l'instance PIE sous forme de FString
	 */
	UFUNCTION(BlueprintPure, Category="Online", meta=(WorldContext="WorldContextObject"))
	static FString GetPieInstance(const UObject* WorldContextObject, const APlayerController* PlayerController);

};
