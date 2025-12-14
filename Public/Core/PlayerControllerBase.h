#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerBase.generated.h"

class UProgression;
enum class EClassArchetype : uint8;

/**
 * PlayerController - Gère les inputs et coordonne le switch d'archétypes
 */
UCLASS()
class DARKSCRIPT_API APlayerControllerBase : public APlayerController, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	APlayerControllerBase();
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category="Input|Enhanced")
	void ApplyInput(FName RowName, int32 PriorityBase = 0, bool bClearExisting = true) const;
	
	UFUNCTION(BlueprintCallable, Category="Input|Enhanced")
	void ApplyInputs(const TArray<FName>& RowNames, int32 PriorityBase = 0, bool bClearExisting = true) const;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	virtual void ClientWasKicked_Implementation(const FText& KickReason) override;
	
protected:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────

};