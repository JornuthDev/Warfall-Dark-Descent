#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DarkPlayerController.generated.h"


class UInputMappingContext;
class UGuiController;

UCLASS()
class DARK_SCRIPT_API ADarkPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	ADarkPlayerController();
	virtual void BeginPlay() override;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category="Input|Enhanced")
	void ApplyInput(FName RowName, int32 PriorityBase = 0, bool bClearExisting = true) const;
	
	UFUNCTION(BlueprintCallable, Category="Input|Enhanced")
	void ApplyInputs(const TArray<FName>& RowNames, int32 PriorityBase = 0, bool bClearExisting = true) const;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	static bool ResolveInputsRow(FName RowName, struct FInputsRow& OutRow);
	void ApplyInputContexts(const TArray<UInputMappingContext*>& Contexts, int32 PriorityBase, bool bClearExisting) const;
	virtual void SetupInputComponent() override;
	virtual void ClientWasKicked_Implementation(const FText& KickReason) override;

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UGuiController* GuiController = nullptr;
};
