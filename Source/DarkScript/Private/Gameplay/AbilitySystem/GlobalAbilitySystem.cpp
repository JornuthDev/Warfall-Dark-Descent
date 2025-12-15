#include "Gameplay/AbilitySystem/GlobalAbilitySystem.h"

UGlobalAbilitySystem::UGlobalAbilitySystem()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	ReplicationMode = EGameplayEffectReplicationMode::Full;
}
