#include "Utils/Libraries/DarkLibrary.h"
#include "Core/PlayerControllerBase.h"
#include "Core/PlayerStateBase.h"
#include "Gameplay/AbilitySystem/GlobalAbilitySystem.h"
#include "Gameplay/AbilitySystem/GlobalAttributeSet.h"
#include "Gameplay/AbilitySystem/ArchetypeAbilitySystem.h"
#include "Gameplay/AbilitySystem/ArchetypeAttributeSet.h"
#include "Interfaces/SystemsInterface.h"
#include "Kismet/GameplayStatics.h"

// ═══════════════════════════════════════════════════════════════════════════════
// ═══ FUNCTIONS (BLUEPRINT API) ═════════════════════════════════════════════════
// ═══════════════════════════════════════════════════════════════════════════════

// ─── Game ───

APlayerControllerBase* UDarkLibrary::GetPlayerController(const UObject* WorldContextObject, int32 PlayerIndex)
{
	return Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(WorldContextObject, PlayerIndex));
}

// ─── Gameplay / Ability ───

EClassArchetype UDarkLibrary::GetArchetype(const UObject* WorldContextObject)
{
	if (const AController* Controller = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (const APlayerStateBase* PlayerState = Cast<APlayerStateBase>(Controller->PlayerState))
		{
			return PlayerState->CurrentArchetype;
		}
	}
	return EClassArchetype::None;
}

void UDarkLibrary::SetArchetype(UObject* WorldContextObject, const EClassArchetype Archetype)
{
	if (const AController* Controller = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (APlayerStateBase* PlayerState = Cast<APlayerStateBase>(Controller->PlayerState))
		{
			PlayerState->SwitchArchetype(Archetype);
		}
	}
}

int32 UDarkLibrary::GetPlayerLevel(UObject* WorldContextObject)
{
	if (const UGlobalAttributeSet* Attributes = GetGlobalAttributeSet(WorldContextObject))
	{
		return static_cast<int32>(Attributes->GetLevel());
	}
	return INDEX_NONE;
}

FActiveGameplayEffectHandle UDarkLibrary::ApplyDamageToTarget(UObject* WorldContextObject, AActor* Target, const EDamageType Type,
	const float Value)
{
	if (!WorldContextObject || !Target) return FActiveGameplayEffectHandle();
		
	const float FinalDamage = -FMath::Abs(Value);
	/*return UAbilitySystem::ApplyHealthSet(Target, Type, FinalDamage);*/
	return FActiveGameplayEffectHandle();
}

FActiveGameplayEffectHandle UDarkLibrary::ApplyHealToTarget(UObject* WorldContextObject, AActor* Target, const EDamageType Type,
	const float Value)
{
	if (!WorldContextObject || !Target) return FActiveGameplayEffectHandle();
	
	const float FinalHeal = FMath::Abs(Value);
	/*return UAbilitySystem::ApplyHealthSet(Target, Type, FinalHeal);*/
	return FActiveGameplayEffectHandle();
}

// ═══════════════════════════════════════════════════════════════════════════════
// ═══ ABILITY SYSTEMS - ACCÈS RAPIDE ════════════════════════════════════════════
// ═══════════════════════════════════════════════════════════════════════════════

UGlobalAbilitySystem* UDarkLibrary::GetGlobalAbilitySystem(UObject* WorldContextObject)
{
	// Vérifier si le WorldContextObject implémente ISystemsInterface
	if (!WorldContextObject->Implements<USystemsInterface>())
	{
		UE_LOG(LogTemp, Error, TEXT("DarkLibrary: Le PlayerController n'implémente pas ISystemsInterface"));
		return nullptr;
	}

	// Récupérer le GlobalAbilitySystem via l'interface
	UGlobalAbilitySystem* GlobalAbilitySystem = ISystemsInterface::Execute_GetGlobalAbilitySystem(WorldContextObject);
	
	if (!GlobalAbilitySystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("DarkLibrary: GlobalAbilitySystem non trouvé sur le PlayerController"));
	}

	return GlobalAbilitySystem;
}

UArchetypeAbilitySystem* UDarkLibrary::GetArchetypeAbilitySystem(UObject* WorldContextObject)
{
	// Vérifier si le WorldContextObject implémente ISystemsInterface
	if (!WorldContextObject->Implements<USystemsInterface>())
	{
		UE_LOG(LogTemp, Error, TEXT("DarkLibrary: Le PlayerController n'implémente pas ISystemsInterface"));
		return nullptr;
	}

	// Récupérer l'ArchetypeAbilitySystem via l'interface
	UArchetypeAbilitySystem* ArchetypeAbilitySystem = ISystemsInterface::Execute_GetArchetypeAbilitySystem(WorldContextObject);
	
	if (!ArchetypeAbilitySystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("DarkLibrary: ArchetypeAbilitySystem non trouvé sur le PlayerController"));
	}

	return ArchetypeAbilitySystem;
}

// ═══════════════════════════════════════════════════════════════════════════════
// ═══ ATTRIBUTE SETS - ACCÈS DIRECT ═════════════════════════════════════════════
// ═══════════════════════════════════════════════════════════════════════════════

UGlobalAttributeSet* UDarkLibrary::GetGlobalAttributeSet(UObject* WorldContextObject)
{
	// Récupérer le GlobalAbilitySystem
	UGlobalAbilitySystem* GlobalAbilitySystem = GetGlobalAbilitySystem(WorldContextObject);
	if (!GlobalAbilitySystem)
	{
		return nullptr;
	}

	// Récupérer le GlobalAttributeSet depuis l'AbilitySystemComponent
	UGlobalAttributeSet* GlobalAttributeSet = const_cast<UGlobalAttributeSet*>(
		GlobalAbilitySystem->GetSet<UGlobalAttributeSet>()
	);

	if (!GlobalAttributeSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("DarkLibrary: GlobalAttributeSet non trouvé sur le GlobalAbilitySystem"));
	}

	return GlobalAttributeSet;
}

UArchetypeAttributeSet* UDarkLibrary::GetArchetypeAttributeSet(UObject* WorldContextObject)
{
	// Récupérer l'ArchetypeAbilitySystem
	UArchetypeAbilitySystem* ArchetypeAbilitySystem = GetArchetypeAbilitySystem(WorldContextObject);
	if (!ArchetypeAbilitySystem)
	{
		return nullptr;
	}

	// Récupérer l'ArchetypeAttributeSet depuis l'AbilitySystemComponent
	UArchetypeAttributeSet* ArchetypeAttributeSet = const_cast<UArchetypeAttributeSet*>(
		ArchetypeAbilitySystem->GetSet<UArchetypeAttributeSet>()
	);

	if (!ArchetypeAttributeSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("DarkLibrary: ArchetypeAttributeSet non trouvé sur l'ArchetypeAbilitySystem"));
	}

	return ArchetypeAttributeSet;
}