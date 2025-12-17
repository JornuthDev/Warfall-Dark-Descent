#include "Core/Framework/Base_Character.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/AbilitySystem/DefaultAbilitySystem.h"
#include "Gameplay/AbilitySystem/DefaultAttributeSet.h"
#include "Utils/Log.h"

// ═══════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════

ABase_Character::ABase_Character()
{
	PrimaryActorTick.bCanEverTick = true;
	
	AbilitySystem = CreateDefaultSubobject<UDefaultAbilitySystem>(TEXT("AbilitySystem"));
}

void ABase_Character::BeginPlay()
{
	Super::BeginPlay();
}

void ABase_Character::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void ABase_Character::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Blueprint API)
// ═══════════════════════════════════════════════════════════════════════

UAbilitySystemComponent* ABase_Character::GetAbilitySystemComponent() const
{
	return AbilitySystem.Get();
}

UGlobalAbilitySystem* ABase_Character::GetGlobalAbilitySystem_Implementation()
{
	APlayerState* PState = GetPlayerState();
	if (!PState) return nullptr;
	
	return GlobalAbilitySystem(PState);
}

UDefaultAbilitySystem* ABase_Character::GetDefaultAbilitySystem_Implementation()
{
	return AbilitySystem.Get();
}

void ABase_Character::InitAbilitySystem(const EClassArchetype Archetype, const FName RowName)
{
	if (!AbilitySystem)
	{
		UE_LOG(DarkScript, Error, TEXT("Base_Character::InitAbilitySystem - AbilitySystem is nullptr"));
		return;
	}
	if (DefaultAttributes)
	{
		AbilitySystem->AddAttributeSetSubobject(DefaultAttributes.Get());
	}
	AbilitySystem->Init(this, this, Archetype, RowName);
}
