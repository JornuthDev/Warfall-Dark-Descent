// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/GlobalAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Core/PlayerStateBase.h"
#include "Utils/Helpers/SystemsHelpers.h"

// ═══════════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════════

UGlobalAttributeSet::UGlobalAttributeSet()
{
	// Initialiser les valeurs par défaut
	Level = 1.0f;
	CurrentExp = 0.0f;
}

AController* UGlobalAttributeSet::GetController() const
{
	if (GetOwningAbilitySystemComponent())
	{
		if (const APlayerStateBase* PState = Cast<APlayerStateBase>(GetOwningAbilitySystemComponent()->GetOwner()))
		{
			return PState->GetOwningController();
		}
	}
	return nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════
// RÉPLICATION
// ═══════════════════════════════════════════════════════════════════════════

void UGlobalAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Répliquer Level à tous les clients, toujours notifier
	DOREPLIFETIME_CONDITION_NOTIFY(UGlobalAttributeSet, Level, COND_None, REPNOTIFY_Always);
	
	// Répliquer CurrentExp à tous les clients, toujours notifier
	DOREPLIFETIME_CONDITION_NOTIFY(UGlobalAttributeSet, CurrentExp, COND_None, REPNOTIFY_Always);
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLBACKS DE RÉPLICATION (Côté CLIENT uniquement)
// ═══════════════════════════════════════════════════════════════════════════

void UGlobalAttributeSet::OnRep_Level(const FGameplayAttributeData& OldLevel)
{
	// Macro GAS pour gérer la réplication correctement
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGlobalAttributeSet, Level, OldLevel);
	
	// ───────────────────────────────────────────────────────────────────────
	// SAUVEGARDE CÔTÉ CLIENT (quand la réplication arrive)
	// ───────────────────────────────────────────────────────────────────────
	if (GetController() && GetController()->PlayerState)
	{
		if (USaveSystem* System = GetSaveSystem(GetController()))
		{
			System->SaveGame(ESaveType::PlayerSave, GetController()->PlayerState.Get(), "OnRep_Level");
		}
	}
}

void UGlobalAttributeSet::OnRep_CurrentExp(const FGameplayAttributeData& OldCurrentExp)
{
	// Macro GAS pour gérer la réplication correctement
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGlobalAttributeSet, CurrentExp, OldCurrentExp);
	
	// ───────────────────────────────────────────────────────────────────────
	// SAUVEGARDE CÔTÉ CLIENT (quand la réplication arrive)
	// ───────────────────────────────────────────────────────────────────────
	if (GetController() && GetController()->PlayerState)
	{
		if (USaveSystem* System = GetSaveSystem(GetController()))
		{
			System->SaveGame(ESaveType::PlayerSave, GetController()->PlayerState.Get(), "OnRep_CurrentExp");
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// POST GAMEPLAY EFFECT EXECUTE (Côté SERVEUR uniquement)
// ═══════════════════════════════════════════════════════════════════════════

void UGlobalAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// Cette fonction est appelée UNIQUEMENT côté serveur après modification d'un attribut
	// C'est ici qu'on sauvegarde côté serveur
	
	FGameplayAttribute ModifiedAttribute = Data.EvaluatedData.Attribute;

	// ───────────────────────────────────────────────────────────────────────
	// SAUVEGARDE CÔTÉ SERVEUR (quand un attribut est modifié)
	// ───────────────────────────────────────────────────────────────────────
	
	if (ModifiedAttribute == GetLevelAttribute() || ModifiedAttribute == GetCurrentExpAttribute())
	{
		if (GetController() && GetController()->PlayerState)
		{
			if (USaveSystem* System = GetSaveSystem(GetController()))
			{
				// Sauvegarde côté serveur après modification
				System->SaveGame(ESaveType::PlayerSave, GetController()->PlayerState.Get(), 
					FString::Printf(TEXT("PostGE_%s"), *ModifiedAttribute.GetName()));
			}
		}
	}
}