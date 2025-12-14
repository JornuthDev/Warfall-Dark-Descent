#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Utils/Types/OnlineTypes.h"
#include "DarkLibrary.generated.h"

class UGlobalAttributeSet;
class UArchetypeAttributeSet;
class UProgression;
class APlayerControllerBase;

/**
 * Dark Library
 * Bibliothèque principale de fonctions pour le projet Dark
 * Fournit un accès rapide aux systèmes principaux du jeu
 */
UCLASS()
class DARKSCRIPT_API UDarkLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// ═══════════════════════════════════════════════════════════════════════════════
	// ═══ FUNCTIONS (BLUEPRINT API) ═════════════════════════════════════════════════
	// ═══════════════════════════════════════════════════════════════════════════════

	// ─── Game ───

	/**
	 * Récupère le PlayerController principal du joueur
	 * @param WorldContextObject Contexte du monde
	 * @param PlayerIndex Index du joueur (généralement 0 pour le joueur local)
	 * @return Le PlayerController castĂ© en APlayerControllerBase
	 */
	UFUNCTION(BlueprintPure, Category="Gameplay", meta=(WorldContext="WorldContextObject", UnsafeDuringActorConstruction="true"))
	static APlayerControllerBase* GetPlayerController(const UObject* WorldContextObject, int32 PlayerIndex);

	// ─── Gameplay / Ability ───

	/**
	 * Récupère l'archétype de classe du joueur
	 * @param WorldContextObject Contexte du monde
	 * @return L'archétype de classe actuel (Warrior, Mage, etc.)
	 */
	UFUNCTION(BlueprintPure, Category="Gameplay|Archetype", meta=(WorldContext="WorldContextObject"))
	static EClassArchetype GetArchetype(const UObject* WorldContextObject);

	/**
	 * Définit l'archétype de classe du joueur
	 * @param WorldContextObject Contexte du monde
	 * @param Archetype Le nouvel archétype à appliquer
	 */
	UFUNCTION(BlueprintCallable, Category="Gameplay|Archetype", meta=(WorldContext="WorldContextObject"))
	static void SetArchetype(UObject* WorldContextObject, const EClassArchetype Archetype);
	
	/**
	 * Récupère le niveau actuel du joueur
	 * @param WorldContextObject Contexte du monde
	 * @return Le niveau du joueur ou INDEX_NONE si non trouvé
	 */
	UFUNCTION(BlueprintPure, Category="Gameplay|Global", meta=(WorldContext="WorldContextObject"))
	static int32 GetPlayerLevel(UObject* WorldContextObject);
	
	/**
	 * Applique des dégâts à une cible
	 * @param WorldContextObject Contexte du monde
	 * @param Target La cible à endommager
	 * @param Type Le type de dégâts (Physical, Magical, etc.)
	 * @param Value La valeur des dégâts (sera convertie en négatif)
	 * @return Le handle de l'effet de gameplay appliqué
	 */
	UFUNCTION(BlueprintCallable, Category="Gameplay|Effect", meta=(WorldContext="WorldContextObject"))
	static FActiveGameplayEffectHandle ApplyDamageToTarget(UObject* WorldContextObject, AActor* Target, const EDamageType Type, const float Value);

	/**
	 * Applique des soins à une cible
	 * @param WorldContextObject Contexte du monde
	 * @param Target La cible à soigner
	 * @param Type Le type de soins
	 * @param Value La valeur des soins (sera convertie en positif)
	 * @return Le handle de l'effet de gameplay appliqué
	 */
	UFUNCTION(BlueprintCallable, Category="Gameplay|Effect", meta=(WorldContext="WorldContextObject"))
	static FActiveGameplayEffectHandle ApplyHealToTarget(UObject* WorldContextObject, AActor* Target, const EDamageType Type, const float Value);

	// ═══════════════════════════════════════════════════════════════════════════════
	// ═══ ABILITY SYSTEMS - ACCÈS RAPIDE ════════════════════════════════════════════
	// ═══════════════════════════════════════════════════════════════════════════════
	
	/**
	 * Récupère le GlobalAbilitySystem depuis n'importe quel contexte
	 * 
	 * Utilise le WorldContextObject pour trouver le LocalPlayerController,
	 * puis accède au PlayerState qui contient le GlobalAbilitySystem
	 * 
	 * Utile pour :
	 * - Accéder au Level global
	 * - Accéder à l'XP globale
	 * - Appliquer des effets globaux (buffs de guilde, etc.)
	 * 
	 * @param WorldContextObject N'importe quel objet du monde (Actor, Component, Widget, etc.)
	 * @return Le GlobalAbilitySystem ou nullptr si non trouvé
	 */
	UFUNCTION(BlueprintPure, Category = "Gameplay|Global", meta = (WorldContext = "WorldContextObject"))
	static UGlobalAbilitySystem* GetGlobalAbilitySystem(UObject* WorldContextObject);

	/**
	 * Récupère l'ArchetypeAbilitySystem depuis n'importe quel contexte
	 * 
	 * Utilise le WorldContextObject pour trouver le LocalPlayerController,
	 * puis accède au Character qui contient l'ArchetypeAbilitySystem
	 * 
	 * Utile pour :
	 * - Accéder à la Health/Mana de l'archétype incarné
	 * - Appliquer des effets de combat
	 * - Vérifier l'état du personnage (mort, vivant, etc.)
	 * 
	 * @param WorldContextObject N'importe quel objet du monde (Actor, Component, Widget, etc.)
	 * @return L'ArchetypeAbilitySystem ou nullptr si non trouvé
	 */
	UFUNCTION(BlueprintPure, Category = "Gameplay|Archetype", meta = (WorldContext = "WorldContextObject"))
	static UArchetypeAbilitySystem* GetArchetypeAbilitySystem(UObject* WorldContextObject);

	// ═══════════════════════════════════════════════════════════════════════════════
	// ═══ ATTRIBUTE SETS - ACCÈS DIRECT ═════════════════════════════════════════════
	// ═══════════════════════════════════════════════════════════════════════════════
	
	/**
	 * Récupère le GlobalAttributeSet depuis n'importe quel contexte
	 * 
	 * Shortcut pour accéder directement aux attributs globaux (Level, XP)
	 * sans passer par l'AbilitySystemComponent
	 * 
	 * @param WorldContextObject N'importe quel objet du monde
	 * @return Le GlobalAttributeSet ou nullptr si non trouvé
	 */
	UFUNCTION(BlueprintPure, Category = "Gameplay|Global", meta = (WorldContext = "WorldContextObject"))
	static UGlobalAttributeSet* GetGlobalAttributeSet(UObject* WorldContextObject);

	/**
	 * Récupère l'ArchetypeAttributeSet depuis n'importe quel contexte
	 * 
	 * Shortcut pour accéder directement aux attributs de l'archétype (Health, Mana)
	 * sans passer par l'AbilitySystemComponent
	 * 
	 * @param WorldContextObject N'importe quel objet du monde
	 * @return L'ArchetypeAttributeSet ou nullptr si non trouvé
	 */
	UFUNCTION(BlueprintPure, Category = "Gameplay|Archetype", meta = (WorldContext = "WorldContextObject"))
	static UArchetypeAttributeSet* GetArchetypeAttributeSet(UObject* WorldContextObject);
	
};