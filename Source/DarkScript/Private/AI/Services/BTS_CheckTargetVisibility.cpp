// Copyright 2024 DarkScript. All Rights Reserved.

#include "AI/Services/BTS_CheckTargetVisibility.h"
#include "AIController.h"
#include "AI/Enemy/EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "GameFramework/Actor.h"

// Memory to track time since target was last seen
struct FBTS_CheckTargetMemory
{
	float TimeSinceLastSeen = 0.0f;
};

UBTS_CheckTargetVisibility::UBTS_CheckTargetVisibility()
{
	NodeName = "Check Target Visibility";
	
	// Tick every 0.5 seconds (performance)
	Interval = 0.5f;
	RandomDeviation = 0.1f;

	// Setup key filters
	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_CheckTargetVisibility, TargetActorKey), UObject::StaticClass());
	LastKnownLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_CheckTargetVisibility, LastKnownLocationKey));

	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
}

void UBTS_CheckTargetVisibility::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return;
	}

	// Get target from blackboard
	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValue<UBlackboardKeyType_Object>(TargetActorKey.SelectedKeyName));
	if (!TargetActor)
	{
		// No target, nothing to check
		return;
	}

	// Get memory
	FBTS_CheckTargetMemory* Memory = CastInstanceNodeMemory<FBTS_CheckTargetMemory>(NodeMemory);
	if (!Memory)
	{
		return;
	}

	// Try to get timeout from EnemyCharacter data
	float EffectiveTimeout = LostTargetTimeout; // Default fallback
	
	if (const AEnemyCharacter* EnemyChar = Cast<AEnemyCharacter>(AIController->GetPawn()))
	{
		if (const FStandardEnemyRow* EnemyData = EnemyChar->GetEnemyData())
		{
			EffectiveTimeout = EnemyData->LoseSightDuration;
		}
	}

	// Check if we can still see the target
	UAIPerceptionComponent* PerceptionComp = AIController->GetPerceptionComponent();
	bool bCanSeeTarget = false;

	if (PerceptionComp)
	{
		FActorPerceptionBlueprintInfo Info;
		PerceptionComp->GetActorsPerception(TargetActor, Info);

		// Check if sight sense is active and successfully sensed
		for (const FAIStimulus& Stimulus : Info.LastSensedStimuli)
		{
			if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
			{
				if (Stimulus.WasSuccessfullySensed())
				{
					bCanSeeTarget = true;
					break;
				}
			}
		}
	}

	if (bCanSeeTarget)
	{
		// Can see target - reset timer
		Memory->TimeSinceLastSeen = 0.0f;

		// Update last known location
		if (bUpdateLastKnownLocation)
		{
			BlackboardComp->SetValue<UBlackboardKeyType_Vector>(LastKnownLocationKey.SelectedKeyName, TargetActor->GetActorLocation());
		}
	}
	else
	{
		// Can't see target - increment timer
		Memory->TimeSinceLastSeen += DeltaSeconds;

		// Check timeout
		if (Memory->TimeSinceLastSeen >= EffectiveTimeout)
		{
			UE_LOG(LogTemp, Log, TEXT("BTS_CheckTargetVisibility: Target lost for %.1fs, clearing target"), Memory->TimeSinceLastSeen);
			
			// Clear target from blackboard
			BlackboardComp->ClearValue(TargetActorKey.SelectedKeyName);
			
			// Reset timer
			Memory->TimeSinceLastSeen = 0.0f;
		}
	}
}

uint16 UBTS_CheckTargetVisibility::GetInstanceMemorySize() const
{
	return sizeof(FBTS_CheckTargetMemory);
}