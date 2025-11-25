// ═══════════════════════════════════════════════════════════════════════════
// INVENTORY GRID - SLATE UI WIDGET
// ═══════════════════════════════════════════════════════════════════════════

#pragma once
#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Tools/Types/InventoryTypes.h"
#include "InventoryGrid.generated.h"

class UInventory;
class SInventoryGridPanel;

/**
 * Inventory Grid Widget (Tarkov-style)
 * Slate-based for performance
 * 
 * Design-time: Configure grid size, border colors
 * Runtime: Call SetInventoryContainer() to bind to inventory
 * 
 * Operations:
 * - Move: Left-click hold
 * - Split: Shift + Left-click
 * - Swap: Move onto another item
 * - Rotate: R key
 */
UCLASS()
class DARK_SCRIPT_API UInventoryGrid : public UWidget
{
	GENERATED_BODY()

public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	UInventoryGrid(const FObjectInitializer& ObjectInitializer);

	// UWidget interface
	virtual void SynchronizeProperties() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/**
	 * Bind widget to inventory container
	 * @param InInventory - Target inventory
	 * @param InContainerIndex - Container index to display
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Grid")
	void SetInventoryContainer(UInventory* InInventory, int32 InContainerIndex);

	/**
	 * Get bound inventory
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Grid")
	UInventory* GetBoundInventory() const { return BoundInventory; }

	/**
	 * Get bound container index
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Grid")
	int32 GetBoundContainerIndex() const { return BoundContainerIndex; }

	/**
	 * Refresh the grid display
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Grid")
	void RefreshGrid();

	/**
	 * Get currently dragged item (if any)
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Grid")
	FGameItemData GetDraggedItem() const { return DraggedItem; }

	/**
	 * Check if currently dragging
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Grid")
	bool IsDragging() const { return bIsDragging; }

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Properties (Design-Time) ──────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/** Grid size for design-time preview (overridden by runtime inventory) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", meta = (ClampMin = "1"))
	FIntPoint GridSize = FIntPoint(8, 5);

	/** Cell size in pixels */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", meta = (ClampMin = "16", ClampMax = "128"))
	float CellSize = 48.0f;

	/** Padding between cells */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", meta = (ClampMin = "0", ClampMax = "10"))
	float CellPadding = 2.0f;

	/** Grid background color */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	FLinearColor BackgroundColor = FLinearColor(0.02f, 0.02f, 0.02f, 0.9f);

	/** Empty cell color */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	FLinearColor EmptyCellColor = FLinearColor(0.1f, 0.1f, 0.1f, 1.0f);

	/** Cell border color */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	FLinearColor CellBorderColor = FLinearColor(0.2f, 0.2f, 0.2f, 1.0f);

	/** Hover cell color */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	FLinearColor HoverCellColor = FLinearColor(0.3f, 0.3f, 0.5f, 1.0f);

	/** Valid placement color */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	FLinearColor ValidPlacementColor = FLinearColor(0.2f, 0.8f, 0.2f, 0.5f);

	/** Invalid placement color */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	FLinearColor InvalidPlacementColor = FLinearColor(0.8f, 0.2f, 0.2f, 0.5f);

	/** Item slot colors by quality */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	TMap<EItemQuality, FLinearColor> QualityColors = {
		{ EItemQuality::Common, FLinearColor(0.5f, 0.5f, 0.5f, 1.0f) },
		{ EItemQuality::Uncommon, FLinearColor(0.2f, 0.8f, 0.2f, 1.0f) },
		{ EItemQuality::Rare, FLinearColor(0.2f, 0.5f, 1.0f, 1.0f) },
		{ EItemQuality::Epic, FLinearColor(0.6f, 0.2f, 0.8f, 1.0f) },
		{ EItemQuality::Legendary, FLinearColor(1.0f, 0.6f, 0.1f, 1.0f) }
	};

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Blueprint Events ──────────────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/**
	 * Called when item is clicked
	 * @param ItemData - Clicked item data
	 * @param MouseButton - Mouse button used
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory|Events")
	void OnItemClicked(const FGameItemData& ItemData, FKey MouseButton);

	/**
	 * Called when item drag starts
	 * @param ItemData - Dragged item data
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory|Events")
	void OnItemDragStarted(const FGameItemData& ItemData);

	/**
	 * Called when item is dropped
	 * @param ItemData - Dropped item data
	 * @param TargetPosition - Drop position
	 * @param bWasValidDrop - Was drop location valid
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory|Events")
	void OnItemDropped(const FGameItemData& ItemData, FIntPoint TargetPosition, bool bWasValidDrop);

	/**
	 * Called when item is right-clicked (use/equip)
	 * @param ItemData - Used item data
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory|Events")
	void OnItemRightClicked(const FGameItemData& ItemData);

	/**
	 * Called when split is requested - Override to show split UI
	 * @param ItemData - Item to split
	 * @param MaxQuantity - Maximum quantity that can be split
	 * @return Quantity to split (0 = cancel, default = half)
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory|Events")
	int32 OnSplitRequested(const FGameItemData& ItemData, int32 MaxQuantity);

protected:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Internal Functions ────────────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/** Slate widget creation */
	TSharedPtr<SInventoryGridPanel> GridPanel;

	/** Bound inventory */
	UPROPERTY(Transient)
	TObjectPtr<UInventory> BoundInventory = nullptr;

	/** Bound container index */
	UPROPERTY(Transient)
	int32 BoundContainerIndex = -1;

	/** Currently dragged item */
	UPROPERTY(Transient)
	FGameItemData DraggedItem;

	/** Is currently dragging */
	UPROPERTY(Transient)
	bool bIsDragging = false;

	/** Should rotate next placement */
	UPROPERTY(Transient)
	bool bRotateNextPlacement = false;

	/** Is this a split drag (Shift held) */
	UPROPERTY(Transient)
	bool bIsSplitDrag = false;

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Event Handlers ────────────────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	/** Handle item left-click */
	void HandleItemLeftClick(FIntPoint Position, bool bIsShiftHeld);

	/** Handle item right-click */
	void HandleItemRightClick(FIntPoint Position);

	/** Handle drag start */
	void HandleDragStart(FIntPoint Position, bool bIsSplit);

	/** Handle drag end */
	void HandleDragEnd(FIntPoint TargetPosition);

	/** Handle rotation key (R) */
	void HandleRotationToggle();

	/** Get item color based on quality */
	FLinearColor GetItemQualityColor(EItemQuality Quality) const;

	/** Check if position is valid for current drag */
	bool IsValidDropPosition(FIntPoint Position) const;

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Inventory Event Callbacks ─────────────────────────────────────────────────
	// ───────────────────────────────────────────────────────────────────────────────

	UFUNCTION()
	void OnInventoryItemAdded(const FGameItemData& ItemData, int32 ContainerIndex, bool bSuccess);

	UFUNCTION()
	void OnInventoryItemRemoved(const FGameItemData& ItemData, int32 ContainerIndex, bool bSuccess);

	UFUNCTION()
	void OnInventoryItemMoved(const FGameItemData& ItemData, int32 FromContainer, int32 ToContainer, bool bSuccess);
};

// ═══════════════════════════════════════════════════════════════════════════
// SLATE INVENTORY GRID PANEL
// ═══════════════════════════════════════════════════════════════════════════

/**
 * Slate widget for rendering inventory grid
 * High-performance custom drawing
 */
class DARK_SCRIPT_API SInventoryGridPanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SInventoryGridPanel)
		: _GridSize(8, 5)
		, _CellSize(48.0f)
		, _CellPadding(2.0f)
		{}
		SLATE_ARGUMENT(FIntPoint, GridSize)
		SLATE_ARGUMENT(float, CellSize)
		SLATE_ARGUMENT(float, CellPadding)
		SLATE_ARGUMENT(FLinearColor, BackgroundColor)
		SLATE_ARGUMENT(FLinearColor, EmptyCellColor)
		SLATE_ARGUMENT(FLinearColor, CellBorderColor)
		SLATE_ARGUMENT(FLinearColor, HoverCellColor)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	// SWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	virtual FVector2D ComputeDesiredSize(float) const override;

	// Public functions
	void SetGridSize(FIntPoint NewSize);
	void SetInventoryData(UInventory* Inventory, int32 ContainerIndex);
	void Refresh();

	// Callbacks
	TFunction<void(FIntPoint, bool)> OnCellLeftClicked;
	TFunction<void(FIntPoint)> OnCellRightClicked;
	TFunction<void(FIntPoint, bool)> OnDragStarted;
	TFunction<void(FIntPoint)> OnDragEnded;
	TFunction<void()> OnRotationToggled;

private:
	// Grid properties
	FIntPoint GridSize;
	float CellSize;
	float CellPadding;

	// Colors
	FLinearColor BackgroundColor;
	FLinearColor EmptyCellColor;
	FLinearColor CellBorderColor;
	FLinearColor HoverCellColor;

	// Inventory data
	TWeakObjectPtr<UInventory> BoundInventory;
	int32 BoundContainerIndex = -1;

	// Interaction state
	FIntPoint HoveredCell = FIntPoint(-1, -1);
	bool bIsDragging = false;
	FIntPoint DragStartCell = FIntPoint(-1, -1);
	FVector2D MouseDownPosition = FVector2D::ZeroVector;
	
	// Drag preview state
	FGameItemData DraggedItemData;
	FIntPoint DragPreviewTargetPosition = FIntPoint(-1, -1);
	bool bDragPreviewRotated = false;
	bool bIsDragPreviewValid = false;
	
	// Drag preview colors
	FLinearColor ValidPlacementColor = FLinearColor(0.2f, 0.8f, 0.2f, 0.5f);
	FLinearColor InvalidPlacementColor = FLinearColor(0.8f, 0.2f, 0.2f, 0.5f);
	FLinearColor DragShadowColor = FLinearColor(0.8f, 0.8f, 0.8f, 0.7f);
	FLinearColor GhostedItemColor = FLinearColor(0.3f, 0.3f, 0.3f, 0.3f);

	// Helper functions
	FIntPoint GetCellFromPosition(const FGeometry& Geometry, const FVector2D& LocalPosition) const;
	FVector2D GetCellScreenPosition(FIntPoint Cell) const;
	bool IsCellValid(FIntPoint Cell) const;
	void DrawCell(const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements, int32& LayerId,
		FIntPoint Cell, const FLinearColor& Color, bool bDrawBorder = true) const;
	void DrawItem(const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements, int32& LayerId,
		const FGameItemData& ItemData) const;
	void DrawDragPreview(const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements, 
		int32& LayerId) const;
};