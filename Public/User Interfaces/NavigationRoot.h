#pragma once

#include "CoreMinimal.h"
#include "WindowRoot.h"
#include "NavigationRoot.generated.h"

/**
 * UNavigationRoot
 *
 * Base class for any group of navigable widgets.
 * Represents a logical UI section (menu, panel, etc.) that manages
 * its own navigation state and focus flow.
 *
 * Contains an ordered list of widgets that can be navigated through
 * and defines which one should be focused by default.
 */
UCLASS()
class DARK_SCRIPT_API UNavigationRoot : public UUserWidget
{
	GENERATED_BODY()
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	UNavigationRoot(const FObjectInitializer& ObjectInitializer);
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	/** Initializes the list of navigable widgets for this root. */
	UFUNCTION(BlueprintCallable, Category="Navigation")
	bool InitializeNavigation(TArray<UWidget*> Widgets);
	
	/** Returns the widget at the specified index, or nullptr if invalid. */
	UFUNCTION(BlueprintPure, Category="Navigation")
	UWidget* GetNaviWidgetAtIndex(const int32 Index) const { return NavWidgetAtIndex(Index); }
	
	UFUNCTION(BlueprintPure, Category="Navigation")
	int32 GetIndexWidget(UWidget* Widget) const { return IndexWidget(Widget);}
	
	/**
	 * Updates the list of navigable widgets for this root.
	 * 
	 * This is called when the layout of the UI changes dynamically 
	 * (for example, when buttons are added or removed at runtime).
	 * It allows the navigation system to rebuild its internal references.
	 *
	 * @param Widgets  Array of widgets to register as navigation elements.
	 * @return True if the widget list was successfully updated, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Navigation")
	bool UpdateWidgets(const TArray<UWidget*>& Widgets);
	bool UpdateWidgets_Implementation(const TArray<UWidget*>& Widgets);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	void OnNavigationRootInitialized();
	UWidget* NavWidgetAtIndex(const int32 Index) const;
	bool IsNavigationFocus() const;
	int32 IndexWidget(UWidget* Widget) const;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	/** Ordered list of widgets participating in this navigation root. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Navigation")
	TArray<UWidget*> NavigationWidgets;
	
	/** Index of the currently focused widget in the NavigationWidgets array. */
	UPROPERTY(BlueprintReadOnly, Category="Navigation")
	int32 NavigationIndex = 0;
};
