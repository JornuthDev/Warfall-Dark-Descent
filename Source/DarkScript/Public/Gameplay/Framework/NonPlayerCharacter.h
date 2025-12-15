// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Framework/Base_Character.h"
#include "NonPlayerCharacter.generated.h"

UCLASS()
class DARKSCRIPT_API ANonPlayerCharacter : public ABase_Character
{
	GENERATED_BODY()

public:
	// ═══════════════════════════════════════════════════════════════════════
	// CONSTRUCTEUR
	// ═══════════════════════════════════════════════════════════════════════
 
	ANonPlayerCharacter();
	virtual void PossessedBy(AController* NewController) override;
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Blueprint API)
	// ═══════════════════════════════════════════════════════════════════════
    
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════
	UPROPERTY(BlueprintReadOnly, Category = "Gameplay|Ability")
	FName DataRowName;
};
