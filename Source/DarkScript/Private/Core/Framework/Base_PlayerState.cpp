#include "Core/Framework/Base_PlayerState.h"
#include "Core/Framework/Base_Character.h"
#include "Net/UnrealNetwork.h"
#include "Gameplay/AbilitySystem/GlobalAbilitySystem.h"
#include "Gameplay/AbilitySystem/GlobalAttributeSet.h"
#include "SaveGame/PlayerSave.h"
#include "Utils/Helpers/SystemsHelper.h"

// ═══════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════

ABase_PlayerState::ABase_PlayerState()
{
	SetNetUpdateFrequency(100.f);
	
	AbilitySystem = CreateDefaultSubobject<UGlobalAbilitySystem>("AbilitySystem");
	Attributes = CreateDefaultSubobject<UGlobalAttributeSet>("Attributes");
	
	ActifArchetype = EClassArchetype::Tank;
}

void ABase_PlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetPlayerController()->IsLocalController())
	{
		InitAbilitySystem();
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
			{
				SaveSystem::Get(GetPlayerController())->RequestLoad(ESaveType::PlayerSave, this, "BeginPlay");
			});
		}
	}
}

void ABase_PlayerState::InitAbilitySystem()
{
	if (!AbilitySystem)
	{
		UE_LOG(DarkScript, Error, TEXT("Base_PlayerState::InitializeGlobalAbilitySystem - AbilitySystem is nullptr"));
		return;
	}
	
	if (HasAuthority())
	{
		if (Attributes)
		{
			AbilitySystem->AddAttributeSetSubobject(Attributes.Get());
		}
		AbilitySystem->InitAbilityActorInfo(this, this);
		
		AbilitySystem->SetNumericAttributeBase(UGlobalAttributeSet::GetLevelAttribute(), 1.f);
		AbilitySystem->SetNumericAttributeBase(UGlobalAttributeSet::GetCurrentExpAttribute(), 0.f);
		return;
	}
	Server_InitAbiltiySystem();
}

void ABase_PlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ABase_PlayerState, ActifArchetype);
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Blueprint API)
// ═══════════════════════════════════════════════════════════════════════

UAbilitySystemComponent* ABase_PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystem.Get();
}

UGlobalAbilitySystem* ABase_PlayerState::GetGlobalAbilitySystem_Implementation()
{
	return AbilitySystem.Get();
}

UDefaultAbilitySystem* ABase_PlayerState::GetDefaultAbilitySystem_Implementation()
{
	if (GetPlayerController())
	{
		ACharacter* Chr = GetPlayerController()->GetCharacter();
		if (!Chr) return nullptr;
		
		return DefaultAbilitySystem(Chr);
	}
	return nullptr;
}

bool ABase_PlayerState::SaveToPlayer_Implementation(UPlayerSave* SaveGame, const FString& Context)
{
	if (!SaveGame || !AbilitySystem) return false;
	
	if (Context == "Archetype")
	{
		SaveGame->GlobalAbilitySave.Archetype = ActifArchetype;
		return true;
	}
	SaveGame->GlobalAbilitySave.Level = static_cast<int32>(Attributes->GetLevel());
	SaveGame->GlobalAbilitySave.CurrentExp = Attributes->GetCurrentExp();
	
	return true;
}

bool ABase_PlayerState::LoadFromPlayer_Implementation(UPlayerSave* SaveGame, const FString& Context)
{
	if (!SaveGame || !AbilitySystem) return false;
	Server_LoadFromPlayer(SaveGame->GlobalAbilitySave, Context);
	
	return true;
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Replicates)
// ═══════════════════════════════════════════════════════════════════════

void ABase_PlayerState::Server_InitAbiltiySystem_Implementation()
{
	InitAbilitySystem();
}

void ABase_PlayerState::OnRep_ActifArchetype()
{
	
}

void ABase_PlayerState::Server_LoadFromPlayer_Implementation(const FGlobalAbilitySave& AbilityLoad, const FString& Context)
{
	if (Context == "Archetype")
	{
		ActifArchetype = AbilityLoad.Archetype;
		return;
	}
	ActifArchetype = AbilityLoad.Archetype;
	AbilitySystem->SetNumericAttributeBase(UGlobalAttributeSet::GetLevelAttribute(), AbilityLoad.Level);
	AbilitySystem->SetNumericAttributeBase(UGlobalAttributeSet::GetCurrentExpAttribute(), AbilityLoad.CurrentExp);
}


