#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PromptRoot.generated.h"

enum class EPromptType : uint8;
/**
 * 
 */
UCLASS()
class DARK_SCRIPT_API UPromptRoot : public UUserWidget
{
	GENERATED_BODY()
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Delegates ─────────────────────────────────────────────────────────────────
private:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPromptYesDelegate, const FString&, Value);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPromptNoDelegate, const FString&, Value);
	
	
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	virtual void NativeConstruct() override;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category="Prompt", meta=(DisplayName="Response To Prompt"))
	void OnPromptResponse(const FString& Value, const bool bPositive)
	{
		bResponse = bPositive;
		PromptValue = Value;
		RemoveFromParent();
	}
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual void NativeDestruct() override;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	FOnPromptYesDelegate OnYes;
	FOnPromptNoDelegate OnNo;
	FSimpleDelegate OnEscape;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Prompt")
	EPromptType PromptType;
	
	FString PromptValue = "";
	
	bool bCloseOnEsc = true;
	bool bEndByClose = false;
	bool bResponse = false;
	bool bIsShowMouseCursor;
	bool bIsInputModeGameOnly = false;
	
	UPROPERTY()
	UWidget* LastFocus = nullptr;
	
};
