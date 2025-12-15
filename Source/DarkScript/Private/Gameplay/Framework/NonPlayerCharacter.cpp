#include "Gameplay/Framework/NonPlayerCharacter.h"

#include "Gameplay/AbilitySystem/DefaultAbilitySystem.h"
#include "Gameplay/AbilitySystem/DefaultAttributeSet.h"

// ═══════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════

ANonPlayerCharacter::ANonPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	DefaultAttributes = CreateDefaultSubobject<UDefaultAttributeSet>("AttributeSet");
	DataRowName = NAME_None;
	
	AbilitySystem->bIsPlayerCharacter = false;
	AbilitySystem->ReplicationMode = EGameplayEffectReplicationMode::Minimal;
}

void ANonPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (NewController)
	{
		InitAbilitySystem(EClassArchetype::None, DataRowName);
	}
}
