#include "Core/DarkPlayerController.h"
#include "Components/GuiController.h"
#include "Tools/Tables.h"
#include "Tools/Paths.h"
#include "EnhancedInputSubsystems.h"
#include "OnlineSubsystemUtils.h"
#include "Core/DarkGameInstance.h"
#include "Core/DarkPlayerState.h"
#include "Core/SubSystems/NavigationInstance.h"
#include "Core/SubSystems/ProxyInstance.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/GameState.h"
#include "Kismet/GameplayStatics.h"
#include "Tools/K2Nodes/PromptAsyncAction.h"
#include "Tools/Libraries/ProxyLibrary.h"
#include "User Interfaces/PromptRoot.h"

// ───────────────────────────────────────────────────────────────────────────────
// ─── Construction ──────────────────────────────────────────────────────────────
ADarkPlayerController::ADarkPlayerController()
{
	GuiController = CreateDefaultSubobject<UGuiController>(TEXT("GuiController"));
	check (GuiController);
}

void ADarkPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	ApplyInput("Default");
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
void ADarkPlayerController::ApplyInput(const FName RowName, const int32 PriorityBase, const bool bClearExisting) const
{
	FInputsRow Row;
	if (!ResolveInputsRow(RowName, Row))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Input] ApplyInputRow: Row '%s' not found in Inputs table."), *RowName.ToString());
		return;
	}

	ApplyInputContexts(Row.Inputs, PriorityBase, bClearExisting);
}

void ADarkPlayerController::ApplyInputs(const TArray<FName>& RowNames, const int32 PriorityBase, const bool bClearExisting) const
{
	TArray<UInputMappingContext*> Contexts;
	Contexts.Reserve(8);

	for (const FName& RowName : RowNames)
	{
		FInputsRow Row;
		if (!ResolveInputsRow(RowName, Row))
		{
			UE_LOG(LogTemp, Warning, TEXT("[Input] ApplyInputRows: Row '%s' not found; skipping."), *RowName.ToString());
			continue;
		}

		for (UInputMappingContext* Imc : Row.Inputs)
		{
			if (Imc) Contexts.Add(Imc);
		}
	}

	ApplyInputContexts(Contexts, PriorityBase, bClearExisting);
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
bool ADarkPlayerController::ResolveInputsRow(const FName RowName, FInputsRow& OutRow)
{
	const UDataTable* InputsTable = UTables::GetTable(ETablePath::E_InputsTable);
	if (!InputsTable) return false;
	const FString Ctx(TEXT("Controller_ResolveInputsRow"));
	
	if (const FInputsRow* Direct = InputsTable->FindRow<FInputsRow>(RowName, Ctx))
	{
		OutRow = *Direct;
		return true;
	}
	
	for (TArray<FName> Rows = InputsTable->GetRowNames(); const FName& Rn : Rows)
	{
		if (const FInputsRow* R = InputsTable->FindRow<FInputsRow>(Rn, Ctx))
		{
			if (R->Name == RowName)
			{
				OutRow = *R;
				return true;
			}
		}
	}
	return false;
}

void ADarkPlayerController::ApplyInputContexts(const TArray<UInputMappingContext*>& Contexts, const int32 PriorityBase,
	const bool bClearExisting) const
{
	const ULocalPlayer* Lp = GetLocalPlayer();
	if (!Lp) return;

	UEnhancedInputLocalPlayerSubsystem* Subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Lp);
	if (!Subsys) return;

	FModifyContextOptions Opt;
	Opt.bForceImmediately = true;
	Opt.bIgnoreAllPressedKeysUntilRelease = true;

	if (bClearExisting)
	{
		Subsys->ClearAllMappings();
	}

	int32 Priority = PriorityBase;
	for (const UInputMappingContext* Imc : Contexts)
	{
		if (Imc)
		{
			Subsys->AddMappingContext(Imc, Priority++, Opt);
		}
	}
}

void ADarkPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	check(InputComponent);
	
	InputComponent->BindKey(EKeys::Ampersand, IE_Pressed, this, &ADarkPlayerController::OpenSessionMenu_Request);
	InputComponent->BindKey(EKeys::E_AccentAigu, IE_Pressed, this, &ADarkPlayerController::KickPlayer_Request);
	InputComponent->BindKey(EKeys::Quote, IE_Pressed, this, &ADarkPlayerController::LeaveSession_Request);
}

void ADarkPlayerController::ClientWasKicked_Implementation(const FText& KickReason)
{
	Super::ClientWasKicked_Implementation(KickReason);

	if (const UProxyInstance* Proxy = GetGameInstance()->GetSubsystem<UProxyInstance>())
	{
		Proxy->DestroySession();
	}
	if (UDarkGameInstance* DarkGameInstance = Cast<UDarkGameInstance>(GetGameInstance()))
	{
		DarkGameInstance->OnKicked(KickReason.ToString());
	}	
	ClientTravel(LVL_MENU_PATH, TRAVEL_Absolute);
}

void ADarkPlayerController::SpawnPlayer_Request_Implementation()
{
	ADarkPlayerState* DarkGS = Cast<ADarkPlayerState>(PlayerState);
	if (!DarkGS) return;
	if (IsLocalController())
	{
		DarkGS->SpawnCharacterClass();
	}
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Debug) ─────────────────────────────────────────────────────────
void ADarkPlayerController::OpenSessionMenu_Request()
{
	if (!GetGameInstance()) return;
	if (UNavigationInstance* Nav = GetGameInstance()->GetSubsystem<UNavigationInstance>())
	{
		UClass* PromptClass = StaticLoadClass(UPromptRoot::StaticClass(),nullptr,
	TEXT("/Dark_Script/UserInterface/PromptTest.PromptTest_C"));
		
		if (!PromptClass) return;
		Nav->PromptSuccessResponse.AddDynamic(this, &ADarkPlayerController::OnPrompt_Success);
		Nav->PromptFailureResponse.AddDynamic(this, &ADarkPlayerController::OnPrompt_Fail);
		Nav->OpenPrompt(this, PromptClass, EPromptType::E_None, true);
	}
}

void ADarkPlayerController::OnPrompt_Success(const FString& Value)
{
	UProxyInstance* Proxy = GetGameInstance()->GetSubsystem<UProxyInstance>();
	if (!Proxy) return;
	Proxy->OnCreateSessionCompleted.AddUObject(Proxy, &UProxyInstance::OpenLobby);
	Proxy->CreateSession(TArray<FSessionPropertyKeyPair>(), true);
}

void ADarkPlayerController::OnPrompt_Fail(const FString& Value)
{
	UProxyInstance* Proxy = GetGameInstance()->GetSubsystem<UProxyInstance>();
	if (!Proxy) return;
	Proxy->OnFindSessionsCompleted.AddLambda(
	[this](const bool bSuccess, const TArray<FBlueprintSessionResult>& Results)
	{
		if (!bSuccess || Results.Num() == 0) return;

		// Récupérer le OnlineSubsystem
		const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
		if (!Subsystem) return;

		IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
		if (!Session.IsValid()) return;

		// Join du premier résultat
		const FOnlineSessionSearchResult& First = Results[0].OnlineResult;

		Session->JoinSession(
			0,                          // Local user index
			NAME_GameSession,           // Nom standard de la session
			First                       // Résultat
		);
	}
);
	Proxy->FindSessions(TArray<FSessionPropertyKeyPair>(), ESessionVisibility::E_Public);
}

void ADarkPlayerController::KickPlayer_Request()
{
	const UProxyInstance* Proxy = GetGameInstance()->GetSubsystem<UProxyInstance>();
	if (!Proxy) return;
	if (Proxy->IsServer())
	{		
		if (GetWorld())
		{
			if (const AGameModeBase* GameMode = GetWorld()->GetAuthGameMode())
			{
				if (GameMode->GameSession)
				{
					AGameState* GS = Cast<AGameState>(UGameplayStatics::GetGameState(GetWorld()));
					if (!GS) return;
					if (GS->PlayerArray.Num() < 2) return;
					const APlayerState* PS = GS->PlayerArray[1];
					if (!PS) return;
					APlayerController* PC = PS->GetPlayerController();
					if (!PC) return;
					GameMode->GameSession->KickPlayer(PC, FText::FromString("Kick by Host"));
				}
			}
		}
	}
}

void ADarkPlayerController::LeaveSession_Request()
{
	UProxyInstance* Proxy = GetGameInstance()->GetSubsystem<UProxyInstance>();
	if (!Proxy) return;
	Proxy->OnLeaveSessionCompleted.AddWeakLambda(this, [this](const bool bSuccess)
	{
		if (!bSuccess) return;
		
		UGameplayStatics::OpenLevel(this, FName(LVL_MENU_PATH), true);
	});
	Proxy->LeaveSession();
}


