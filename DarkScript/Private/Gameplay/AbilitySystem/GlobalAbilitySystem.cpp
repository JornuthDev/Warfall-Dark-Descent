#include "Gameplay/AbilitySystem/GlobalAbilitySystem.h"

#include "GameFramework/PlayerState.h"
#include "Gameplay/AbilitySystem/DefaultAbilitySystem.h"
#include "Gameplay/AbilitySystem/GlobalAttributeSet.h"
#include "Utils/Log.h"

// ═══════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════

UGlobalAbilitySystem::UGlobalAbilitySystem()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	ReplicationMode = EGameplayEffectReplicationMode::Full;
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Blueprint API)
// ═══════════════════════════════════════════════════════════════════════

void UGlobalAbilitySystem::SwitchArchetype(const EClassArchetype NewArchetype)
{
	const UGlobalAttributeSet* GlobalAttr = const_cast<UGlobalAttributeSet*>(GetSet<UGlobalAttributeSet>());
	if (!GlobalAttr)
	{
		UE_LOG(DarkScript, Error, TEXT("SwitchArchetype:: GlobalAttr is not valid"));
		return;
	}
	if (NewArchetype == GlobalAttr->ConvertArchetype())
	{
		UE_LOG(DarkScript, Error, TEXT("SwitchArchetype:: Archetype is same (New = %s / Current = %s"), *UEnum::GetValueAsString(NewArchetype), *UEnum::GetValueAsString(GlobalAttr->ConvertArchetype()));
		return;
	}
	if (NewArchetype == EClassArchetype::None)
	{
		UE_LOG(DarkScript, Error, TEXT("SwitchArchetype:: Archetype is equal (None)"));
		return;
	}
	
	if (!GetOwnerActor())
	{
		UE_LOG(DarkScript, Error, TEXT("SwitchArchetype:: GetOwnerActor() is not valid"));
		return;
	}
	
	if (GetOwnerActor()->HasAuthority())
	{
		SetClassArchetype(NewArchetype);
		
		if (GetOwnerActor()->GetClass()->ImplementsInterface(USystemsInterface::StaticClass()))
		{
			if (UDefaultAbilitySystem* System = ISystemsInterface::Execute_GetDefaultAbilitySystem(GetOwnerActor()))
			{
				System->InitAttributes(NewArchetype, NAME_None);
			}
		}
	}
	else
	{
		Server_SwitchArchetype(NewArchetype);
	}
}

EClassArchetype UGlobalAbilitySystem::GetClassArchetype() const
{
	const UGlobalAttributeSet* GlobalAttr = const_cast<UGlobalAttributeSet*>(GetSet<UGlobalAttributeSet>());
	if (!GlobalAttr) return EClassArchetype::Tank;
	
	return GlobalAttr->ConvertArchetype();
}

void UGlobalAbilitySystem::SetClassArchetype(const EClassArchetype NewArchetype)
{
	if (const UGlobalAttributeSet* GlobalAttr = const_cast<UGlobalAttributeSet*>(GetSet<UGlobalAttributeSet>()); !GlobalAttr) return;
	
	SetNumericAttributeBase(UGlobalAttributeSet::GetArchetypeAttribute(), static_cast<int32>(NewArchetype));
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Replication)
// ═══════════════════════════════════════════════════════════════════════

void UGlobalAbilitySystem::Server_SwitchArchetype_Implementation(const EClassArchetype NewArchetype)
{
	SwitchArchetype(NewArchetype);
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Statics)
// ═══════════════════════════════════════════════════════════════════════

UGlobalAbilitySystem* UGlobalAbilitySystem::GetSystem(UObject* WorldContextObject)
{
	if (!WorldContextObject)
		return nullptr;

	const AController* PC;
	APlayerState* PS;
	UGlobalAbilitySystem* GlobalSystem = nullptr;
	if (ACharacter* Character = Cast<ACharacter>(WorldContextObject))
	{
		if (Character->GetClass()->ImplementsInterface(USystemsInterface::StaticClass()))
		{
			GlobalSystem = ISystemsInterface::Execute_GetGlobalAbilitySystem(Character);
		}
		if (GlobalSystem)
		{
			return GlobalSystem;
		}
		PC = Character->GetController();
	}
	else
	{
		PC = Cast<AController>(WorldContextObject);
	}

	if (PC && PC->PlayerState)
	{
		PS = PC->PlayerState.Get();
	}
	else
	{
		PS = Cast<APlayerState>(WorldContextObject);
	}

	if (PS && PS->GetClass()->ImplementsInterface(USystemsInterface::StaticClass()))
	{
		GlobalSystem = ISystemsInterface::Execute_GetGlobalAbilitySystem(PS);
		if (GlobalSystem)
		{
			return GlobalSystem;
		}
	}
	return nullptr;
}
