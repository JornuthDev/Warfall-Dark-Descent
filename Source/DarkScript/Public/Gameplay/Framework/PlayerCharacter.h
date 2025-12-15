// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Framework/Base_Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class DARKSCRIPT_API APlayerCharacter : public ABase_Character
{
	GENERATED_BODY()

public:
	// ═══════════════════════════════════════════════════════════════════════
	// CONSTRUCTEUR
	// ═══════════════════════════════════════════════════════════════════════
	
	APlayerCharacter();
	virtual void PossessedBy(AController* NewController) override;
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Blueprint API)
	// ═══════════════════════════════════════════════════════════════════════
    
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Helpers)
	// ═══════════════════════════════════════════════════════════════════════
	
	UFUNCTION(BlueprintPure, Category = "Framework")
	APlayerState* GetLocalPlayerState() const
	{
		if (IsLocallyControlled())
		{
			return GetPlayerState();
		}
		return nullptr;
	}
	
protected:
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Replication)
	// ═══════════════════════════════════════════════════════════════════════
	
	virtual void OnRep_PlayerState() override;
	UFUNCTION(Server,Reliable)
	void Server_SetupArchetype(const EClassArchetype InArchetype);
	void Server_SetupArchetype_Implementation(const EClassArchetype InArchetype)
	{
		IncarnatedArchetype = InArchetype;
	}
	
public:
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════
	
	UPROPERTY(BlueprintReadOnly, Category = "Gameplay|Ability")
	EClassArchetype IncarnatedArchetype;
};
