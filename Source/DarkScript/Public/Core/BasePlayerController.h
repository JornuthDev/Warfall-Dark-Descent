#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

class UProgression;
/**
 * 
 */
UCLASS()
class DARKSCRIPT_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	ABasePlayerController();
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category="Input|Enhanced")
	void ApplyInput(FName RowName, int32 PriorityBase = 0, bool bClearExisting = true) const;
	UFUNCTION(BlueprintCallable, Category="Input|Enhanced")
	void ApplyInputs(const TArray<FName>& RowNames, int32 PriorityBase = 0, bool bClearExisting = true) const;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	virtual void ClientWasKicked_Implementation(const FText& KickReason) override;
	
protected:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UProgression> Progression;
	
};
