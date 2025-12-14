#pragma once
#include "CoreMinimal.h"
#include "Core/Systems/SaveSystem.h"

inline USaveSystem* GetSaveSystem(const AController* Controller)
{
	if (!Controller || !Controller->GetGameInstance()) return nullptr;
	if (!Controller->IsLocalController()) return nullptr;
	return Controller->GetGameInstance()->GetSubsystem<USaveSystem>();
}

inline int32 CalculatePerkPoints(const int32 Level, const float PerkWeight)
{
	if (PerkWeight <= 0 || Level <= 0)
	{
		return 1;
	}

	const float Result = static_cast<float>(Level) / PerkWeight;
	return FMath::FloorToInt(Result);
}

inline FGameplayEffectAttributeCaptureDefinition GetCaptureDefinition(const FGameplayAttribute& Attribute)
{
	FGameplayEffectAttributeCaptureDefinition Def;
	Def.AttributeToCapture = Attribute;
	return Def;
}
