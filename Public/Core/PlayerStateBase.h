// Copyright Dark Script - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/SaveableInterface.h"
#include "Interfaces/SystemsInterface.h"
#include "Utils/Types/PlayerTypes.h"
#include "PlayerStateBase.generated.h"

class UGlobalAbilitySystem;
class UGlobalAttributeSet;
class UPlayerSaveGame;

// ═══════════════════════════════════════════════════════════════════════════
// CLASSE : PLAYER STATE BASE
// ═══════════════════════════════════════════════════════════════════════════

/**
 * PlayerState de base du projet Dark Script
 * 
 * Contient :
 * - GlobalAbilitySystem : ASC pour les données globales (Level, XP)
 * - GlobalAttributeSet : Attributs partagés entre tous les archétypes
 * - CurrentArchetype : L'archétype actuellement incarné (répliqué)
 * 
 * Implémente :
 * - IAbilitySystemInterface : Interface GAS standard
 * - ISystemsInterface : Interface custom pour accès aux systèmes
 * - ISaveableInterface : Interface de sauvegarde
 */
UCLASS()
class DARKSCRIPT_API APlayerStateBase : public APlayerState, 
	public IAbilitySystemInterface,
	public ISystemsInterface,
	public ISaveableInterface
{
	GENERATED_BODY()

	// ═══════════════════════════════════════════════════════════════════════
	// DELEGATES
	// ═══════════════════════════════════════════════════════════════════════
	
	DECLARE_DELEGATE_OneParam(FOnArchetypeChanged, EClassArchetype);
	
public:
	// ═══════════════════════════════════════════════════════════════════════
	// CONSTRUCTEUR
	// ═══════════════════════════════════════════════════════════════════════
	
	APlayerStateBase();
	FOnArchetypeChanged OnArchetypeChanged;
	
	// ═══════════════════════════════════════════════════════════════════════
	// COMPONENTS
	// ═══════════════════════════════════════════════════════════════════════
	
	/** AbilitySystemComponent pour les données GLOBALES (Level, XP) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	TObjectPtr<UGlobalAbilitySystem> GlobalAbilitySystem;

	// ═══════════════════════════════════════════════════════════════════════
	// ATTRIBUTE SETS
	// ═══════════════════════════════════════════════════════════════════════
	
	/** AttributeSet global (Level, CurrentExp) */
	UPROPERTY()
	TObjectPtr<UGlobalAttributeSet> GlobalAttributeSet;

	// ═══════════════════════════════════════════════════════════════════════
	// ARCHETYPE ACTUEL (répliqué)
	// ═══════════════════════════════════════════════════════════════════════
	
	/** L'archétype actuellement incarné par le joueur */
	UPROPERTY(ReplicatedUsing = OnRep_CurrentArchetype, BlueprintReadOnly, Category = "Archetype")
	EClassArchetype CurrentArchetype;

	/** Callback de réplication pour CurrentArchetype */
	UFUNCTION()
	virtual void OnRep_CurrentArchetype();

	// ═══════════════════════════════════════════════════════════════════════
	// INTERFACE : IAbilitySystemInterface (GAS standard)
	// ═══════════════════════════════════════════════════════════════════════
	
	/**
	 * Retourne l'AbilitySystemComponent principal
	 * Pour le PlayerState, c'est le GlobalAbilitySystem
	 */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// ═══════════════════════════════════════════════════════════════════════
	// INTERFACE : ISystemsInterface (custom)
	// ═══════════════════════════════════════════════════════════════════════
	
	/**
	 * Retourne le GlobalAbilitySystem (implémentation de l'interface)
	 */
	virtual UGlobalAbilitySystem* GetGlobalAbilitySystem_Implementation() override;

	/**
	 * Retourne l'ArchetypeAbilitySystem (forward vers le Character)
	 * Note : Le PlayerState ne possède PAS l'ArchetypeASC, il doit le demander au Character
	 */
	virtual UArchetypeAbilitySystem* GetArchetypeAbilitySystem_Implementation() override;

	// ═══════════════════════════════════════════════════════════════════════
	// INTERFACE : ISaveableInterface
	// ═══════════════════════════════════════════════════════════════════════
	
	/**
	 * Sauvegarde les données globales dans le PlayerSaveGame
	 * - Level
	 * - CurrentExp
	 * - CurrentArchetype
	 */
	virtual bool SaveToPlayer_Implementation(UPlayerSaveGame* SaveGame, const FString& Context) override;

	/**
	 * Charge les données globales depuis le PlayerSaveGame
	 * - Level
	 * - CurrentExp
	 * - CurrentArchetype
	 */
	virtual bool LoadFromPlayer_Implementation(UPlayerSaveGame* SaveGame, const FString& Context) override;
	
	/**
	 * RPC Server pour charger les données globales
	 * Appelé depuis LoadFromPlayer_Implementation pour assurer le chargement côté serveur
	 */
	UFUNCTION(Server, Reliable)
	void LoadOnServer(const EClassArchetype LoadArchetype, const int32 LoadLevel, const float LoadCurrentExp);
	void LoadOnServer_Implementation(const EClassArchetype LoadArchetype, const int32 LoadLevel, const float LoadCurrentExp);
	
	// ═══════════════════════════════════════════════════════════════════════
	// MÉTHODES PUBLIQUES
	// ═══════════════════════════════════════════════════════════════════════
	
	/**
	 * Initialise le GlobalAbilitySystem
	 * Appelé depuis BeginPlay du PlayerState (serveur uniquement)
	 */
	UFUNCTION(BlueprintCallable, Category = "AbilitySystem")
	void InitializeGlobalAbilitySystem();

	/**
	 * Change l'archétype actuel (Lobby uniquement)
	 * 
	 * Gestion intelligente de la sauvegarde :
	 * - Si appelé côté serveur LOCAL (listen server) : sauvegarde immédiate
	 * - Si appelé côté client : RPC vers serveur, puis OnRep côté client déclenche la sauvegarde
	 * 
	 * @param NewArchetype Le nouvel archétype à activer
	 */
	UFUNCTION(BlueprintCallable, Category = "Archetype")
	void SwitchArchetype(EClassArchetype NewArchetype);

protected:
	// ═══════════════════════════════════════════════════════════════════════
	// RPC SERVER
	// ═══════════════════════════════════════════════════════════════════════
	
	/**
	 * RPC Server pour changer d'archétype
	 * Appelé automatiquement par SwitchArchetype() si on est côté client
	 */
	UFUNCTION(Server, Reliable)
	void Server_SwitchArchetype(EClassArchetype NewArchetype);
	void Server_SwitchArchetype_Implementation(EClassArchetype NewArchetype);

	// ═══════════════════════════════════════════════════════════════════════
	// OVERRIDES
	// ═══════════════════════════════════════════════════════════════════════
	
	/** Appelé au début du jeu */
	virtual void BeginPlay() override;

	/** Enregistre les propriétés répliquées */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};