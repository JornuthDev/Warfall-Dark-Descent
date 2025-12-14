// Copyright Dark Script - All Rights Reserved

#include "Core/CharacterBase.h"
#include "Gameplay/AbilitySystem/ArchetypeAbilitySystem.h"
#include "Gameplay/AbilitySystem/ArchetypeAttributeSet.h"
#include "Gameplay/AbilitySystem/GlobalAbilitySystem.h"
#include "Core/PlayerStateBase.h"
#include "GameFramework/PlayerState.h"

// ═══════════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════════

ACharacterBase::ACharacterBase()
{
	// ───────────────────────────────────────────────────────────────────────
	// Activer le tick si nécessaire
	// ───────────────────────────────────────────────────────────────────────
	PrimaryActorTick.bCanEverTick = true;

	// ───────────────────────────────────────────────────────────────────────
	// Créer l'ArchetypeAbilitySystem
	// ───────────────────────────────────────────────────────────────────────
	ArchetypeAbilitySystem = CreateDefaultSubobject<UArchetypeAbilitySystem>(TEXT("ArchetypeAbilitySystem"));
	ArchetypeAbilitySystem->SetIsReplicated(true);
	ArchetypeAbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Full);

	// ───────────────────────────────────────────────────────────────────────
	// Créer l'ArchetypeAttributeSet
	// ───────────────────────────────────────────────────────────────────────
	ArchetypeAttributeSet = CreateDefaultSubobject<UArchetypeAttributeSet>(TEXT("ArchetypeAttributeSet"));

	// ───────────────────────────────────────────────────────────────────────
	// Archétype par défaut (sera écrasé par le PlayerState)
	// ───────────────────────────────────────────────────────────────────────
	IncarnatedArchetype = EClassArchetype::Tank;
}

// ═══════════════════════════════════════════════════════════════════════════
// BEGIN PLAY
// ═══════════════════════════════════════════════════════════════════════════

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// L'initialisation de l'ASC se fait dans PossessedBy (serveur) ou OnRep_PlayerState (client)
	// Ne rien faire ici pour éviter les doubles initialisations
}

// ═══════════════════════════════════════════════════════════════════════════
// POSSESSED BY (Serveur uniquement)
// ═══════════════════════════════════════════════════════════════════════════

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Cette fonction est appelée UNIQUEMENT côté serveur
	// C'est ici qu'on initialise l'ASC côté serveur
	
	UE_LOG(LogTemp, Log, TEXT("CharacterBase::PossessedBy - Initialisation côté SERVEUR"));
	
	InitializeArchetypeAbilitySystem();
	
	if (APlayerStateBase* PS = Cast<APlayerStateBase>(GetPlayerState()))
	{
		PS->OnArchetypeChanged.BindLambda([this] (const EClassArchetype Archetype)
		{
			OnArchetypeChanged(Archetype);
		});
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// ON REP PLAYER STATE (Client uniquement)
// ═══════════════════════════════════════════════════════════════════════════

void ACharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Cette fonction est appelée UNIQUEMENT côté client quand le PlayerState est répliqué
	// C'est ici qu'on initialise l'ASC côté client
	
	UE_LOG(LogTemp, Log, TEXT("CharacterBase::OnRep_PlayerState - Initialisation côté CLIENT"));
	
	InitializeArchetypeAbilitySystem();
	
	if (APlayerStateBase* PS = Cast<APlayerStateBase>(GetPlayerState()))
	{
		PS->OnArchetypeChanged.BindLambda([this] (const EClassArchetype Archetype)
		{
			OnArchetypeChanged(Archetype);
		});
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// INTERFACE : IAbilitySystemInterface
// ═══════════════════════════════════════════════════════════════════════════

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	// Pour le Character, l'ASC principal est l'ArchetypeAbilitySystem
	return ArchetypeAbilitySystem;
}

// ═══════════════════════════════════════════════════════════════════════════
// INTERFACE : ISystemsInterface
// ═══════════════════════════════════════════════════════════════════════════

UGlobalAbilitySystem* ACharacterBase::GetGlobalAbilitySystem_Implementation()
{
	// Le Character ne possède PAS le GlobalAbilitySystem
	// Il faut le demander au PlayerState
	
	APlayerState* PS = GetPlayerState();
	if (!PS)
	{
		return nullptr;
	}

	return ISystemsInterface::GetGlobalAbilitySystemFromObject(PS);
}

UArchetypeAbilitySystem* ACharacterBase::GetArchetypeAbilitySystem_Implementation()
{
	// Le Character possède l'ArchetypeAbilitySystem
	return ArchetypeAbilitySystem;
}

// ═══════════════════════════════════════════════════════════════════════════
// MÉTHODE : InitializeArchetypeAbilitySystem
// ═══════════════════════════════════════════════════════════════════════════

void ACharacterBase::InitializeArchetypeAbilitySystem()
{
	if (!ArchetypeAbilitySystem)
	{
		UE_LOG(LogTemp, Error, TEXT("CharacterBase::InitializeArchetypeAbilitySystem - ArchetypeAbilitySystem est nullptr"));
		return;
	}

	// Vérifier que le PlayerState existe
	APlayerStateBase* PS = GetPlayerState<APlayerStateBase>();
	if (!PS)
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterBase::InitializeArchetypeAbilitySystem - PlayerState non disponible, retry later"));
		return;
	}

	// ───────────────────────────────────────────────────────────────────────
	// Initialiser l'AbilitySystemComponent
	// Owner = PlayerState (pour la persistance entre les Characters)
	// Avatar = Character (pour les animations, effets visuels, etc.)
	// ───────────────────────────────────────────────────────────────────────
	
	ArchetypeAbilitySystem->InitAbilityActorInfo(PS, this);
	
	UE_LOG(LogTemp, Log, TEXT("CharacterBase::InitializeArchetypeAbilitySystem - ASC initialisé pour %s"), 
		*GetName());

	// ───────────────────────────────────────────────────────────────────────
	// Ajouter l'ArchetypeAttributeSet à l'ASC
	// ───────────────────────────────────────────────────────────────────────
	
	if (ArchetypeAttributeSet)
	{
		ArchetypeAbilitySystem->AddAttributeSetSubobject(ArchetypeAttributeSet.Get());
		UE_LOG(LogTemp, Log, TEXT("  -> ArchetypeAttributeSet ajouté à l'ASC"));
	}

	// ───────────────────────────────────────────────────────────────────────
	// Récupérer l'archétype actuel depuis le PlayerState
	// ───────────────────────────────────────────────────────────────────────
	
	IncarnatedArchetype = PS->CurrentArchetype;
	UE_LOG(LogTemp, Log, TEXT("  -> Archétype incarné: %d"), static_cast<int32>(IncarnatedArchetype));
	
	// ───────────────────────────────────────────────────────────────────────
	// Initialiser les attributs avec des valeurs par défaut
	// Uniquement côté serveur pour éviter les conflits
	// ───────────────────────────────────────────────────────────────────────
	
	if (HasAuthority())
	{
		ArchetypeAbilitySystem->InitializeAttributes(IncarnatedArchetype);
		UE_LOG(LogTemp, Log, TEXT("  -> Attributs d'archétype initialisés (HP: 100/100, Mana: 100/100)"));
	}

	// ───────────────────────────────────────────────────────────────────────
	// TODO PHASE 8+ : Ajouter les abilities de l'archétype
	// ───────────────────────────────────────────────────────────────────────
	
	// Exemple : ArchetypeAbilitySystem->GiveAbility(...);
}

// ═══════════════════════════════════════════════════════════════════════════
// MÉTHODE : OnArchetypeChanged
// ═══════════════════════════════════════════════════════════════════════════

void ACharacterBase::OnArchetypeChanged_Implementation(EClassArchetype NewArchetype)
{
	IncarnatedArchetype = NewArchetype;
	
	UE_LOG(LogTemp, Log, TEXT("CharacterBase::OnArchetypeChanged - Nouvel archétype: %d"), 
		static_cast<int32>(NewArchetype));

	// ───────────────────────────────────────────────────────────────────────
	// TODO PHASE 9+ : Changer le mesh, les animations, etc.
	// ───────────────────────────────────────────────────────────────────────
	
	// Exemple :
	// switch (NewArchetype)
	// {
	//     case EClassArchetype::Tank:
	//         GetMesh()->SetSkeletalMesh(TankMesh);
	//         break;
	//     case EClassArchetype::Healer:
	//         GetMesh()->SetSkeletalMesh(HealerMesh);
	//         break;
	//     // etc.
	// }
}