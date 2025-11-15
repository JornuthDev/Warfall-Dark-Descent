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
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject", Category = "Navigation|Prompt", DisplayName="Create & Display Prompt"))
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
