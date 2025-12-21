// DynamicGameplayEffect.cpp
#include "Gameplay/AbilitySystem/Effects/DynamicGameplayEffect.h"
#include "Gameplay/AbilitySystem/DynamicEffectConfig.h"
#include "DarkGameplayTags.h"

UDynamicGameplayEffect* UDynamicGameplayEffect::CreateDynamicEffect(
	const UObject* WorldContextObject,
	const FDynamicEffectConfig& Config)
{
	if (!WorldContextObject)
	{
		UE_LOG(LogTemp, Error, TEXT("CreateDynamicEffect: Invalid WorldContextObject"));
		return nullptr;
	}

	// ✅ SOLUTION : Créer dans le package du monde
	// Cela permet la réplication car l'objet existe dans un package valide
	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("CreateDynamicEffect: Cannot get World"));
		return nullptr;
	}

	// Créer un nom unique
	FString EffectName = Config.Identifier.IsEmpty() 
		? FString::Printf(TEXT("DynGE_%lld"), FPlatformTime::Cycles64())
		: Config.Identifier;

	// ✅ Créer l'effet dans le package du monde avec RF_Transient
	// RF_Transient = ne sera pas sauvegardé, mais PEUT être répliqué
	UDynamicGameplayEffect* NewEffect = NewObject<UDynamicGameplayEffect>(
		World,
		UDynamicGameplayEffect::StaticClass(),
		FName(*EffectName),
		RF_NoFlags  // Pas de flags spéciaux, objet normal réplicable
	);

	if (NewEffect)
	{
		NewEffect->EffectIdentifier = EffectName;
		NewEffect->ConfigureFromStruct(Config);
	}

	return NewEffect;
}

void UDynamicGameplayEffect::ConfigureFromStruct(const FDynamicEffectConfig& Config)
{
	// ═══════════════════════════════════════════════════════════════════
	// POLITIQUE DE DURÉE
	// ═══════════════════════════════════════════════════════════════════
	DurationPolicy = Config.DurationPolicy;

	if (Config.bUseDurationSetByCaller && Config.DurationSetByCallerTag.IsValid())
	{
		// Utiliser SetByCaller pour la durée
		FSetByCallerFloat SetByCallerDuration;
		SetByCallerDuration.DataTag = Config.DurationSetByCallerTag;
		DurationMagnitude = FGameplayEffectModifierMagnitude(SetByCallerDuration);
	}
	else if (DurationPolicy == EGameplayEffectDurationType::HasDuration)
	{
		// Durée scalaire fixe
		FScalableFloat ScalableDuration(Config.Duration);
		DurationMagnitude = FGameplayEffectModifierMagnitude(ScalableDuration);
	}

	// ═══════════════════════════════════════════════════════════════════
	// MODIFIERS
	// ═══════════════════════════════════════════════════════════════════
	Modifiers.Empty();
	Modifiers.Reserve(Config.Modifiers.Num());

	for (const FDynamicModifierConfig& ModConfig : Config.Modifiers)
	{
		if (!ModConfig.Attribute.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("ConfigureFromStruct: Invalid Attribute in modifier"));
			continue;
		}

		FGameplayModifierInfo ModInfo;
		ModInfo.Attribute = ModConfig.Attribute;
		ModInfo.ModifierOp = ModConfig.ModifierOp;

		// Déterminer la magnitude
		if (ModConfig.CalculationClass)
		{
			// ✅ Utiliser une classe de calcul
			FCustomCalculationBasedFloat CalcBased;
			CalcBased.CalculationClassMagnitude = ModConfig.CalculationClass;
			ModInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(CalcBased);
		}
		else if (ModConfig.bUseSetByCaller && ModConfig.SetByCallerTag.IsValid())
		{
			// ✅ Utiliser SetByCaller
			FSetByCallerFloat SetByCaller;
			SetByCaller.DataTag = ModConfig.SetByCallerTag;
			ModInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);
		}
		else
		{
			// ✅ Magnitude scalaire fixe
			FScalableFloat ScalableMag(ModConfig.Magnitude);
			ModInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(ScalableMag);
		}

		Modifiers.Add(ModInfo);
	}

	// ═══════════════════════════════════════════════════════════════════
	// TAGS
	// ═══════════════════════════════════════════════════════════════════
	if (!Config.GrantedTags.IsEmpty())
	{
		CachedAssetTags = Config.GrantedTags;
	}

	if (!Config.ApplicationRequiredTags.IsEmpty())
	{
		CachedBlockedAbilityTags = Config.ApplicationRequiredTags;
	}

	// ═══════════════════════════════════════════════════════════════════
	// EFFET PÉRIODIQUE
	// ═══════════════════════════════════════════════════════════════════
	if (Config.Period > 0.f)
	{
		FScalableFloat ScalablePeriod(Config.Period);
		Period = ScalablePeriod;
		bExecutePeriodicEffectOnApplication = Config.bExecutePeriodicEffectOnApplication;
	}
}