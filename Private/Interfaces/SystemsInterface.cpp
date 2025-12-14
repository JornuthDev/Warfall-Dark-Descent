// Copyright Dark Script - All Rights Reserved

#include "Interfaces/SystemsInterface.h"
#include "Gameplay/AbilitySystem/GlobalAbilitySystem.h"
#include "Gameplay/AbilitySystem/ArchetypeAbilitySystem.h"

// ═══════════════════════════════════════════════════════════════════════════
// IMPLÉMENTATION PAR DÉFAUT : GetGlobalAbilitySystem
// ═══════════════════════════════════════════════════════════════════════════

UGlobalAbilitySystem* ISystemsInterface::GetGlobalAbilitySystem_Implementation()
{
	// Implémentation par défaut : retourner nullptr
	// Les classes qui implémentent cette interface doivent override cette méthode
	return nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════
// IMPLÉMENTATION PAR DÉFAUT : GetArchetypeAbilitySystem
// ═══════════════════════════════════════════════════════════════════════════

UArchetypeAbilitySystem* ISystemsInterface::GetArchetypeAbilitySystem_Implementation()
{
	// Implémentation par défaut : retourner nullptr
	// Les classes qui implémentent cette interface doivent override cette méthode
	return nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════
// HELPER STATIQUE : GetGlobalAbilitySystemFromObject
// ═══════════════════════════════════════════════════════════════════════════

UGlobalAbilitySystem* ISystemsInterface::GetGlobalAbilitySystemFromObject(UObject* Object)
{
	// Vérifier que l'objet est valide
	if (!Object)
	{
		return nullptr;
	}

	// Vérifier si l'objet implémente l'interface
	if (!Object->Implements<USystemsInterface>())
	{
		return nullptr;
	}

	// Appeler la méthode de l'interface (retourne le type spécifique)
	return ISystemsInterface::Execute_GetGlobalAbilitySystem(Object);
}

// ═══════════════════════════════════════════════════════════════════════════
// HELPER STATIQUE : GetArchetypeAbilitySystemFromObject
// ═══════════════════════════════════════════════════════════════════════════

UArchetypeAbilitySystem* ISystemsInterface::GetArchetypeAbilitySystemFromObject(UObject* Object)
{
	// Vérifier que l'objet est valide
	if (!Object)
	{
		return nullptr;
	}

	// Vérifier si l'objet implémente l'interface
	if (!Object->Implements<USystemsInterface>())
	{
		return nullptr;
	}

	// Appeler la méthode de l'interface (retourne le type spécifique)
	return ISystemsInterface::Execute_GetArchetypeAbilitySystem(Object);
}