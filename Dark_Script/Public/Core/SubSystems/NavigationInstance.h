#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NavigationInstance.generated.h"

class UWidget;
enum class EPromptType : uint8;
class UPromptRoot;
class UNavigationRoot;

// ───────────────────────────────────────────────────────────────────────────────
// ─── Delegates ─────────────────────────────────────────────────────────────────
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPromptYesDelegate, const FString&, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPromptNoDelegate, const FString&, Value);

/**
 * UNavigationInstance
 *
 * Global subsystem (GameInstanceSubsystem) handling UI navigation logic.
 * Responsible for:
 *   - Managing the currently active NavigationRoot.
 *   - Monitoring keyboard focus changes at the Slate level.
 *   - Automatically restoring focus when it is lost unintentionally.
 *
 * Provides a unified, persistent navigation system across all UIs.
 */
UCLASS()
class DARK_SCRIPT_API UNavigationInstance : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	UNavigationRoot* GetNavigationRoot() const {return NavigationRoot;}
	int32 GetNavigationIndex() const;
	void SetNavigationRoot(UNavigationRoot* InNavRoot);
	
	UPromptRoot* OpenPrompt(UObject* WorldContextObject, TSubclassOf<UPromptRoot> WidgetClass, EPromptType PromptType, const bool bCloseOnEsc = false, const bool bCallBack = false);
protected:
	UFUNCTION()
	void OnSuccessResponse(const FString& Value);
	UFUNCTION()
	void OnFailureResponse(const FString& Value);
	void OnCloseByEscape();
	
	UWidget* GetFocusedWidget() const;
	
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	FOnPromptYesDelegate PromptSuccessResponse;
	FOnPromptNoDelegate PromptFailureResponse;
	FSimpleDelegate PromptCloseByEsc;
	
private:
	UPROPERTY()
	UNavigationRoot* NavigationRoot;
	UPROPERTY()
	UPromptRoot* PromptRoot;
	FDelegateHandle FocusChangingHandle;
};
