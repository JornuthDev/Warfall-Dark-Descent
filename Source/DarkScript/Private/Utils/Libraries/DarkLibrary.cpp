#include "Utils/Libraries/DarkLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/Progression.h"
#include "Core/PlayerControllerBase.h"

#include "Kismet/GameplayStatics.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
APlayerControllerBase* UDarkLibrary::GetPlayerController(const UObject* WorldContextObject, int32 PlayerIndex)
{
	return Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(WorldContextObject, PlayerIndex));
}

EClassArchetype UDarkLibrary::GetArchetype(const UObject* WorldContextObject)
{
	if (GetProgressionComponent(WorldContextObject))
	{
		return GetProgressionComponent(WorldContextObject)->GetClassArchetype();
	}
	return EClassArchetype::None;
}

int32 UDarkLibrary::GetPlayerLevel(const UObject* WorldContextObject)
{
	if (GetProgressionComponent(WorldContextObject))
	{
		return GetProgressionComponent(WorldContextObject)->GetPlayerLevel();
	}
	return INDEX_NONE;
}

void UDarkLibrary::SetArchetype(const UObject* WorldContextObject, const EClassArchetype Archetype)
{
	if (GetProgressionComponent(WorldContextObject))
	{
		GetProgressionComponent(WorldContextObject)->SetClassArchetype(Archetype);
	}
}

void UDarkLibrary::SetPlayerLevel(const UObject* WorldContextObject, const int32 PlayerLevel)
{
	if (GetProgressionComponent(WorldContextObject))
	{
		GetProgressionComponent(WorldContextObject)->SetPlayerLevel(PlayerLevel);
	}
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

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
UProgression* UDarkLibrary::GetProgressionComponent(const UObject* WorldContextObject)
{
	if (const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		return Cast<UProgression>(PlayerController->GetComponentByClass(UProgression::StaticClass()));
	}
	return nullptr;
}
