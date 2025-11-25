#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DarkPlayerController.generated.h"


class UInventoryManager;
class UInventoryController;
class UInventory;
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
	// ─── Functions (Replicates) ────────────────────────────────────────────────────
	UFUNCTION(Client, Reliable)
	void SpawnPlayer_Request();
	
private:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Debug) ─────────────────────────────────────────────────────────
	void OpenSessionMenu_Request();
	void KickPlayer_Request();
	void LeaveSession_Request();
	
	UFUNCTION()
	void OnPrompt_Success(const FString& Value);
	UFUNCTION()
	void OnPrompt_Fail(const FString& Value);
	
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	UPROPERTY()
	TObjectPtr<UGuiController> GuiController;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInventoryManager> InventoryManager;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInventory> Storage;
};
