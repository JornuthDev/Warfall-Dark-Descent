// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GuiController.generated.h"

class UWindowRoot;
struct FRootModuleRow;

/**
 * UGuiController
 *
 * Core component responsible for managing all GUI windows and modules.
 * Handles activation/deactivation of UI elements, fullscreen layering,
 * input mode switching, and HUD visibility logic.
 *
 * Typically attached to a PlayerController to act as the bridge between
 * gameplay and user interface systems.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARK_SCRIPT_API UGuiController : public UActorComponent
{
	GENERATED_BODY()
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Delegates ─────────────────────────────────────────────────────────────────
private:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnModuleToggled, FName, Module, bool, bEnabled);

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
public:
	UGuiController();
	virtual void BeginPlay() override;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	
	UFUNCTION(BlueprintCallable, Category="GUI")
	void InitializeHUD();
	/**
 * Enables or disables a given UI module (window).
 * - If enabled, the module is created and displayed.
 * - If disabled, the module is removed and cleaned up.
 *
 * @param Module  The name/key of the module to modify.
 * @param bEnable True to enable, false to disable.
 * @return True if the operation succeeded.
 */
	UFUNCTION(BlueprintCallable, Category="GUI")
	bool EnableModule(FName Module, bool bEnable);
	
/**
 * Toggles a module's visibility.
 * Automatically enables it if inactive, or disables it if currently active.
 *
 * @param Module  The module key to toggle.
 * @return True if the module is now active after toggling.
 */
	UFUNCTION(BlueprintCallable, Category="GUI")
	bool ToggleModule(FName Module);
	
/**
 * Checks if a given module is currently active.
 *
 * @param Module  The module key to query.
 * @return True if the module is active (displayed on screen).
 */	
	UFUNCTION(BlueprintCallable, Category="GUI")
	bool IsActiveModule(FName Module) const;
	
	/**
	 * Returns a reference to a module widget instance if it exists.
	 * Returns nullptr if the module is not active or not yet created.
	 *
	 * @param Module  The module key to retrieve.
	 * @return The UWindowRoot instance, or nullptr.
	 */	
	UFUNCTION(BlueprintCallable, Category="GUI")
	UWindowRoot* GetModule(FName Module) const;
	
/**
* Event triggered when a module changes state (enabled/disabled).
* Allows Blueprints to react to UI module lifecycle changes.
*/
	UPROPERTY(BlueprintAssignable, Category="GUI")
	FOnModuleToggled OnModuleToggled;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
private:
	void LoadModulesTable();
	bool GetModuleRow(FName Module, FRootModuleRow& OutRow, FName& OutKey) const;
	UWindowRoot* EnsureWidgetInstance(const FRootModuleRow& Row, const FName& Key);
	void RefreshInputMode() const;
	void HideHUDForFullscreen(bool bKeepShortcutVisible);
	void RestoreHUDAfterFullscreen();
	void ApplyUIFocus(UWindowRoot* FullscreenWidget) const;
	bool GetTopFullscreen(FRootModuleRow& OutRow, FName& OutKey) const;
	static void HideFullscreenWidget(UWindowRoot* W);
	void ShowFullscreenWidget(UWindowRoot* W, const FRootModuleRow& Row);
	void ApplyShortcutPolicyFromTopFullscreen();
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
/** Data table reference containing all module definitions. */
	UPROPERTY()
	UDataTable* ModulesTable = nullptr;
	
/** Map of module key → instantiated window widgets. */
	UPROPERTY(Transient)
	TMap<FName, UWindowRoot*> ModuleInstances;
	
/** Set of currently active (visible) module keys. */
	UPROPERTY(Transient)
	TSet<FName> ActiveModules;
	
/** Set of HUD modules temporarily hidden due to fullscreen overlays. */
	UPROPERTY(Transient)
	TSet<FName> HudHiddenByFullscreen;
	
/** Stack of active fullscreen module keys (top = current visible one). */
	UPROPERTY(Transient)
	TArray<FName> FullscreenStack;

/** Number of currently active fullscreen windows. */
	int32 ActiveFullscreenCount = 0;

/** Weak reference to the currently focused fullscreen widget. */
	UPROPERTY(Transient)
	TWeakObjectPtr<UWindowRoot> FocusedFullscreen;

/** Rendering order offset applied to popup windows. */
	static constexpr int32 Popup_Z_Bias = 100000;
};
