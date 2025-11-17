#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DarkPlayerState.generated.h"

class UInventory;
class USaveInstance;
class ADarkCharacter;
/**
 * 
 */
UCLASS()
class DARK_SCRIPT_API ADarkPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	ADarkPlayerState();
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Replicates) ────────────────────────────────────────────────────
	UFUNCTION(Server, Reliable)
	void SpawnCharacterClass();
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ADarkCharacter> CharacterClass;
	TWeakObjectPtr<USaveInstance> SaveInstance;
};
