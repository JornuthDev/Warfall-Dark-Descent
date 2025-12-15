#include "Interfaces/SystemsInterface.h"

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Blueprint API)
// ═══════════════════════════════════════════════════════════════════════

UGlobalAbilitySystem* ISystemsInterface::GetGlobalAbilitySystem_Implementation()
{
	return nullptr;
}

UDefaultAbilitySystem* ISystemsInterface::GetDefaultAbilitySystem_Implementation()
{
	return nullptr;
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Statics)
// ═══════════════════════════════════════════════════════════════════════

UGlobalAbilitySystem* ISystemsInterface::GlobalAbilitySystem(UObject* Context)
{
	if (!Context) return nullptr;
	
	if (!Context->GetClass()->ImplementsInterface(USystemsInterface::StaticClass())) return nullptr;
	
	return Execute_GetGlobalAbilitySystem(Context);
}

UDefaultAbilitySystem* ISystemsInterface::DefaultAbilitySystem(UObject* Context)
{
	if (!Context) return nullptr;
	
	if (!Context->GetClass()->ImplementsInterface(USystemsInterface::StaticClass())) return nullptr;
	
	return Execute_GetDefaultAbilitySystem(Context);
}
