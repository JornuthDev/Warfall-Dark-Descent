#include "Gameplay/Framework/PlayerCharacter.h"

#include "Core/Framework/Base_PlayerState.h"
#include "Gameplay/AbilitySystem/AdvancedAttributeSet.h"
#include "Gameplay/AbilitySystem/DefaultAbilitySystem.h"
#include "Gameplay/AbilitySystem/GlobalAbilitySystem.h"
#include "Gameplay/AbilitySystem/GlobalAttributeSet.h"
#include "Net/UnrealNetwork.h"

// ═══════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	DefaultAttributes = CreateDefaultSubobject<UAdvancedAttributeSet>(FName("AttributeSet"));
	IncarnatedArchetype = EClassArchetype::Tank;
	
	AbilitySystem->bIsPlayerCharacter = true;
	AbilitySystem->ReplicationMode = EGameplayEffectReplicationMode::Full;
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (HasAuthority())
	{
		UGlobalAbilitySystem* System = Execute_GetGlobalAbilitySystem(this);
		if (!System) return;
		
		FTimerHandle Handle;
		TWeakObjectPtr<APlayerCharacter> WeakThis = this;
		TWeakObjectPtr<UGlobalAbilitySystem> WeakSystem = System;
		
		GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([WeakThis, WeakSystem]()
		{
			if (!WeakThis.IsValid() || !WeakSystem.IsValid())
			{
				return;
			}
			
			APlayerCharacter* This = WeakThis.Get();
			UGlobalAbilitySystem* System = WeakSystem.Get();
			
			if (This->IsLocallyControlled())
			{
				if (Cast<ABase_PlayerState>(This->GetLocalPlayerState()))
				{
					UE_LOG(DarkScript, Warning, TEXT("PlayerCharacter::PossessedBy - Server Init for %s"), *This->GetName());
					This->IncarnatedArchetype = System->GetClassArchetype();
					This->InitAbilitySystem(This->IncarnatedArchetype, NAME_None);
				}
			}
			System->GetGameplayAttributeValueChangeDelegate(UGlobalAttributeSet::GetArchetypeAttribute()).AddUObject(This, &APlayerCharacter::Handle_ArchetypeChanged);
		}),0.1f, false);
		
	}
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APlayerCharacter, IncarnatedArchetype);
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Helpers)
// ═══════════════════════════════════════════════════════════════════════

void APlayerCharacter::Handle_ArchetypeChanged(const FOnAttributeChangeData& ChangeData)
{
	const UGlobalAbilitySystem* System = Execute_GetGlobalAbilitySystem(this);
	if (!System) return;
	IncarnatedArchetype = System->GetClassArchetype();
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Replication)
// ═══════════════════════════════════════════════════════════════════════

void APlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	if (!HasAuthority() && GetLocalPlayerState())
	{
		UGlobalAbilitySystem* System = Execute_GetGlobalAbilitySystem(this);
		if (!System) return;
		
		if (IsLocallyControlled())
		{
			FTimerHandle Handle;
			TWeakObjectPtr<APlayerCharacter> WeakThis = this;
			TWeakObjectPtr<UGlobalAbilitySystem> WeakSystem = System;
			
			GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([WeakThis, WeakSystem]()
			{
				if (!WeakThis.IsValid() || !WeakSystem.IsValid())
				{
					return;
				}
				
				APlayerCharacter* This = WeakThis.Get();
				UGlobalAbilitySystem* System = WeakSystem.Get();
				
				if (Cast<ABase_PlayerState>(This->GetLocalPlayerState()))
				{
					UE_LOG(DarkScript, Warning, TEXT("PlayerCharacter::OnRep_PlayerState - Client Init for %s"), *This->GetName());
					This->IncarnatedArchetype = System->GetClassArchetype();
					This->InitAbilitySystem(This->IncarnatedArchetype, NAME_None);
				}
			}), 0.1f, false);
		}
	}
}

void APlayerCharacter::OnRep_IncarnatedArchetype()
{
}




