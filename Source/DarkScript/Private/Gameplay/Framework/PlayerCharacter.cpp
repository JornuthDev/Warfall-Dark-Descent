#include "Gameplay/Framework/PlayerCharacter.h"

#include "Core/Framework/Base_PlayerState.h"
#include "Gameplay/AbilitySystem/AdvancedAttributeSet.h"
#include "Gameplay/AbilitySystem/DefaultAbilitySystem.h"

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
		if (IsLocallyControlled())
		{
			FTimerHandle Handle;
			GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([this]()
			{
				if (const ABase_PlayerState* PState = Cast<ABase_PlayerState>(GetLocalPlayerState()))
				{
					UE_LOG(DarkScript, Warning, TEXT("PlayerCharacter::PossessedBy - Server Init for %s"), *GetName());
					IncarnatedArchetype = PState->ActifArchetype;
					InitAbilitySystem(PState->ActifArchetype);
				}
			}), 0.2f, false);
		}
	}
}

void APlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	if (!HasAuthority() && GetLocalPlayerState())
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([this]()
		{
			if (const ABase_PlayerState* PState = Cast<ABase_PlayerState>(GetLocalPlayerState()))
			{
				UE_LOG(DarkScript, Warning, TEXT("PlayerCharacter::OnRep_PlayerState - Client Init for %s"), *GetName());
				Server_SetupArchetype(PState->ActifArchetype);
				InitAbilitySystem(PState->ActifArchetype);
			}
		}), 0.2f, false);
	}
}




