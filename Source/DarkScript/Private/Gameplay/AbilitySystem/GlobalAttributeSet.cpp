#include "Gameplay/AbilitySystem/GlobalAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Core/Framework/Base_PlayerState.h"
#include "Core/Systems/SaveSystem.h"
#include "Net/UnrealNetwork.h"
#include "Utils/Helpers/SystemsHelper.h"

// ═══════════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════════

UGlobalAttributeSet::UGlobalAttributeSet()
{
	// Initialiser les valeurs par défaut
	Level = 1.0f;
	CurrentExp = 0.0f;
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
	Save("OnRep_Level");
}

void UGlobalAttributeSet::OnRep_CurrentExp(const FGameplayAttributeData& OldCurrentExp)
{
	// Macro GAS pour gérer la réplication correctement
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGlobalAttributeSet, CurrentExp, OldCurrentExp);
	
	// ───────────────────────────────────────────────────────────────────────
	// SAUVEGARDE CÔTÉ CLIENT (quand la réplication arrive)
	// ───────────────────────────────────────────────────────────────────────
	Save("OnRep_CurrentExp");
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
	Save("PostGameplayEffectExecute");
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Helpers)
// ═══════════════════════════════════════════════════════════════════════

void UGlobalAttributeSet::Save(const FString& Context) const
{
	if (GetPlayerController() && GetPlayerController()->PlayerState)
	{
		if (USaveSystem* System = SaveSystem::Get(GetPlayerController()))
		{
			System->RequestSave(ESaveType::PlayerSave, GetPlayerController()->PlayerState.Get(), Context);
		}
	}
}

APlayerController* UGlobalAttributeSet::GetPlayerController() const
{
	if (GetOwningAbilitySystemComponent())
	{
		if (const ABase_PlayerState* PlayerState = Cast<ABase_PlayerState>(GetOwningAbilitySystemComponent()->GetOwner()))
		{
			return PlayerState->GetPlayerController();
		}
	}
	return nullptr;
}
