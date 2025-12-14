#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Utils/Types/UtilsTypes.h"
#include "UtilsLibrary.generated.h"

struct FGameplayEventData;
struct FGameplayTag;

/**
 * Utils Library
 * Bibliothèque de fonctions utilitaires pour la gestion des propriétés de session
 */
UCLASS()
class DARKSCRIPT_API UUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// ═══════════════════════════════════════════════════════════════════════════════
	// ═══ FUNCTIONS (BLUEPRINT API) ═════════════════════════════════════════════════
	// ═══════════════════════════════════════════════════════════════════════════════

	// ─── Make Literal Session Property ───

	/**
	 * Crée une propriété de session de type Byte
	 * @param Key Le nom de la clé de la propriété
	 * @param Value La valeur byte à stocker
	 * @return La paire clé-valeur créée
	 */
	UFUNCTION(BlueprintPure, Category = "Utils|Variables")
	static FPropertyKeyPair MakeLiteralSessionPropertyByte(FName Key, uint8 Value);

	/**
	 * Crée une propriété de session de type Bool
	 * @param Key Le nom de la clé de la propriété
	 * @param Value La valeur booléenne à stocker
	 * @return La paire clé-valeur créée
	 */
	UFUNCTION(BlueprintPure, Category = "Utils|Variables")
	static FPropertyKeyPair MakeLiteralSessionPropertyBool(FName Key, bool Value);

	/**
	 * Crée une propriété de session de type String
	 * @param Key Le nom de la clé de la propriété
	 * @param Value La valeur string à stocker
	 * @return La paire clé-valeur créée
	 */
	UFUNCTION(BlueprintPure, Category = "Utils|Variables")
	static FPropertyKeyPair MakeLiteralSessionPropertyString(FName Key, const FString& Value);

	/**
	 * Crée une propriété de session de type Int
	 * @param Key Le nom de la clé de la propriété
	 * @param Value La valeur int32 à stocker
	 * @return La paire clé-valeur créée
	 */
	UFUNCTION(BlueprintPure, Category = "Utils|Variables")
	static FPropertyKeyPair MakeLiteralSessionPropertyInt(FName Key, int32 Value);

	/**
	 * Crée une propriété de session de type Float
	 * @param Key Le nom de la clé de la propriété
	 * @param Value La valeur float à stocker
	 * @return La paire clé-valeur créée
	 */
	UFUNCTION(BlueprintPure, Category = "Utils|Variables")
	static FPropertyKeyPair MakeLiteralSessionPropertyFloat(FName Key, float Value);

	// ─── Get Session Property ───

	/**
	 * Récupère une propriété de session de type Byte
	 * @param Properties Le tableau de propriétés dans lequel chercher
	 * @param Key Le nom de la clé à rechercher
	 * @param Result Success si la propriété est trouvée et du bon type, Failure sinon
	 * @param Value La valeur récupérée (valable uniquement si Result est Success)
	 */
	UFUNCTION(BlueprintCallable, Category = "Utils|Variables", meta=(ExpandEnumAsExecs = "Result"))
	static void GetSessionPropertyByte(const TArray<FPropertyKeyPair>& Properties, const FName Key, EBlueprintResultSwitch &Result, uint8& Value);

	/**
	 * Récupère une propriété de session de type Bool
	 * @param Properties Le tableau de propriétés dans lequel chercher
	 * @param Key Le nom de la clé à rechercher
	 * @param Result Success si la propriété est trouvée et du bon type, Failure sinon
	 * @param Value La valeur récupérée (valable uniquement si Result est Success)
	 */
	UFUNCTION(BlueprintCallable, Category = "Utils|Variables", meta=(ExpandEnumAsExecs = "Result"))
	static void GetSessionPropertyBool(const TArray<FPropertyKeyPair>& Properties, const FName Key, EBlueprintResultSwitch &Result, bool& Value);

	/**
	 * Récupère une propriété de session de type String
	 * @param Properties Le tableau de propriétés dans lequel chercher
	 * @param Key Le nom de la clé à rechercher
	 * @param Result Success si la propriété est trouvée et du bon type, Failure sinon
	 * @param Value La valeur récupérée (valable uniquement si Result est Success)
	 */
	UFUNCTION(BlueprintCallable, Category = "Utils|Variables", meta=(ExpandEnumAsExecs = "Result"))
	static void GetSessionPropertyString(const TArray<FPropertyKeyPair>& Properties, const FName Key, EBlueprintResultSwitch &Result, FString& Value);

	/**
	 * Récupère une propriété de session de type Int
	 * @param Properties Le tableau de propriétés dans lequel chercher
	 * @param Key Le nom de la clé à rechercher
	 * @param Result Success si la propriété est trouvée et du bon type, Failure sinon
	 * @param Value La valeur récupérée (valable uniquement si Result est Success)
	 */
	UFUNCTION(BlueprintCallable, Category = "Utils|Variables", meta=(ExpandEnumAsExecs = "Result"))
	static void GetSessionPropertyInt(const TArray<FPropertyKeyPair>& Properties, const FName Key, EBlueprintResultSwitch &Result, int32& Value);

	/**
	 * Récupère une propriété de session de type Float
	 * @param Properties Le tableau de propriétés dans lequel chercher
	 * @param Key Le nom de la clé à rechercher
	 * @param Result Success si la propriété est trouvée et du bon type, Failure sinon
	 * @param Value La valeur récupérée (valable uniquement si Result est Success)
	 */
	UFUNCTION(BlueprintCallable, Category = "Utils|Variables", meta=(ExpandEnumAsExecs = "Result"))
	static void GetSessionPropertyFloat(const TArray<FPropertyKeyPair>& Properties, const FName Key, EBlueprintResultSwitch &Result, float& Value);
};
