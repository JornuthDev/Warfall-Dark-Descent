#include "Gameplay/AbilitySystem/DefaultAbilitySystem.h"
#include "Gameplay/AbilitySystem/DefaultAttributeSet.h"
#include "Gameplay/AbilitySystem/Effects/GE_PerkToCriticals.h"
#include "Gameplay/AbilitySystem/Effects/GE_PerkToDamageMultipliers.h"
#include "Gameplay/AbilitySystem/Effects/GE_PerkToMaxAttributes.h"
#include "Gameplay/AbilitySystem/Effects/GE_PerkToRegenRates.h"

// ═══════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════
UDefaultAbilitySystem::UDefaultAbilitySystem()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	
	GE_PerkToMaxAttributes = UGE_PerkToMaxAttributes::StaticClass();
	GE_PerkToRegenRates = UGE_PerkToRegenRates::StaticClass();
	GE_PerkToDamageMultipliers = UGE_PerkToDamageMultipliers::StaticClass();
	GE_PerkToCriticals = UGE_PerkToCriticals::StaticClass();
}

void UDefaultAbilitySystem::Init(AActor* InOwnerActor, AActor* InAvatarActor, const EClassArchetype Archetype,
	const FName RowName)
{
	if (GetOwner()->HasAuthority())
	{
		InitAbilityActorInfo(InOwnerActor, InAvatarActor);
		InitAttributes(Archetype, RowName);
		ApplyPermanentGameplayEffects();
		return;
	}
	InitAbilityActorInfo(InOwnerActor, InAvatarActor);
	Server_Init(InOwnerActor, InAvatarActor, Archetype, RowName);
}

void UDefaultAbilitySystem::InitAttributes(const EClassArchetype Archetype, const FName RowName)
{
	if (GetOwner()->HasAuthority())
	{
		if (UDefaultAttributeSet* DefaultSet = const_cast<UDefaultAttributeSet*>(GetSet<UDefaultAttributeSet>()))
		{
			DefaultSet->Init(Archetype, RowName);
		}
		return;
	}
	Server_InitAttributes(Archetype, RowName);
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Overrides)
// ═══════════════════════════════════════════════════════════════════════

FActiveGameplayEffectHandle UDefaultAbilitySystem::ApplyGameplayEffectSpecToTarget(
	const FGameplayEffectSpec& GameplayEffect, UAbilitySystemComponent* Target, FPredictionKey PredictionKey)
{
	FGameplayEffectSpec ModifiedSpec = GameplayEffect;
    
	if (const UDefaultAttributeSet* DefaultSet = const_cast<UDefaultAttributeSet*>(GetSet<UDefaultAttributeSet>()))
	{
		// Vérifier si le GE a une définition
		if (ModifiedSpec.Def)
		{
			// Parcourir tous les modifiers de la DÉFINITION
			for (const FGameplayModifierInfo& ModifierInfo : ModifiedSpec.Def->Modifiers)
			{
				// 1. Vérifier si l'attribut est ApplyDamages
				if (ModifierInfo.Attribute == UDefaultAttributeSet::GetApplyDamagesAttribute())
				{
					// 2. Vérifier si le type de calcul est SetByCaller
					if (ModifierInfo.ModifierMagnitude.GetMagnitudeCalculationType() == 
						EGameplayEffectMagnitudeCalculation::SetByCaller)
					{
						// 3. Récupérer le Data Tag du SetByCaller
						FGameplayTag DataTag = ModifierInfo.ModifierMagnitude.GetSetByCallerFloat().DataTag;
                        
						// 4. Appliquer l'empowerment sur ce tag spécifique
						if (DataTag.IsValid())
						{
							DefaultSet->EmpoweredMagnitude(ModifiedSpec, DataTag);
						}
					}
				}
			}
		}
	}
    
	return Super::ApplyGameplayEffectSpecToTarget(ModifiedSpec, Target, PredictionKey);
}

void UDefaultAbilitySystem::ApplyPermanentGameplayEffects()
{
	FGameplayEffectContextHandle EffectContext = MakeEffectContext();
	EffectContext.AddSourceObject(this);
	
	auto ApplyPermanentEffect = [this, EffectContext](const TSubclassOf<UGameplayEffect> GameplayEffect) -> void
	{
		if (GameplayEffect)
		{
			const FGameplayEffectSpecHandle Spec = MakeOutgoingSpec(
				GameplayEffect, GetContextLevel(), EffectContext);
			if (Spec.IsValid())
			{
				ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
			}
		}
	};
	ApplyPermanentEffect(GE_PerkToMaxAttributes);
	ApplyPermanentEffect(GE_PerkToRegenRates);
	ApplyPermanentEffect(GE_PerkToDamageMultipliers);
	ApplyPermanentEffect(GE_PerkToCriticals);
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Replication)
// ═══════════════════════════════════════════════════════════════════════

void UDefaultAbilitySystem::Server_Init_Implementation(AActor* InOwnerActor, AActor* InAvatarActor,
	const EClassArchetype Archetype, const FName RowName)
{
	Init(InOwnerActor, InAvatarActor, Archetype, RowName);
}

void UDefaultAbilitySystem::Server_InitAttributes_Implementation(const EClassArchetype Archetype, const FName RowName)
{
	InitAttributes(Archetype, RowName);
}

