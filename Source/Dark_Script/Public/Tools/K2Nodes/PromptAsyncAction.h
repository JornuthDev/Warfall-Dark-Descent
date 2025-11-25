#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Core/SubSystems/NavigationInstance.h"
#include "PromptAsyncAction.generated.h"

class UPromptRoot;

UENUM()
enum class EPromptType : uint8
{
	E_None UMETA(DisplayName = "None"),
	E_Response UMETA(DisplayName = "Response"),
	E_ValueReturn UMETA(DisplayName = "ValueReturn"),
	E_All UMETA(DisplayName = "All"),
};

UCLASS()
class DARK_SCRIPT_API UPromptAsyncAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	/**
	 * Creates and displays a prompt widget and returns user feedback asynchronously.
	 *
	 * This async action spawns a UPromptRoot widget inside the active navigation system,
	 * optionally closing when Escape is pressed. The prompt will trigger either OnSuccess 
	 * or OnFailure depending on user input or configured prompt type.
	 *
	 * @param WorldContextObject   Context object used to retrieve the world and navigation system.
	 * @param WidgetClass          The UPromptRoot subclass used to instantiate the prompt widget.
	 * @param PromptType           Determines how the prompt behaves and what type of output it returns.
	 * @param bCloseOnEsc          If true, the prompt closes and fires failure when the user presses Escape.
	 *
	 * @return                     A PromptAsyncAction instance that handles the asynchronous operation.
	 */
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject", DisplayName="Create & Display Prompt"), Category = "Navigation|Prompt")
	static UPromptAsyncAction* CreatePrompt(UObject* WorldContextObject, TSubclassOf<UPromptRoot> WidgetClass, EPromptType PromptType, bool bCloseOnEsc = false);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	virtual void Activate() override;
	UFUNCTION()
	void Internal_OnSuccess(const FString& Value);
	UFUNCTION()
	void Internal_OnFailure(const FString& Value);
	void OnEscape();
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	UPROPERTY(BlueprintAssignable)
	FOnPromptYesDelegate OnSuccess;
	UPROPERTY(BlueprintAssignable)
	FOnPromptNoDelegate OnFailure;
private:
	TSubclassOf<UPromptRoot> StoredClass;
	EPromptType StoredType;
	bool bCloseOnEsc = false;
	
	TWeakObjectPtr<UNavigationInstance> NavInstance;
	TWeakObjectPtr<UObject> WorldContextObject;
	TWeakObjectPtr<UPromptRoot> PromptRoot;
	TWeakObjectPtr<APlayerController> PlayerController;
};
