#include "Components/GuiController.h"

#include "Core/DarkGameInstance.h"
#include "Tools/Libraries/DarkLibrary.h"
#include "Tools/Tables.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
UGuiController::UGuiController()
{
	/*PrimaryComponentTick.bCanEverTick = true;*/
}

void UGuiController::BeginPlay()
{
	Super::BeginPlay();
	
	if (!Cast<APlayerController>(GetOwner())->IsLocalController()) return;
	
	LoadModulesTable();
	if (UDarkLibrary::GetGameInstance(GetWorld())->IsPlaying())
	{
		InitializeHUD();
	}
	else
	{
		/*EnableModule("MainMenu", true);*/
	}
	RefreshInputMode();
}

void UGuiController::InitializeHUD()
{
	if (ModulesTable)
	{
		static const FString Ctx(TEXT("GuiController_BeginPlay"));

		for (TArray<FName> Rows = ModulesTable->GetRowNames(); const FName& RowName : Rows)
		{
			if (const FRootModuleRow* Row = ModulesTable->FindRow<FRootModuleRow>(RowName, Ctx))
			{
				if (Row->Type == EModuleDisplayType::E_Hud && Row->bAutoShow)
				{
					const FName Key = Row->Name.IsNone() ? RowName : Row->Name;
					EnableModule(Key, true);
				}
			}
		}
	}
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
bool UGuiController::EnableModule(FName Module, bool bEnable)
{
	if (!Cast<APlayerController>(GetOwner())->IsLocalController()) return false;
	
	// Resolve row/key and ensure instance
	FRootModuleRow Row;
	FName Key;
	if (!GetModuleRow(Module, Row, Key)) return false;

	UWindowRoot* W = EnsureWidgetInstance(Row, Key);
	if (!W) return false;

	const bool bWasActive = ActiveModules.Contains(Key);

	// ─── ENABLE ────────────────────────────────────────────────────────────────
	if (bEnable)
	{
		if (bWasActive) return true;

		// Z: Popup above all via bias; HUD/Fullscreen use table Z
		const int32 Z = (Row.Type == EModuleDisplayType::E_Popup)
			? Row.ZOrder + Popup_Z_Bias
			: Row.ZOrder;

		W->OnBeforeActivated();
		W->AddToViewport(Z);
		ActiveModules.Add(Key);
		W->OnActivated();

		const int32 FullBefore = ActiveFullscreenCount;

		if (Row.Type == EModuleDisplayType::E_Fullscreen)
		{
			// If a fullscreen is already visible, hide it (keep on stack)
			if (FullscreenStack.Num() > 0)
			{
				FName PrevKey;
				if (FRootModuleRow PrevRow; GetTopFullscreen(PrevRow, PrevKey))
				{
					if (UWindowRoot** PPrev = ModuleInstances.Find(PrevKey))
					{
						HideFullscreenWidget(*PPrev);
					}
				}
			}

			// Push the new fullscreen and update counter
			FullscreenStack.Add(Key);
			ActiveFullscreenCount = FullscreenStack.Num();

			// Focus the new fullscreen
			FocusedFullscreen = W;
			ApplyUIFocus(W);
		}

		// First fullscreen (0 -> 1): hide HUD once (respect Shortcut policy of TOP)
		if (FullBefore == 0 && ActiveFullscreenCount == 1)
		{
			ApplyShortcutPolicyFromTopFullscreen();
			bool bKeepShortcutVisible = false;
			{
				FName TopKey;
				if (FRootModuleRow TopRow; GetTopFullscreen(TopRow, TopKey))
				{
					bKeepShortcutVisible = (TopRow.bHideShortcut == false);
				}
			}
			HideHUDForFullscreen(bKeepShortcutVisible);

		}
		// Already in fullscreen: new TOP may change Shortcut policy
		else if (ActiveFullscreenCount > 0)
		{
			ApplyShortcutPolicyFromTopFullscreen();
		}

		RefreshInputMode();
		OnModuleToggled.Broadcast(Key, true);
		return true;
	}

	// ─── DISABLE ───────────────────────────────────────────────────────────────
	if (!bWasActive) return true;

	W->OnBeforeDeactivated();
	W->RemoveFromParent();
	ActiveModules.Remove(Key);

	const int32 FullBefore = ActiveFullscreenCount;

	if (Row.Type == EModuleDisplayType::E_Fullscreen)
	{
		// Remove this fullscreen from the stack
		if (const int32 Index = FullscreenStack.Find(Key); Index != INDEX_NONE)
		{
			FullscreenStack.RemoveAt(Index);
		}

		ActiveFullscreenCount = FullscreenStack.Num();

		// If any fullscreen remains → show the new TOP and refocus
		if (ActiveFullscreenCount > 0)
		{
			FName NewTopKey;
			if (FRootModuleRow NewTopRow; GetTopFullscreen(NewTopRow, NewTopKey))
			{
				if (UWindowRoot** PTop = ModuleInstances.Find(NewTopKey))
				{
					ShowFullscreenWidget(*PTop, NewTopRow);
				}
			}

			// Shortcut policy now depends on the new TOP fullscreen
			ApplyShortcutPolicyFromTopFullscreen();
		}
	}

	// Leaving fullscreen mode (1 -> 0): restore previously hidden HUD
	if (FullBefore == 1 && ActiveFullscreenCount == 0)
	{
		RestoreHUDAfterFullscreen();
	}

	RefreshInputMode();
	OnModuleToggled.Broadcast(Key, false);
	return true;
}

bool UGuiController::ToggleModule(const FName Module)
{
	const bool bNow = !IsActiveModule(Module);
	EnableModule(Module, bNow);
	return bNow;
}

bool UGuiController::IsActiveModule(const FName Module) const
{
	if (!ModulesTable) return false;

	FName Key;
	if (FRootModuleRow Row; !const_cast<UGuiController*>(this)->GetModuleRow(Module, Row, Key)) return false;

	return ActiveModules.Contains(Key);
}

UWindowRoot* UGuiController::GetModule(const FName Module) const
{
	if (!ModulesTable) return nullptr;

	FName Key;
	if (FRootModuleRow Row; !const_cast<UGuiController*>(this)->GetModuleRow(Module, Row, Key)) return nullptr;

	if (UWindowRoot* const* Ptr = ModuleInstances.Find(Key))
	{
		return *Ptr;
	}
	return nullptr;
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
void UGuiController::LoadModulesTable()
{
	ModulesTable = UTables::GetTable(ETablePath::E_ModulesTable);
}

bool UGuiController::GetModuleRow(const FName Module, FRootModuleRow& OutRow, FName& OutKey) const
{
	if (!ModulesTable) return false;
	const FString Ctx(TEXT("GuiController_GetModuleRow"));
	
	if (const FRootModuleRow* Direct = ModulesTable->FindRow<FRootModuleRow>(Module, Ctx))
	{
		OutRow = *Direct;
		OutKey = Direct->Name.IsNone() ? Module : Direct->Name;
		return true;
	}
	
	for (TArray<FName> Rows = ModulesTable->GetRowNames(); const FName& RowName : Rows)
	{
		if (const FRootModuleRow* Row = ModulesTable->FindRow<FRootModuleRow>(RowName, Ctx))
		{
			if (Row->Name == Module)
			{
				OutRow = *Row;
				OutKey = Row->Name.IsNone() ? RowName : Row->Name;
				return true;
			}
		}
	}
	return false;
}

UWindowRoot* UGuiController::EnsureWidgetInstance(const FRootModuleRow& Row, const FName& Key)
{
	if (UWindowRoot** Found = ModuleInstances.Find(Key))
	{
		return *Found;
	}

	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GuiController] Owner is not a PlayerController."));
		return nullptr;
	}

	UClass* WidgetClass = Row.ModuleClass.LoadSynchronous();
	if (!WidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GuiController] ModuleClass is null for %s"), *Key.ToString());
		return nullptr;
	}

	UWindowRoot* W = CreateWidget<UWindowRoot>(PC, WidgetClass);
	if (!W)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GuiController] CreateWidget failed for %s"), *Key.ToString());
		return nullptr;
	}

	W->SetModuleKey(Key);
	W->SetCloseOptions(Row.bCloseOnEsc, Row.bCloseOnDeath);

	ModuleInstances.Add(Key, W);
	return W;
}

void UGuiController::RefreshInputMode() const
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC) return;
	
	if (ActiveFullscreenCount > 0)
	{
		if (UWindowRoot* Target = FocusedFullscreen.Get())
		{
			FInputModeUIOnly Mode;
			Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			Mode.SetWidgetToFocus(Target->TakeWidget());
			PC->SetInputMode(Mode);
			PC->bShowMouseCursor = true;
		}
		else
		{
			FInputModeUIOnly Mode;
			Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PC->SetInputMode(Mode);
			PC->bShowMouseCursor = true;
		}
	}
	else
	{
		const FInputModeGameOnly Mode;
		PC->SetInputMode(Mode);
		PC->bShowMouseCursor = false;
	}
}

void UGuiController::HideHUDForFullscreen(const bool bKeepShortcutVisible)
{
	if (!ModulesTable) return;

	HudHiddenByFullscreen.Empty();

	static const FString Ctx(TEXT("GuiController_HideHUDForFullscreen"));
	const FName Shortcut(TEXT("Shortcut"));

	for (TArray<FName> Rows = ModulesTable->GetRowNames(); const FName& RowName : Rows)
	{
		const FRootModuleRow* Row = ModulesTable->FindRow<FRootModuleRow>(RowName, Ctx);
		if (!Row || Row->Type != EModuleDisplayType::E_Hud) continue;

		const FName Key = Row->Name.IsNone() ? RowName : Row->Name;
		
		if (!ActiveModules.Contains(Key)) continue;
		
		if (bKeepShortcutVisible && Key == Shortcut)
		{
			continue;
		}

		if (UWindowRoot** Pw = ModuleInstances.Find(Key))
		{
			if (UWindowRoot* W = *Pw)
			{
				W->OnBeforeDeactivated();
				W->RemoveFromParent();
				HudHiddenByFullscreen.Add(Key);
			}
		}
	}
}

void UGuiController::RestoreHUDAfterFullscreen()
{
	for (const FName& Key : HudHiddenByFullscreen)
	{
		if (UWindowRoot** Pw = ModuleInstances.Find(Key))
		{
			if (UWindowRoot* W = *Pw)
			{
				FName Dummy;
				if (FRootModuleRow Row; GetModuleRow(Key, Row, Dummy))
				{
					W->OnBeforeActivated();
					W->AddToViewport(Row.ZOrder);
					W->OnActivated();
				}
				else
				{
					W->OnBeforeActivated();
					W->AddToViewport(0);
					W->OnActivated();
				}
			}
		}
	}
	HudHiddenByFullscreen.Empty();
}

void UGuiController::ApplyUIFocus(UWindowRoot* FullscreenWidget) const
{
	if (!FullscreenWidget) return;

	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC) return;

	FullscreenWidget->SetUserFocus(PC);
	FullscreenWidget->SetKeyboardFocus();

	FInputModeUIOnly Mode;
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	Mode.SetWidgetToFocus(FullscreenWidget->TakeWidget());
	PC->SetInputMode(Mode);
	PC->bShowMouseCursor = true;
}

bool UGuiController::GetTopFullscreen(FRootModuleRow& OutRow, FName& OutKey) const
{
	if (FullscreenStack.Num() <= 0) return false;

	const FName TopKey = FullscreenStack.Last();

	FRootModuleRow Row;
	FName Key;
	if (!const_cast<UGuiController*>(this)->GetModuleRow(TopKey, Row, Key))
		return false;

	OutRow = Row;
	OutKey = Key;
	return true;
}

void UGuiController::HideFullscreenWidget(UWindowRoot* W)
{
	if (!W) return;
	W->OnBeforeDeactivated();
	W->RemoveFromParent();
}

void UGuiController::ShowFullscreenWidget(UWindowRoot* W, const FRootModuleRow& Row)
{
	if (!W) return;

	W->OnBeforeActivated();
	W->AddToViewport(Row.ZOrder);
	W->OnActivated();

	FocusedFullscreen = W;
	ApplyUIFocus(W);
}

void UGuiController::ApplyShortcutPolicyFromTopFullscreen()
{
	if (ActiveFullscreenCount <= 0 || !ModulesTable) return;
	
	FRootModuleRow TopRow;
	if (FName TopKey; !GetTopFullscreen(TopRow, TopKey)) return;

	const bool bKeepShortcutVisible = (TopRow.bHideShortcut == false);

	const FName Shortcut(TEXT("Shortcut"));
	
	FRootModuleRow ShortcutRow;
	FName ShortcutKey;
	if (!GetModuleRow(Shortcut, ShortcutRow, ShortcutKey)) return;
	
	if (!ActiveModules.Contains(ShortcutKey)) return;

	if (const bool bIsHidden = HudHiddenByFullscreen.Contains(ShortcutKey); bKeepShortcutVisible && bIsHidden)
	{
		if (UWindowRoot** Pw = ModuleInstances.Find(ShortcutKey))
		{
			if (UWindowRoot* W = *Pw)
			{
				W->OnBeforeActivated();
				W->AddToViewport(ShortcutRow.ZOrder);
				HudHiddenByFullscreen.Remove(ShortcutKey);
				W->OnActivated();
			}
		}
	}
	else if (!bKeepShortcutVisible && !bIsHidden)
	{
		if (UWindowRoot** Pw = ModuleInstances.Find(ShortcutKey))
		{
			if (UWindowRoot* W = *Pw)
			{
				W->OnBeforeDeactivated();
				W->RemoveFromParent();
				HudHiddenByFullscreen.Add(ShortcutKey);
			}
		}
	}
}

