// Copyright Dark Script - All Rights Reserved

#include "Core/PlayerStateBase.h"
#include "Gameplay/AbilitySystem/GlobalAbilitySystem.h"
#include "Gameplay/AbilitySystem/GlobalAttributeSet.h"
#include "Gameplay/AbilitySystem/ArchetypeAbilitySystem.h"
#include "SaveGame/PlayerSaveGame.h"
#include "Net/UnrealNetwork.h"
#include "Utils/Helpers/SystemsHelpers.h"

// ═══════════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════════

APlayerStateBase::APlayerStateBase()
{
	// ───────────────────────────────────────────────────────────────────────
	// Configurer la fréquence de réplication
	// ───────────────────────────────────────────────────────────────────────
	SetNetUpdateFrequency(100.0f);

	// ───────────────────────────────────────────────────────────────────────
	// Créer le GlobalAbilitySystem
	// ───────────────────────────────────────────────────────────────────────
	GlobalAbilitySystem = CreateDefaultSubobject<UGlobalAbilitySystem>(TEXT("GlobalAbilitySystem"));
	GlobalAbilitySystem->SetIsReplicated(true);
	GlobalAbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Full);

	// ───────────────────────────────────────────────────────────────────────
	// Créer le GlobalAttributeSet
	// ───────────────────────────────────────────────────────────────────────
	GlobalAttributeSet = CreateDefaultSubobject<UGlobalAttributeSet>(TEXT("GlobalAttributeSet"));

	// ───────────────────────────────────────────────────────────────────────
	// Archétype par défaut
	// ───────────────────────────────────────────────────────────────────────
	CurrentArchetype = EClassArchetype::Tank;
}

// ═══════════════════════════════════════════════════════════════════════════
// BEGIN PLAY
// ═══════════════════════════════════════════════════════════════════════════

void APlayerStateBase::BeginPlay()
{
	Super::BeginPlay();

	// Initialiser le GlobalAbilitySystem sur le serveur uniquement
	if (HasAuthority())
	{
		InitializeGlobalAbilitySystem();
	}
	else
	{
		if (USaveSystem* System = GetSaveSystem(GetOwningController()))
		{
			System->LoadGame(ESaveType::PlayerSave, this, "BeginPlay Loading");
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// RÉPLICATION
// ═══════════════════════════════════════════════════════════════════════════

void APlayerStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Répliquer l'archétype actuel
	DOREPLIFETIME(APlayerStateBase, CurrentArchetype);
}

void APlayerStateBase::OnRep_CurrentArchetype()
{
	// Log pour debug
	UE_LOG(LogTemp, Log, TEXT("PlayerStateBase: CurrentArchetype répliqué -> %d"), 
		static_cast<int32>(CurrentArchetype));
	
	// ───────────────────────────────────────────────────────────────────────
	// SAUVEGARDE CÔTÉ CLIENT (quand la réplication arrive)
	// ───────────────────────────────────────────────────────────────────────
	if (USaveSystem* System = GetSaveSystem(GetOwningController()))
	{
		System->SaveGame(ESaveType::PlayerSave, this, "OnRep_CurrentArchetype");
	}
	
	// TODO PHASE 9+ : Notifier les widgets UI du changement d'archétype
}

// ═══════════════════════════════════════════════════════════════════════════
// INTERFACE : IAbilitySystemInterface
// ═══════════════════════════════════════════════════════════════════════════

UAbilitySystemComponent* APlayerStateBase::GetAbilitySystemComponent() const
{
	// Pour le PlayerState, l'ASC principal est le GlobalAbilitySystem
	return GlobalAbilitySystem;
}

// ═══════════════════════════════════════════════════════════════════════════
// INTERFACE : ISystemsInterface
// ═══════════════════════════════════════════════════════════════════════════

UGlobalAbilitySystem* APlayerStateBase::GetGlobalAbilitySystem_Implementation()
{
	// Le PlayerState possède le GlobalAbilitySystem
	return GlobalAbilitySystem;
}

UArchetypeAbilitySystem* APlayerStateBase::GetArchetypeAbilitySystem_Implementation()
{
	// Le PlayerState ne possède PAS l'ArchetypeAbilitySystem
	// Il faut le demander au Character possédé par le PlayerController
	
	// Récupérer le Pawn du joueur
	APawn* PawnOwner = GetPawn();
	if (!PawnOwner)
	{
		return nullptr;
	}

	return ISystemsInterface::GetArchetypeAbilitySystemFromObject(PawnOwner);
}

// ═══════════════════════════════════════════════════════════════════════════
// INTERFACE : ISaveableInterface
// ═══════════════════════════════════════════════════════════════════════════

bool APlayerStateBase::SaveToPlayer_Implementation(UPlayerSaveGame* SaveGame, const FString& Context)
{
	if (!SaveGame || !GlobalAbilitySystem)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerStateBase::SaveToPlayer - SaveGame ou GlobalAbilitySystem invalide"));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("PlayerStateBase::SaveToPlayer - Contexte: %s"), *Context);

	// ───────────────────────────────────────────────────────────────────────
	// Sauvegarder les attributs globaux
	// ───────────────────────────────────────────────────────────────────────
	
	if (GlobalAttributeSet)
	{
		// Sauvegarder Level
		SaveGame->Level = static_cast<int32>(GlobalAttributeSet->GetLevel());
		
		// Sauvegarder CurrentExp
		SaveGame->CurrentExp = GlobalAttributeSet->GetCurrentExp();
		
		UE_LOG(LogTemp, Log, TEXT("  -> Level sauvegardé: %d"), SaveGame->Level);
		UE_LOG(LogTemp, Log, TEXT("  -> CurrentExp sauvegardé: %f"), SaveGame->CurrentExp);
	}

	// ───────────────────────────────────────────────────────────────────────
	// Sauvegarder l'archétype actuel
	// ───────────────────────────────────────────────────────────────────────
	
	SaveGame->Archetype = CurrentArchetype;
	UE_LOG(LogTemp, Log, TEXT("  -> Archetype sauvegardé: %d"), static_cast<int32>(CurrentArchetype));

	return true;
}

bool APlayerStateBase::LoadFromPlayer_Implementation(UPlayerSaveGame* SaveGame, const FString& Context)
{
	if (!SaveGame || !GlobalAbilitySystem)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerStateBase::LoadFromPlayer - SaveGame ou GlobalAbilitySystem invalide"));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("PlayerStateBase::LoadFromPlayer - Contexte: %s"), *Context);

	// Appeler le serveur pour charger les données (sécurise le chargement)
	LoadOnServer(SaveGame->Archetype, SaveGame->Level, SaveGame->CurrentExp);

	return true;
}

void APlayerStateBase::LoadOnServer_Implementation(const EClassArchetype LoadArchetype, const int32 LoadLevel, const float LoadCurrentExp)
{
	// ───────────────────────────────────────────────────────────────────────
	// Charger les attributs globaux
	// ───────────────────────────────────────────────────────────────────────
	
	if (GlobalAttributeSet)
	{
		// Charger Level
		GlobalAbilitySystem->SetNumericAttributeBase(
			UGlobalAttributeSet::GetLevelAttribute(),
			static_cast<float>(LoadLevel)
		);
		
		// Charger CurrentExp
		GlobalAbilitySystem->SetNumericAttributeBase(
			UGlobalAttributeSet::GetCurrentExpAttribute(),
			LoadCurrentExp
		);
		
		UE_LOG(LogTemp, Log, TEXT("  -> Level chargé: %d"), LoadLevel);
		UE_LOG(LogTemp, Log, TEXT("  -> CurrentExp chargé: %f"), LoadCurrentExp);
	}

	// ───────────────────────────────────────────────────────────────────────
	// Charger l'archétype actuel
	// ───────────────────────────────────────────────────────────────────────
	
	CurrentArchetype = LoadArchetype;
	UE_LOG(LogTemp, Log, TEXT("  -> Archetype chargé: %d"), static_cast<int32>(CurrentArchetype));
}

// ═══════════════════════════════════════════════════════════════════════════
// MÉTHODE : InitializeGlobalAbilitySystem
// ═══════════════════════════════════════════════════════════════════════════



void APlayerStateBase::InitializeGlobalAbilitySystem()
{
	if (!GlobalAbilitySystem)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerStateBase::InitializeGlobalAbilitySystem - GlobalAbilitySystem est nullptr"));
		return;
	}

	// ───────────────────────────────────────────────────────────────────────
	// Initialiser l'AbilitySystemComponent avec le PlayerState comme Owner et Avatar
	// ───────────────────────────────────────────────────────────────────────
	
	GlobalAbilitySystem->InitAbilityActorInfo(this, this);
	
	UE_LOG(LogTemp, Log, TEXT("PlayerStateBase::InitializeGlobalAbilitySystem - ASC initialisé pour %s"), 
		*GetName());

	// ───────────────────────────────────────────────────────────────────────
	// Ajouter le GlobalAttributeSet à l'ASC
	// ───────────────────────────────────────────────────────────────────────
	
	if (GlobalAttributeSet)
	{
		GlobalAbilitySystem->AddAttributeSetSubobject(GlobalAttributeSet.Get());
		UE_LOG(LogTemp, Log, TEXT("  -> GlobalAttributeSet ajouté à l'ASC"));
	}

	// ───────────────────────────────────────────────────────────────────────
	// Initialiser les attributs avec des valeurs par défaut
	// ───────────────────────────────────────────────────────────────────────
	
	GlobalAbilitySystem->SetNumericAttributeBase(UGlobalAttributeSet::GetLevelAttribute(), 1.0f);
	GlobalAbilitySystem->SetNumericAttributeBase(UGlobalAttributeSet::GetCurrentExpAttribute(), 0.0f);
	
	UE_LOG(LogTemp, Log, TEXT("  -> Attributs globaux initialisés (Level: 1, XP: 0)"));

	// ───────────────────────────────────────────────────────────────────────
	// TODO PHASE 8+ : Ajouter les abilities globales
	// ───────────────────────────────────────────────────────────────────────
	
	// Exemple : GlobalAbilitySystem->GiveAbility(...);
}

// ═══════════════════════════════════════════════════════════════════════════
// MÉTHODE : SwitchArchetype (NOUVEAU)
// ═══════════════════════════════════════════════════════════════════════════

void APlayerStateBase::SwitchArchetype(EClassArchetype NewArchetype)
{
	if (NewArchetype == EClassArchetype::None) return;
	// Vérifier que l'archétype est différent
	if (CurrentArchetype == NewArchetype)
	{
		UE_LOG(LogTemp, Warning, TEXT("SwitchArchetype: Archétype identique (%d), annulé"), 
			static_cast<int32>(NewArchetype));
		return;
	}

	// ───────────────────────────────────────────────────────────────────────
	// CAS 1 : Serveur (Authority)
	// ───────────────────────────────────────────────────────────────────────
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Log, TEXT("SwitchArchetype: Changement côté SERVEUR %d -> %d"), 
			static_cast<int32>(CurrentArchetype), static_cast<int32>(NewArchetype));
		
		// Changer l'archétype (déclenchera OnRep côté clients)
		CurrentArchetype = NewArchetype;
		
		if (OnArchetypeChanged.IsBound())
		{
			OnArchetypeChanged.Execute(CurrentArchetype);
		}
		
		// ───────────────────────────────────────────────────────────────────
		// SAUVEGARDE IMMÉDIATE si on est le serveur LOCAL (listen server)
		// ───────────────────────────────────────────────────────────────────
		if (GetOwningController() && GetOwningController()->IsLocalController())
		{
			if (USaveSystem* System = GetSaveSystem(GetOwningController()))
			{
				System->SaveGame(ESaveType::PlayerSave, this, "SwitchArchetype_LocalServer");
				UE_LOG(LogTemp, Log, TEXT("  -> Sauvegarde LOCAL SERVER effectuée"));
			}
		}
		// Sinon, la sauvegarde sera faite côté client via OnRep_CurrentArchetype
	}
	// ───────────────────────────────────────────────────────────────────────
	// CAS 2 : Client (demande au serveur)
	// ───────────────────────────────────────────────────────────────────────
	else
	{
		UE_LOG(LogTemp, Log, TEXT("SwitchArchetype: Demande au serveur %d -> %d"), 
			static_cast<int32>(CurrentArchetype), static_cast<int32>(NewArchetype));
		
		// Demander au serveur de faire le changement
		Server_SwitchArchetype(NewArchetype);
		
		// La sauvegarde côté client sera faite dans OnRep_CurrentArchetype
	}
}

void APlayerStateBase::Server_SwitchArchetype_Implementation(EClassArchetype NewArchetype)
{
	// Le serveur change l'archétype
	SwitchArchetype(NewArchetype);
}