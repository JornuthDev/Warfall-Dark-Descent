// Copyright Dark Script - All Rights Reserved

#include "Gameplay/AbilitySystem/GlobalAbilitySystem.h"
#include "Gameplay/AbilitySystem/GlobalAttributeSet.h"

// ═══════════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════════

UGlobalAbilitySystem::UGlobalAbilitySystem()
{
	// Configurer la réplication
	SetIsReplicatedByDefault(true);
	
	// Mode de réplication : Full (répliquer tous les GameplayEffects)
	// Alternative : Mixed (optimisé pour le multijoueur, à tester selon les besoins)
	ReplicationMode = EGameplayEffectReplicationMode::Full;
}

// ═══════════════════════════════════════════════════════════════════════════
// BEGIN PLAY
// ═══════════════════════════════════════════════════════════════════════════

void UGlobalAbilitySystem::BeginPlay()
{
	Super::BeginPlay();

	// Log pour vérifier que le component est bien créé
	UE_LOG(LogTemp, Log, TEXT("GlobalAbilitySystem: BeginPlay appelé sur %s"), 
		*GetOwner()->GetName());
}

// ═══════════════════════════════════════════════════════════════════════════
// MÉTHODES CUSTOM (à implémenter en Phase 8+)
// ═══════════════════════════════════════════════════════════════════════════

/*
float UGlobalAbilitySystem::GetLevelWithBonuses() const
{
	// Récupérer le Level de base
	float BaseLevel = GetNumericAttribute(UGlobalAttributeSet::GetLevelAttribute());
	
	// TODO : Ajouter les bonus de Level (équipements, buffs temporaires, etc.)
	// float BonusLevel = CalculateLevelBonuses();
	
	return BaseLevel; // + BonusLevel;
}

void UGlobalAbilitySystem::AddExperience(int32 Amount)
{
	if (Amount <= 0) return;
	
	// Récupérer l'XP actuelle
	float CurrentXP = GetNumericAttribute(UGlobalAttributeSet::GetCurrentExpAttribute());
	
	// Ajouter l'XP via un GameplayEffect
	// TODO : Créer un GE_AddExperience et l'appliquer
	
	// Vérifier si on peut level up
	if (CanLevelUp())
	{
		// Gérer le level up
		// TODO : Créer un GE_LevelUp et l'appliquer
	}
}

bool UGlobalAbilitySystem::CanLevelUp() const
{
	// TODO : Récupérer le seuil d'XP requis pour le niveau actuel
	// Comparer avec CurrentExp
	return false;
}
*/