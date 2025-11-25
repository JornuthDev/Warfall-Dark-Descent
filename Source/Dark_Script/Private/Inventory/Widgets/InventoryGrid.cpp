// ═══════════════════════════════════════════════════════════════════════════
// INVENTORY GRID - IMPLEMENTATION
// ═══════════════════════════════════════════════════════════════════════════

#include "Inventory/Widgets/InventoryGrid.h"
#include "Inventory/Inventory.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Rendering/DrawElements.h"
#include "Framework/Application/SlateApplication.h"

// ═══════════════════════════════════════════════════════════════════════════
// UINVENTORYGRID - UWIDGET IMPLEMENTATION
// ═══════════════════════════════════════════════════════════════════════════

UInventoryGrid::UInventoryGrid(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsDragging = false;
	bRotateNextPlacement = false;
	BoundContainerIndex = -1;
}

void UInventoryGrid::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (GridPanel.IsValid())
	{
		GridPanel->SetGridSize(GridSize);
	}
}

void UInventoryGrid::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	GridPanel.Reset();
}

TSharedRef<SWidget> UInventoryGrid::RebuildWidget()
{
	GridPanel = SNew(SInventoryGridPanel)
		.GridSize(GridSize)
		.CellSize(CellSize)
		.CellPadding(CellPadding)
		.BackgroundColor(BackgroundColor)
		.EmptyCellColor(EmptyCellColor)
		.CellBorderColor(CellBorderColor)
		.HoverCellColor(HoverCellColor);

	// Bind callbacks
	if (GridPanel.IsValid())
	{
		GridPanel->OnCellLeftClicked = [this](FIntPoint Position, bool bIsShiftHeld)
		{
			HandleItemLeftClick(Position, bIsShiftHeld);
		};

		GridPanel->OnCellRightClicked = [this](FIntPoint Position)
		{
			HandleItemRightClick(Position);
		};

		GridPanel->OnDragStarted = [this](FIntPoint Position, bool bIsSplit)
		{
			HandleDragStart(Position, bIsSplit);
		};

		GridPanel->OnDragEnded = [this](FIntPoint TargetPosition)
		{
			HandleDragEnd(TargetPosition);
		};

		GridPanel->OnRotationToggled = [this]()
		{
			HandleRotationToggle();
		};
	}

	return GridPanel.ToSharedRef();
}

#if WITH_EDITOR
const FText UInventoryGrid::GetPaletteCategory()
{
	return NSLOCTEXT("InventoryGrid", "Category", "Inventory");
}
#endif

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

void UInventoryGrid::SetInventoryContainer(UInventory* InInventory, int32 InContainerIndex)
{
	// Unbind old inventory
	if (BoundInventory)
	{
		BoundInventory->OnItemAdded.RemoveDynamic(this, &UInventoryGrid::OnInventoryItemAdded);
		BoundInventory->OnItemRemoved.RemoveDynamic(this, &UInventoryGrid::OnInventoryItemRemoved);
		BoundInventory->OnItemMoved.RemoveDynamic(this, &UInventoryGrid::OnInventoryItemMoved);
	}

	BoundInventory = InInventory;
	BoundContainerIndex = InContainerIndex;

	// Bind new inventory
	if (BoundInventory)
	{
		BoundInventory->OnItemAdded.AddDynamic(this, &UInventoryGrid::OnInventoryItemAdded);
		BoundInventory->OnItemRemoved.AddDynamic(this, &UInventoryGrid::OnInventoryItemRemoved);
		BoundInventory->OnItemMoved.AddDynamic(this, &UInventoryGrid::OnInventoryItemMoved);

		// Update grid size from container (RUNTIME size overrides design-time size)
		FContainerData* Container = BoundInventory->GetContainer(BoundContainerIndex);
		if (Container && GridPanel.IsValid())
		{
			// Update UWidget property
			GridSize = Container->GridSize;
			
			// Update Slate panel and force layout recalculation
			GridPanel->SetGridSize(Container->GridSize);
		}
	}
	
	// Update Slate widget inventory data
	if (GridPanel.IsValid())
	{
		GridPanel->SetInventoryData(BoundInventory, BoundContainerIndex);
	}

	RefreshGrid();
}

void UInventoryGrid::RefreshGrid()
{
	if (GridPanel.IsValid())
	{
		GridPanel->Refresh();
	}
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Event Handlers ────────────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

void UInventoryGrid::HandleItemLeftClick(FIntPoint Position, bool bIsShiftHeld)
{
	if (!BoundInventory)
	{
		return;
	}

	FGameItemData* ItemData = BoundInventory->GetItemAt(BoundContainerIndex, Position);
	if (!ItemData)
	{
		return;
	}

	// If shift held: start split drag
	if (bIsShiftHeld && ItemData->Quantity > 1)
	{
		HandleDragStart(Position, true);
	}
	else
	{
		// Normal click
		OnItemClicked(*ItemData, EKeys::LeftMouseButton);
	}
}

void UInventoryGrid::HandleItemRightClick(FIntPoint Position)
{
	if (!BoundInventory)
	{
		return;
	}

	FGameItemData* ItemData = BoundInventory->GetItemAt(BoundContainerIndex, Position);
	if (!ItemData)
	{
		return;
	}

	OnItemRightClicked(*ItemData);
}

void UInventoryGrid::HandleDragStart(FIntPoint Position, bool bIsSplit)
{
	if (!BoundInventory)
	{
		return;
	}

	FGameItemData* ItemData = BoundInventory->GetItemAt(BoundContainerIndex, Position);
	if (!ItemData)
	{
		return;
	}

	DraggedItem = *ItemData;
	bIsDragging = true;
	bIsSplitDrag = bIsSplit;  // Store if this is a split operation

	OnItemDragStarted(DraggedItem);
}

void UInventoryGrid::HandleDragEnd(FIntPoint TargetPosition)
{
	if (!bIsDragging || !BoundInventory)
	{
		bIsDragging = false;
		bIsSplitDrag = false;
		return;
	}

	bool bWasValidDrop = false;
	FIntPoint SourcePosition = DraggedItem.GridPosition.Position;

	// Check if dropping on same position
	if (SourcePosition == TargetPosition && !bRotateNextPlacement)
	{
		// No-op: dropping on self
		bIsDragging = false;
		bIsSplitDrag = false;
		bRotateNextPlacement = false;
		DraggedItem = FGameItemData();
		return;
	}

	// Check if target cell has an item
	FGameItemData* TargetItem = BoundInventory->GetItemAt(BoundContainerIndex, TargetPosition);

	if (TargetItem)
	{
		// ═══════════════════════════════════════════════════════════════════════════
		// TARGET CELL HAS ITEM: SWAP or STACK
		// ═══════════════════════════════════════════════════════════════════════════

		// Check if items can stack
		bool bCanStack = (TargetItem->ItemRowHandle.RowName == DraggedItem.ItemRowHandle.RowName);

		if (bIsSplitDrag)
		{
			// ─── SHIFT+DRAG TO ITEM ───
			
			if (bCanStack)
			{
				// SPLIT+STACK: Add split quantity to existing stack
				int32 MaxSplitQuantity = FMath::Min(
					DraggedItem.Quantity - 1,  // Must leave at least 1 in source
					TargetItem->Quantity  // Room available in target (handled by server)
				);
				
				int32 SplitQuantity = OnSplitRequested(DraggedItem, MaxSplitQuantity);
				
				if (SplitQuantity > 0 && SplitQuantity <= MaxSplitQuantity)
				{
					BoundInventory->SplitItem(
						BoundContainerIndex,
						SourcePosition,
						SplitQuantity,
						BoundContainerIndex,
						TargetPosition
					);
					bWasValidDrop = true;
				}
			}
			else
			{
				// SPLIT+SWAP: Split source, swap with target
				// This is complex - for now just do normal swap
				UE_LOG(LogTemp, Warning, TEXT("Split+Swap not fully implemented yet - doing regular swap"));
				
				BoundInventory->SwapItems(
					BoundContainerIndex, SourcePosition,
					BoundContainerIndex, TargetPosition
				);
				bWasValidDrop = true;
			}
		}
		else
		{
			// ─── NORMAL DRAG TO ITEM ───
			
			if (bCanStack)
			{
				// STACK: Move entire quantity to existing stack (if there's room)
				FItemRowDetails* RowDetails = DraggedItem.GetRowDetails();
				if (RowDetails)
				{
					int32 AvailableSpace = RowDetails->MaxStackSize - TargetItem->Quantity;
					
					if (AvailableSpace > 0)
					{
						// Can add at least some quantity
						int32 QuantityToMove = FMath::Min(DraggedItem.Quantity, AvailableSpace);
						
						// Use SplitItem to transfer (handles stacking automatically)
						// Works for both full and partial moves
						BoundInventory->SplitItem(
							BoundContainerIndex, SourcePosition, QuantityToMove,
							BoundContainerIndex, TargetPosition
						);
						bWasValidDrop = true;
						
						// Note: If QuantityToMove < DraggedItem.Quantity, 
						// the remainder stays in source position automatically
					}
					else
					{
						// Target stack is full - swap instead
						BoundInventory->SwapItems(
							BoundContainerIndex, SourcePosition,
							BoundContainerIndex, TargetPosition
						);
						bWasValidDrop = true;
					}
				}
			}
			else
			{
				// SWAP: Different items, swap positions
				BoundInventory->SwapItems(
					BoundContainerIndex, SourcePosition,
					BoundContainerIndex, TargetPosition
				);
				bWasValidDrop = true;
			}
		}
	}
	else
	{
		// ═══════════════════════════════════════════════════════════════════════════
		// TARGET CELL IS EMPTY: MOVE or SPLIT
		// ═══════════════════════════════════════════════════════════════════════════

		// Check if drop position is valid
		if (IsValidDropPosition(TargetPosition))
		{
			if (bIsSplitDrag && DraggedItem.Quantity > 1)
			{
				// ─── SHIFT+DRAG TO EMPTY ───
				// SPLIT: Create new stack with custom quantity (Blueprint can show UI)
				int32 SplitQuantity = OnSplitRequested(DraggedItem, DraggedItem.Quantity - 1);
				
				if (SplitQuantity > 0 && SplitQuantity < DraggedItem.Quantity)
				{
					BoundInventory->SplitItem(
						BoundContainerIndex,
						SourcePosition,
						SplitQuantity,
						BoundContainerIndex,
						TargetPosition
					);
					bWasValidDrop = true;
				}
				// else: User cancelled split
			}
			else
			{
				// ─── NORMAL DRAG TO EMPTY ───
				// MOVE: Move entire item to new position
				BoundInventory->MoveItem(
					BoundContainerIndex,
					SourcePosition,
					BoundContainerIndex,
					TargetPosition,
					bRotateNextPlacement
				);
				bWasValidDrop = true;
			}
		}
	}

	OnItemDropped(DraggedItem, TargetPosition, bWasValidDrop);

	// Reset drag state
	bIsDragging = false;
	bIsSplitDrag = false;
	bRotateNextPlacement = false;
	DraggedItem = FGameItemData();
}

int32 UInventoryGrid::OnSplitRequested_Implementation(const FGameItemData& ItemData, int32 MaxQuantity)
{
	// Default: split half (Blueprint can override for custom UI)
	// For even numbers: 200 → 100/100
	// For odd numbers: 201 → 100/101 (source keeps the extra)
	return (MaxQuantity + 1) / 2;  // Ceil division
}

void UInventoryGrid::HandleRotationToggle()
{
	if (bIsDragging)
	{
		bRotateNextPlacement = !bRotateNextPlacement;
	}
}

FLinearColor UInventoryGrid::GetItemQualityColor(EItemQuality Quality) const
{
	const FLinearColor* FoundColor = QualityColors.Find(Quality);
	return FoundColor ? *FoundColor : FLinearColor::White;
}

bool UInventoryGrid::IsValidDropPosition(FIntPoint Position) const
{
	if (!BoundInventory || !bIsDragging)
	{
		return false;
	}

	FItemRowDetails* RowDetails = DraggedItem.GetRowDetails();
	if (!RowDetails)
	{
		return false;
	}

	return BoundInventory->CanItemFitAt(
		BoundContainerIndex,
		Position,
		RowDetails->ObjectSize2D,
		bRotateNextPlacement,
		&DraggedItem
	);
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Inventory Event Callbacks ─────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

void UInventoryGrid::OnInventoryItemAdded(const FGameItemData& ItemData, int32 ContainerIndex, bool bSuccess)
{
	if (ContainerIndex == BoundContainerIndex && bSuccess)
	{
		RefreshGrid();
	}
}

void UInventoryGrid::OnInventoryItemRemoved(const FGameItemData& ItemData, int32 ContainerIndex, bool bSuccess)
{
	if (ContainerIndex == BoundContainerIndex && bSuccess)
	{
		RefreshGrid();
	}
}

void UInventoryGrid::OnInventoryItemMoved(const FGameItemData& ItemData, int32 FromContainer, int32 ToContainer, bool bSuccess)
{
	if ((FromContainer == BoundContainerIndex || ToContainer == BoundContainerIndex) && bSuccess)
	{
		RefreshGrid();
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// SINVENTORYGRIDPANEL - SLATE WIDGET IMPLEMENTATION
// ═══════════════════════════════════════════════════════════════════════════

void SInventoryGridPanel::Construct(const FArguments& InArgs)
{
	GridSize = InArgs._GridSize;
	CellSize = InArgs._CellSize;
	CellPadding = InArgs._CellPadding;
	BackgroundColor = InArgs._BackgroundColor;
	EmptyCellColor = InArgs._EmptyCellColor;
	CellBorderColor = InArgs._CellBorderColor;
	HoverCellColor = InArgs._HoverCellColor;

	bIsDragging = false;
	HoveredCell = FIntPoint(-1, -1);
	DragStartCell = FIntPoint(-1, -1);
	DragPreviewTargetPosition = FIntPoint(-1, -1);
	bDragPreviewRotated = false;
	bIsDragPreviewValid = false;
	MouseDownPosition = FVector2D::ZeroVector;
	
	// Default colors
	ValidPlacementColor = FLinearColor(0.2f, 0.8f, 0.2f, 0.5f);
	InvalidPlacementColor = FLinearColor(0.8f, 0.2f, 0.2f, 0.5f);
	DragShadowColor = FLinearColor(0.8f, 0.8f, 0.8f, 0.7f);
	GhostedItemColor = FLinearColor(0.3f, 0.3f, 0.3f, 0.3f);
}

int32 SInventoryGridPanel::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	// Draw background
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId++,
		AllottedGeometry.ToPaintGeometry(),
		FCoreStyle::Get().GetBrush("WhiteBrush"),
		ESlateDrawEffect::None,
		BackgroundColor
	);

	// Draw empty cells
	for (int32 Y = 0; Y < GridSize.Y; Y++)
	{
		for (int32 X = 0; X < GridSize.X; X++)
		{
			FIntPoint Cell(X, Y);
			FLinearColor CellColor = EmptyCellColor;

			// Highlight hovered cell
			if (Cell == HoveredCell)
			{
				CellColor = HoverCellColor;
			}

			DrawCell(AllottedGeometry, OutDrawElements, LayerId, Cell, CellColor, true);
		}
	}

	// Draw items
	if (BoundInventory.IsValid())
	{
		FContainerData* Container = BoundInventory->GetContainer(BoundContainerIndex);
		if (Container)
		{
			for (const FGameItemDataEntry& Entry : Container->Items.Items)
			{
				DrawItem(AllottedGeometry, OutDrawElements, LayerId, Entry.ItemData);
			}
		}
	}

	// Draw drag preview
	if (bIsDragging && DragPreviewTargetPosition != FIntPoint(-1, -1))
	{
		DrawDragPreview(AllottedGeometry, OutDrawElements, LayerId);
	}

	return LayerId;
}

FReply SInventoryGridPanel::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FIntPoint Cell = GetCellFromPosition(MyGeometry, MouseEvent.GetScreenSpacePosition());
	
	if (!IsCellValid(Cell))
	{
		return FReply::Unhandled();
	}

	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bool bIsShiftHeld = MouseEvent.IsShiftDown();
		
		// ALWAYS capture mouse down position for drag detection
		MouseDownPosition = MouseEvent.GetScreenSpacePosition();
		
		// Store clicked item data for drag preview (if any)
		if (BoundInventory.IsValid())
		{
			FGameItemData* ItemData = BoundInventory->GetItemAt(BoundContainerIndex, Cell);
			if (ItemData)
			{
				DraggedItemData = *ItemData;
			}
		}
		
		if (OnCellLeftClicked)
		{
			OnCellLeftClicked(Cell, bIsShiftHeld);
		}

		// Start potential drag
		DragStartCell = Cell;
		return FReply::Handled().CaptureMouse(SharedThis(this));
	}
	else if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (OnCellRightClicked)
		{
			OnCellRightClicked(Cell);
		}
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply SInventoryGridPanel::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (bIsDragging && OnDragEnded)
		{
			FIntPoint Cell = GetCellFromPosition(MyGeometry, MouseEvent.GetScreenSpacePosition());
			OnDragEnded(Cell);
		}

		bIsDragging = false;
		DragStartCell = FIntPoint(-1, -1);
		
		// Reset drag preview
		DragPreviewTargetPosition = FIntPoint(-1, -1);
		bDragPreviewRotated = false;
		bIsDragPreviewValid = false;
		DraggedItemData = FGameItemData();
		
		return FReply::Handled().ReleaseMouseCapture();
	}

	return FReply::Unhandled();
}

FReply SInventoryGridPanel::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FIntPoint NewHoveredCell = GetCellFromPosition(MyGeometry, MouseEvent.GetScreenSpacePosition());
	
	if (NewHoveredCell != HoveredCell)
	{
		HoveredCell = NewHoveredCell;
	}

	// Check if should start drag
	if (!bIsDragging && DragStartCell.X >= 0 && MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		// Calculate total distance moved from initial mouse down position
		FVector2D CurrentPosition = MouseEvent.GetScreenSpacePosition();
		FVector2D TotalDragDistance = CurrentPosition - MouseDownPosition;
		
		// Use a small threshold (5 pixels) - now works for slow movements too!
		if (TotalDragDistance.SizeSquared() > 25.0f) // 5 pixel threshold
		{
			bIsDragging = true;
			if (OnDragStarted)
			{
				OnDragStarted(DragStartCell, MouseEvent.IsShiftDown());
			}
		}
	}
	
	// Update drag preview if dragging
	if (bIsDragging && BoundInventory.IsValid())
	{
		// Update drag preview position
		FIntPoint CurrentCell = GetCellFromPosition(MyGeometry, MouseEvent.GetScreenSpacePosition());
		DragPreviewTargetPosition = CurrentCell;
		
		// Check if current position is valid
		FIntPoint ItemSize = DraggedItemData.ObjectSize2D;
		if (bDragPreviewRotated)
		{
			ItemSize = FIntPoint(ItemSize.Y, ItemSize.X);
		}
		
		bIsDragPreviewValid = BoundInventory->CanItemFitAt(
			BoundContainerIndex,
			CurrentCell,
			DraggedItemData.ObjectSize2D,
			bDragPreviewRotated,
			&DraggedItemData  // Ignore the item being dragged
		);
		
		// Force repaint to show preview
		const_cast<SInventoryGridPanel*>(this)->Invalidate(EInvalidateWidgetReason::Paint);
	}

	return FReply::Handled();
}

FReply SInventoryGridPanel::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::R)
	{
		if (bIsDragging)
		{
			bDragPreviewRotated = !bDragPreviewRotated;
			
			// Revalidate placement with new rotation
			if (BoundInventory.IsValid() && DragPreviewTargetPosition != FIntPoint(-1, -1))
			{
				bIsDragPreviewValid = BoundInventory->CanItemFitAt(
					BoundContainerIndex,
					DragPreviewTargetPosition,
					DraggedItemData.ObjectSize2D,
					bDragPreviewRotated,
					&DraggedItemData
				);
			}
			
			// Force repaint to show rotated preview
			const_cast<SInventoryGridPanel*>(this)->Invalidate(EInvalidateWidgetReason::Paint);
		}
		
		if (OnRotationToggled)
		{
			OnRotationToggled();
		}
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FVector2D SInventoryGridPanel::ComputeDesiredSize(float) const
{
	float Width = GridSize.X * (CellSize + CellPadding) + CellPadding;
	float Height = GridSize.Y * (CellSize + CellPadding) + CellPadding;
	return FVector2D(Width, Height);
}

void SInventoryGridPanel::SetGridSize(FIntPoint NewSize)
{
	if (GridSize != NewSize)
	{
		GridSize = NewSize;
		
		// Force layout recalculation with new grid size
		Invalidate(EInvalidateWidgetReason::Layout);
		Invalidate(EInvalidateWidgetReason::Paint);
	}
}

void SInventoryGridPanel::SetInventoryData(UInventory* Inventory, int32 ContainerIndex)
{
	BoundInventory = Inventory;
	BoundContainerIndex = ContainerIndex;
}

void SInventoryGridPanel::Refresh()
{
	Invalidate(EInvalidateWidgetReason::Paint);
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Helper Functions ──────────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

FIntPoint SInventoryGridPanel::GetCellFromPosition(const FGeometry& Geometry, const FVector2D& ScreenPosition) const
{
	FVector2D LocalPosition = Geometry.AbsoluteToLocal(ScreenPosition);
	
	int32 X = FMath::FloorToInt(LocalPosition.X / (CellSize + CellPadding));
	int32 Y = FMath::FloorToInt(LocalPosition.Y / (CellSize + CellPadding));
	
	return FIntPoint(X, Y);
}

FVector2D SInventoryGridPanel::GetCellScreenPosition(FIntPoint Cell) const
{
	float X = Cell.X * (CellSize + CellPadding) + CellPadding;
	float Y = Cell.Y * (CellSize + CellPadding) + CellPadding;
	return FVector2D(X, Y);
}

bool SInventoryGridPanel::IsCellValid(FIntPoint Cell) const
{
	return Cell.X >= 0 && Cell.Y >= 0 && Cell.X < GridSize.X && Cell.Y < GridSize.Y;
}

void SInventoryGridPanel::DrawCell(const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements,
	int32& LayerId, FIntPoint Cell, const FLinearColor& Color, bool bDrawBorder) const
{
	FVector2D CellPos = GetCellScreenPosition(Cell);
	FVector2D CellDrawSize(CellSize, CellSize);

	// Draw cell background
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(FVector2f(CellDrawSize), FSlateLayoutTransform(FVector2f(CellPos))),
		FCoreStyle::Get().GetBrush("WhiteBrush"),
		ESlateDrawEffect::None,
		Color
	);

	// Draw border if requested
	if (bDrawBorder)
	{
		TArray<FVector2D> BorderPoints;
		BorderPoints.Add(CellPos);
		BorderPoints.Add(CellPos + FVector2D(CellSize, 0));
		BorderPoints.Add(CellPos + FVector2D(CellSize, CellSize));
		BorderPoints.Add(CellPos + FVector2D(0, CellSize));
		BorderPoints.Add(CellPos);

		FSlateDrawElement::MakeLines(
			OutDrawElements,
			LayerId + 1,
			AllottedGeometry.ToPaintGeometry(),
			BorderPoints,
			ESlateDrawEffect::None,
			CellBorderColor,
			false,
			1.0f
		);
	}

	LayerId += 2;
}

void SInventoryGridPanel::DrawItem(const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements,
	int32& LayerId, const FGameItemData& ItemData) const
{
	FItemRowDetails* RowDetails = ItemData.GetRowDetails();
	if (!RowDetails)
	{
		return;
	}

	FVector2D ItemPos = GetCellScreenPosition(ItemData.GridPosition.Position);
	FIntPoint ActualSize = ItemData.GetActualSize();
	FVector2D ItemDrawSize(
		ActualSize.X * (CellSize + CellPadding) - CellPadding,
		ActualSize.Y * (CellSize + CellPadding) - CellPadding
	);

	// Determine if this item is being dragged
	bool bIsBeingDragged = bIsDragging && 
	                       ItemData.GridPosition.Position == DragStartCell;

	// Use ghosted color if being dragged, normal color otherwise
	FLinearColor ItemColor = bIsBeingDragged ? 
	                         GhostedItemColor : 
	                         FLinearColor(0.2f, 0.2f, 0.2f, 1.0f);
	// TODO: Get quality color from UInventoryGrid

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId++,
		AllottedGeometry.ToPaintGeometry(FVector2f(ItemDrawSize), FSlateLayoutTransform(FVector2f(ItemPos))),
		FCoreStyle::Get().GetBrush("WhiteBrush"),
		ESlateDrawEffect::None,
		ItemColor
	);

	// Draw item icon if available
	if (RowDetails->Icon)
	{
		FSlateBrush IconBrush;
		IconBrush.SetResourceObject(RowDetails->Icon);
		IconBrush.ImageSize = ItemDrawSize;
		IconBrush.DrawAs = ESlateBrushDrawType::Image;
		
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId++,
			AllottedGeometry.ToPaintGeometry(FVector2f(ItemDrawSize), FSlateLayoutTransform(FVector2f(ItemPos))),
			&IconBrush,
			ESlateDrawEffect::None,
			FLinearColor::White
		);
	}

	// Draw quantity text if > 1
	if (ItemData.Quantity > 1)
	{
		FString QuantityText = FString::Printf(TEXT("%d"), ItemData.Quantity);
		
		FVector2D TextPos = ItemPos + FVector2D(4, ItemDrawSize.Y - 20);
		FVector2D TextSize = FVector2D(ItemDrawSize.X - 8, 16);
		
		FSlateDrawElement::MakeText(
			OutDrawElements,
			LayerId++,
			AllottedGeometry.ToPaintGeometry(FVector2f(TextSize), FSlateLayoutTransform(FVector2f(TextPos))),
			QuantityText,
			FCoreStyle::GetDefaultFontStyle("Regular", 10),
			ESlateDrawEffect::None,
			FLinearColor::White
		);
	}
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Draw Drag Preview ─────────────────────────────────────────────────────────
// ───────────────────────────────────────────────────────────────────────────────

void SInventoryGridPanel::DrawDragPreview(const FGeometry& AllottedGeometry, 
	FSlateWindowElementList& OutDrawElements, int32& LayerId) const
{
	if (!BoundInventory.IsValid())
	{
		return;
	}

	// Get item size
	FIntPoint ItemSize = DraggedItemData.ObjectSize2D;
	if (bDragPreviewRotated)
	{
		ItemSize = FIntPoint(ItemSize.Y, ItemSize.X);
	}

	// Draw preview cells with validation color
	FLinearColor PreviewColor = bIsDragPreviewValid ? ValidPlacementColor : InvalidPlacementColor;
	
	for (int32 Y = 0; Y < ItemSize.Y; Y++)
	{
		for (int32 X = 0; X < ItemSize.X; X++)
		{
			FIntPoint CellPos = DragPreviewTargetPosition + FIntPoint(X, Y);
			
			// Only draw if cell is within grid bounds
			if (IsCellValid(CellPos))
			{
				DrawCell(AllottedGeometry, OutDrawElements, LayerId, CellPos, PreviewColor, true);
			}
		}
	}

	// Draw item icon/shadow at preview position
	FItemRowDetails* RowDetails = DraggedItemData.GetRowDetails();
	if (RowDetails && RowDetails->Icon)
	{
		FVector2D ItemPos = GetCellScreenPosition(DragPreviewTargetPosition);
		FVector2D ItemDrawSize(
			ItemSize.X * (CellSize + CellPadding) - CellPadding,
			ItemSize.Y * (CellSize + CellPadding) - CellPadding
		);

		// Draw semi-transparent shadow of the item
		FSlateBrush IconBrush;
		IconBrush.SetResourceObject(RowDetails->Icon);
		IconBrush.ImageSize = ItemDrawSize;
		IconBrush.DrawAs = ESlateBrushDrawType::Image;
		
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId++,
			AllottedGeometry.ToPaintGeometry(FVector2f(ItemDrawSize), FSlateLayoutTransform(FVector2f(ItemPos))),
			&IconBrush,
			ESlateDrawEffect::None,
			DragShadowColor  // Semi-transparent
		);
		
		// Draw quantity if > 1
		if (DraggedItemData.Quantity > 1)
		{
			FString QuantityText = FString::Printf(TEXT("%d"), DraggedItemData.Quantity);
			FVector2D TextPos = ItemPos + FVector2D(4, ItemDrawSize.Y - 20);
			FVector2D TextSize = FVector2D(ItemDrawSize.X - 8, 16);
			
			FSlateDrawElement::MakeText(
				OutDrawElements,
				LayerId++,
				AllottedGeometry.ToPaintGeometry(FVector2f(TextSize), FSlateLayoutTransform(FVector2f(TextPos))),
				QuantityText,
				FCoreStyle::GetDefaultFontStyle("Regular", 10),
				ESlateDrawEffect::None,
				FLinearColor::White
			);
		}
	}
}