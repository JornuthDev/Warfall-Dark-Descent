// Copyright 2024 DarkScript. All Rights Reserved.

#include "AI/Tasks/BTTask_PatrolWait.h"
#include "AI/Enemy/EnemyCharacter.h"
#include "AIController.h"

// Memory to track wait time
struct FBTPatrolWaitMemory
{
	float RemainingWaitTime = 0.0f;
};

UBTTask_PatrolWait::UBTTask_PatrolWait()
{
	NodeName = "Patrol Wait";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_PatrolWait::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	// Get wait time from enemy data
	float MinTime = FallbackWaitTime;
	float MaxTime = FallbackWaitTime + FallbackRandomDeviation;

	if (const AEnemyCharacter* EnemyChar = Cast<AEnemyCharacter>(ControlledPawn))
	{
		if (const FStandardEnemyRow* EnemyData = EnemyChar->GetEnemyData())
		{
			MinTime = EnemyData->IdleTimeMin;
			MaxTime = EnemyData->IdleTimeMax;
			UE_LOG(LogTemp, Log, TEXT("BTTask_PatrolWait: Using idle time from data: %.1f-%.1fs"), MinTime, MaxTime);
		}
	}

	// Calculate random wait time
	float WaitTime = FMath::FRandRange(MinTime, MaxTime);

	// Store in memory
	FBTPatrolWaitMemory* Memory = CastInstanceNodeMemory<FBTPatrolWaitMemory>(NodeMemory);
	if (Memory)
	{
		Memory->RemainingWaitTime = WaitTime;
	}

	UE_LOG(LogTemp, Log, TEXT("BTTask_PatrolWait: Waiting for %.1fs"), WaitTime);

	return EBTNodeResult::InProgress;
}

void UBTTask_PatrolWait::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTPatrolWaitMemory* Memory = CastInstanceNodeMemory<FBTPatrolWaitMemory>(NodeMemory);
	if (!Memory)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// Countdown
	Memory->RemainingWaitTime -= DeltaSeconds;

	if (Memory->RemainingWaitTime <= 0.0f)
	{
		// Wait finished
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

uint16 UBTTask_PatrolWait::GetInstanceMemorySize() const
{
	return sizeof(FBTPatrolWaitMemory);
}