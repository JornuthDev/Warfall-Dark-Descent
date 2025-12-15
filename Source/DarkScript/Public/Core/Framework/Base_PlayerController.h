#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Base_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DARKSCRIPT_API ABase_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	// ═══════════════════════════════════════════════════════════════════════
	// CONSTRUCTEUR
	// ═══════════════════════════════════════════════════════════════════════
	
	ABase_PlayerController();
    virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Blueprint API)
	// ═══════════════════════════════════════════════════════════════════════
    
	UFUNCTION(BlueprintCallable, Category="Inputs|Enhanced")
	bool ApplyInput(FName RowName, int32 PriorityBase = 0, bool bClearExisting = true) const;
	
	UFUNCTION(BlueprintCallable, Category="Inputs|Enhanced")
	bool ApplyInputs(const TArray<FName>& RowNames, int32 PriorityBase = 0, bool bClearExisting = true) const;
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Override)
	// ═══════════════════════════════════════════════════════════════════════
	
	virtual void ClientWasKicked_Implementation(const FText& KickReason) override;
	
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════
};
