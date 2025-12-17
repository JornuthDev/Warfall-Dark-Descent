#include "Core/Framework/Base_PlayerState.h"
#include "Core/Framework/Base_Character.h"
#include "Gameplay/AbilitySystem/DefaultAbilitySystem.h"
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
}

void ABase_PlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			if (!GetPlayerController())
			{
				return;
			}
			if (GetPlayerController()->IsLocalController())
			{
				InitAbilitySystem();
				SaveSystem::Get(GetPlayerController())->RequestLoad(ESaveType::PlayerSave, this, "BeginPlay");
				AbilitySystem->GetGameplayAttributeValueChangeDelegate(UGlobalAttributeSet::GetArchetypeAttribute()).AddLambda([this](const FOnAttributeChangeData& A)
				{
					if (USaveSystem* System = SaveSystem::Get(GetPlayerController()))
					{
						System->RequestSave(ESaveType::PlayerSave, this, "Archetype");
					}
				});
				//DEBUG FOR DEVELOPMENT
				if (IsEditorMode() && AbilitySystem)
				{
					for (const FWorldContext& Context : GEngine->GetWorldContexts())
					{
						if (Context.World() == GetWorld())
						{
							const int32 Index = Context.PIEInstance + 1;
							const EClassArchetype ContextArchetype = static_cast<EClassArchetype>(Index);
							AbilitySystem->SwitchArchetype(ContextArchetype);
							return;
						}
					}
				}
			}
		});
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
		UE_LOG(DarkScript, Warning, TEXT("Base_PlayerState::InitializeGlobalAbilitySystem"));
		if (Attributes)
		{
			AbilitySystem->AddAttributeSetSubobject(Attributes.Get());
		}
		AbilitySystem->InitAbilityActorInfo(this, this);
		
		AbilitySystem->SetNumericAttributeBase(UGlobalAttributeSet::GetArchetypeAttribute(), 1.f);
		AbilitySystem->SetNumericAttributeBase(UGlobalAttributeSet::GetLevelAttribute(), 1.f);
		AbilitySystem->SetNumericAttributeBase(UGlobalAttributeSet::GetCurrentExpAttribute(), 0.f);
		return;
	}
	Server_InitAbiltiySystem();
}

void ABase_PlayerState::ClientInitialize(class AController* C)
{
	Super::ClientInitialize(C);
	
	UE_LOG(DarkScript, Warning, TEXT("ABase_PlayerState::ClientInitialize"));
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
		UE_LOG(LogTemp, Warning, TEXT("ABase_PlayerState::SaveToPlayer_Implementation: Save Archetype to %d"), static_cast<int32>(Attributes->ConvertArchetype()));
		SaveGame->GlobalAbilitySave.Archetype = Attributes->ConvertArchetype();
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

void ABase_PlayerState::Server_LoadFromPlayer_Implementation(const FGlobalAbilitySave& AbilityLoad, const FString& Context)
{
	if (Context == "Archetype")
	{
		AbilitySystem->SetNumericAttributeBase(UGlobalAttributeSet::GetArchetypeAttribute(), static_cast<int32>(AbilityLoad.Archetype));
		return;
	}
	AbilitySystem->SetNumericAttributeBase(UGlobalAttributeSet::GetArchetypeAttribute(), static_cast<int32>(AbilityLoad.Archetype));
	AbilitySystem->SetNumericAttributeBase(UGlobalAttributeSet::GetLevelAttribute(), AbilityLoad.Level);
	AbilitySystem->SetNumericAttributeBase(UGlobalAttributeSet::GetCurrentExpAttribute(), AbilityLoad.CurrentExp);
}


