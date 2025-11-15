#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WindowRoot.generated.h"

/**
 * UWindowRoot
 *
 * Base widget class for all UI windows in the game.
 * Provides common lifecycle hooks (activation/deactivation),
 * module control logic, and general integration with the GUI system.
 *
 * Designed to be subclassed in Blueprint or C++ for specific window types.
 */
UCLASS()
class DARK_SCRIPT_API UWindowRoot : public UUserWidget
{
	GENERATED_BODY()
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	explicit UWindowRoot(const FObjectInitializer& ObjectInitializer);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	/**
	 * Called right before the window becomes active (added to viewport or focused).
	 * Override in Blueprint or C++ to prepare state, load data, or trigger animations.
	 */	
	UFUNCTION(BlueprintNativeEvent, Category="Window|Lifecycle", meta = (DisplayName="On Before Activated"))
	void OnBeforeActivated();
	virtual void OnBeforeActivated_Implementation();
	
	/**
	 * Called once the window is fully active and visible.
	 * Override in Blueprint or C++ to initialize visual components or bind inputs.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Window|Lifecycle", meta = (DisplayName="On Activated"))
	void OnActivated();
	virtual void OnActivated_Implementation();
	
	/**
	 * Called right before the window is deactivated or removed from the viewport.
	 * Override in Blueprint or C++ to clean up data, stop animations, or save state.
	 */	
	UFUNCTION(BlueprintNativeEvent, Category="Window|Lifecycle", meta = (DisplayName="On Before Deactivated"))
	void OnBeforeDeactivated();
	virtual void OnBeforeDeactivated_Implementation();
	
	/**
	 * Returns the unique module key associated with this window.
	 * Used internally by the GUI system to identify and manage active modules.
	 */	
	UFUNCTION(BlueprintPure, Category="Window|Control", meta = (DisplayName="Get Module Key"))
	FName GetModuleKeyName() const { return ModuleKey; }
	
	/**
	 * Disables this window's associated module via the GUI Controller.
	 * Triggers internal cleanup and removal from the viewport.
	 */	
	UFUNCTION(BlueprintCallable, Category="Window|Control", meta = (DisplayName="Disable Module"))
	void DisableModule() const;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	void SetModuleKey(const FName InKey) { ModuleKey = InKey; }		
	void SetCloseOptions(const bool bInCloseOnEsc, const bool bInCloseOnDeath) { bCloseOnEsc = bInCloseOnEsc; bCloseOnDeath = bInCloseOnDeath; }
	
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
protected:
	/** Unique identifier for this module within the GUI system. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Window|Control")
	FName ModuleKey = NAME_None;
	
	/** If true, this window will automatically close when the player presses ESC. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Window|Control")
	bool bCloseOnEsc = true;
	
	/** If true, this window will automatically close when the player dies. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Window|Control")
	bool bCloseOnDeath = true;
};

/**
 * EModuleDisplayType
 *
 * Defines how a UI module (window) should be displayed in the viewport.
 * Used by the GUI controller to determine Z-order and interaction behavior.
 */
UENUM(BlueprintType)
enum class EModuleDisplayType : uint8
{
	/** Permanent HUD element (e.g., health bar, minimap). */
	E_Hud UMETA(DisplayName="HUD"),

	/** Fullscreen interface that hides HUD elements (e.g., inventory, pause menu). */
	E_Fullscreen UMETA(DisplayName="Fullscreen"),

	/** Popup or overlay displayed on top of other elements. */
	E_Popup UMETA(DisplayName="Popup"),
};