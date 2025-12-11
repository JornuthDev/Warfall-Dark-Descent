// Copyright 2024 DarkScript. All Rights Reserved.

#include "AI/Enemy/EnemyAIController.h"
#include "AI/Enemy/EnemyCharacter.h"
#include "AI/Enemy/EnemyData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"

AEnemyAIController::AEnemyAIController()
{
	UAIPerceptionComponent* PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnPerceptionUpdated);
	SetPerceptionComponent(*PerceptionComp);
	
	// Create sight config
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1200.0f;
	SightConfig->LoseSightRadius = 1500.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	PerceptionComponent->ConfigureSense(*SightConfig);

	// Create hearing config
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = 800.0f;
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;
	PerceptionComponent->ConfigureSense(*HearingConfig);
	
	// Load Behavior Tree asset
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BehaviorTreeObj(TEXT("/DarkScript/AI/Enemy/BT_StandardEnemy"));
	if (BehaviorTreeObj.Succeeded())
	{
		BehaviorTreeAsset = BehaviorTreeObj.Object;
	}
	
	// Load Blackboard Asset
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BlackboardObj(TEXT("/DarkScript/AI/Enemy/BB_StandardEnemy"));
	if (BlackboardObj.Succeeded())
	{
		BlackboardAsset = BlackboardObj.Object;
	}
	
	// Initialize defaults
	bIsInCombat = false;
	CurrentTarget = nullptr;
	ControlledEnemy = nullptr;
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	// Perception will be initialized in OnPossess when we have enemy data
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Cache enemy reference
	ControlledEnemy = Cast<AEnemyCharacter>(InPawn);
	if (!ControlledEnemy)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ EnemyAIController possessed non-EnemyCharacter pawn!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("🤖 AI Controller possessing: %s"), *ControlledEnemy->GetName());
}

void AEnemyAIController::InitializePerception()
{
	if (!ControlledEnemy)
	{
		return;
	}

	// Safety check: PerceptionComponent exists (from AAIController)
	if (!PerceptionComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ AI Controller: No PerceptionComponent from parent!"));
		return;
	}

	// Get enemy data from the character
	const FStandardEnemyRow* EnemyData = ControlledEnemy->GetEnemyData();
	if (!EnemyData)
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ AI Controller: No enemy data for perception setup"));
		return;
	}

	// Configure sight from data table
	SightConfig->SightRadius = EnemyData->SightRadius;
	SightConfig->LoseSightRadius = EnemyData->SightRadius;
	SightConfig->PeripheralVisionAngleDegrees = EnemyData->PeripheralVisionAngle;
	PerceptionComponent->ConfigureSense(*SightConfig);

	// Configure hearing from data table
	HearingConfig->HearingRange = EnemyData->HearingRadius;
	PerceptionComponent->ConfigureSense(*HearingConfig);

	// Set dominant sense
	PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

	// Bind perception event (if not already bound)
	if (!PerceptionComponent->OnTargetPerceptionUpdated.IsBound())
	{
		PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnPerceptionUpdated);
	}

	UE_LOG(LogTemp, Log, TEXT("  👁️ Perception: Sight=%.0f, Hearing=%.0f, FOV=%.0f°"),
		EnemyData->SightRadius,
		EnemyData->HearingRadius,
		EnemyData->PeripheralVisionAngle);
}

void AEnemyAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor)
	{
		return;
	}

	// Only care about successfully sensed stimuli
	if (!Stimulus.WasSuccessfullySensed())
	{
		// Lost sight/hearing of actor
		if (CurrentTarget == Actor)
		{
			UE_LOG(LogTemp, Log, TEXT("  👁️ Lost perception of target: %s"), *Actor->GetName());
			// Don't clear target immediately - let BT decide based on last known location
		}
		return;
	}

	// Check if this is a valid target (player or other enemy based on tags)
	// For now, we'll assume any perceived actor is a potential target
	if (!CurrentTarget)
	{
		UE_LOG(LogTemp, Log, TEXT("  👁️ Detected actor: %s (Distance: %.0f)"),
			*Actor->GetName(),
			FVector::Dist(GetPawn()->GetActorLocation(), Actor->GetActorLocation()));

		SetTarget(Actor);
		EnterCombat();
	}
}

void AEnemyAIController::SetupBlackboardValues()
{
	if (!BlackboardAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ No Blackboard asset assigned to EnemyAIController!"));
		return;
	}

	// Initialize blackboard
	if (UBlackboardComponent* BlackboardPtr = nullptr; UseBlackboard(BlackboardAsset, BlackboardPtr))
	{
		Blackboard = BlackboardPtr;
		UE_LOG(LogTemp, Log, TEXT("  🧠 Blackboard initialized"));

		// Set initial values
		if (Blackboard)
		{
			// Combat state
			Blackboard->SetValueAsBool("IsInCombat", bIsInCombat);
			
			// Resource info (from character)
			if (ControlledEnemy)
			{
				Blackboard->SetValueAsFloat("CurrentResource", ControlledEnemy->GetCurrentResource());
				Blackboard->SetValueAsFloat("MaxResource", ControlledEnemy->GetMaxResource());
				Blackboard->SetValueAsFloat("ResourcePercent", ControlledEnemy->GetResourcePercent());
			}

			UE_LOG(LogTemp, Log, TEXT("  🧠 Blackboard values initialized"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Failed to initialize Blackboard!"));
	}
}

void AEnemyAIController::StartBehaviorTree()
{
	if (!BehaviorTreeAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ No Behavior Tree asset assigned to EnemyAIController!"));
		return;
	}

	if (RunBehaviorTree(BehaviorTreeAsset))
	{
		UE_LOG(LogTemp, Log, TEXT("  🌳 Behavior Tree started successfully"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Failed to start Behavior Tree!"));
	}
}

void AEnemyAIController::SetTarget(AActor* NewTarget)
{
	if (CurrentTarget == NewTarget)
	{
		return;
	}

	CurrentTarget = NewTarget;

	// Update blackboard
	if (Blackboard)
	{
		Blackboard->SetValueAsObject("TargetActor", CurrentTarget);
		
		if (CurrentTarget)
		{
			Blackboard->SetValueAsVector("LastKnownTargetLocation", CurrentTarget->GetActorLocation());
		}
	}

	if (CurrentTarget)
	{
		UE_LOG(LogTemp, Log, TEXT("  🎯 Target set: %s"), *CurrentTarget->GetName());
	}
}

void AEnemyAIController::ClearTarget()
{
	CurrentTarget = nullptr;

	if (Blackboard)
	{
		Blackboard->ClearValue("TargetActor");
	}

	UE_LOG(LogTemp, Log, TEXT("  🎯 Target cleared"));
}

void AEnemyAIController::EnterCombat()
{
	if (bIsInCombat)
	{
		return;
	}

	bIsInCombat = true;

	// Update blackboard
	if (Blackboard)
	{
		Blackboard->SetValueAsBool("IsInCombat", true);
	}

	// Notify controlled enemy
	if (ControlledEnemy)
	{
		ControlledEnemy->SetInCombat(true);
	}

	UE_LOG(LogTemp, Log, TEXT("  ⚔️ Entered combat"));
}

void AEnemyAIController::ExitCombat()
{
	if (!bIsInCombat)
	{
		return;
	}

	bIsInCombat = false;

	// Update blackboard
	if (Blackboard)
	{
		Blackboard->SetValueAsBool("IsInCombat", false);
	}

	// Notify controlled enemy
	if (ControlledEnemy)
	{
		ControlledEnemy->SetInCombat(false);
	}

	UE_LOG(LogTemp, Log, TEXT("  🛡️ Exited combat"));
}