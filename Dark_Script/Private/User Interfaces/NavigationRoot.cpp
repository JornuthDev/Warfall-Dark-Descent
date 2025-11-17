#include "User Interfaces/NavigationRoot.h"
#include "Core/SubSystems/NavigationInstance.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
UNavigationRoot::UNavigationRoot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);
	bIsVariable = true;
}

void UNavigationRoot::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (!IsWidgetReady()) return;
	if (!IsNavigationFocus()) return;
	
	for (UWidget* Widget : NavigationWidgets)
	{
		if (Widget->HasKeyboardFocus() || Widget->HasAnyUserFocus())
		{
			NavigationIndex = NavigationWidgets.IndexOfByKey(Widget);
			break;
		}
	}
	UpdateWidgets(NavigationWidgets);
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
bool UNavigationRoot::InitializeNavigation(const TArray<UWidget*> Widgets)
{	
	NavigationWidgets = Widgets;
	
	const int32 Num = NavigationWidgets.Num();
	if (Num == 0) return false;
		
	for (int32 i = 0; i < Num; i++)
	{
		UWidget* Widget = NavigationWidgets[i];
		Widget->Navigation = NewObject<UWidgetNavigation>();
		if (!Widget || !Widget->Navigation) continue;
		
		FWidgetNavigationData DataUp;
		DataUp.Rule = EUINavigationRule::Explicit;
		DataUp.Widget = NavigationWidgets[(i - 1 + Num) % Num];
		DataUp.WidgetToFocus = NavigationWidgets[(i - 1 + Num) % Num]->GetFName();
		Widget->Navigation->Up = DataUp;
				
		FWidgetNavigationData DataDown;
		DataDown.Rule = EUINavigationRule::Explicit;
		DataDown.Widget = NavigationWidgets[(i + 1) % Num];
		DataDown.WidgetToFocus = NavigationWidgets[(i + 1) % Num]->GetFName();
		Widget->Navigation->Down = DataDown;
		Widget->BuildNavigation();
	}
	return true;
}

bool UNavigationRoot::UpdateWidgets_Implementation(const TArray<UWidget*>& Widgets)
{
	return true;
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
void UNavigationRoot::OnNavigationRootInitialized()
{
	if (!NavigationWidgets.IsValidIndex(NavigationIndex)) return;
	
	NavWidgetAtIndex(NavigationIndex)->SetKeyboardFocus();
	UpdateWidgets(NavigationWidgets);
}

UWidget* UNavigationRoot::NavWidgetAtIndex(const int32 Index) const
{
	if (NavigationWidgets.IsValidIndex(Index))
	{
		return NavigationWidgets[Index];
	}
	return nullptr;
}

bool UNavigationRoot::IsNavigationFocus() const
{
	if (!GetOwningPlayer() || !GetOwningPlayer()->GetGameInstance()) return false;
	const UNavigationInstance* NavInstance = GetOwningPlayer()->GetGameInstance()->GetSubsystem<UNavigationInstance>();
	if (!NavInstance) return false;
	
	return NavInstance->GetNavigationRoot() == this;
}

int32 UNavigationRoot::IndexWidget(UWidget* Widget) const
{
	if (!Widget || !NavigationWidgets.Contains(Widget)) return -1;
	
	int32 Index = 0;
	for (const UWidget* W : NavigationWidgets)
	{
		if (W == Widget) return Index;
		++Index;
	}
	return -1;
}

bool UNavigationRoot::IsWidgetReady() const
{
	if (!IsValid(this)) return false;
	if (!GetWorld()) return false;
	
	APlayerController* PC = GetOwningPlayer();
	if (!IsValid(PC)) return false;
	
	if (!GetCachedWidget().IsValid()) return false;
	
	UGameInstance* GI = PC->GetGameInstance();
	if (!GI) return false;

	const UNavigationInstance* Nav = GI->GetSubsystem<UNavigationInstance>();
	if (!Nav) return false;
	
	return true;
}

