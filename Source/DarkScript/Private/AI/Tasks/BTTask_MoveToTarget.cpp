// Copyright 2024 DarkScript. All Rights Reserved.

#include "AI/Tasks/BTTask_MoveToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "GameFramework/Actor.h"
#include "Navigation/PathFollowingComponent.h"

// Memory struct to track last target location
struct FBTMoveToTargetMemory
{
	FVector LastTargetLocation;
	TWeakObjectPtr<AActor> LastTargetActor;
};

UBTTask_MoveToTarget::UBTTask_MoveToTarget()
{
	NodeName = "Move To Target Actor";
	
	// Accept only Object keys (Actor)
	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_MoveToTarget, TargetActorKey), UObject::StaticClass());
	
	// Task settings
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = true; // Create instance to store memory
}

EBTNodeResult::Type UBTTask_MoveToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	// Get target actor from blackboard
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValue<UBlackboardKeyType_Object>(TargetActorKey.SelectedKeyName));
	if (!TargetActor)
	{
		return EBTNodeResult::Failed;
	}

	// Initialize memory
	FBTMoveToTargetMemory* Memory = CastInstanceNodeMemory<FBTMoveToTargetMemory>(NodeMemory);
	if (Memory)
	{
		Memory->LastTargetLocation = TargetActor->GetActorLocation();
		Memory->LastTargetActor = TargetActor;
	}

	// Move to target actor
	EPathFollowingRequestResult::Type MoveResult = AIController->MoveToActor(
		TargetActor,
		AcceptableRadius,
		bStopOnOverlap,
		bUsePathfinding,
		bCanStrafe,
		nullptr, // Filter class
		bAllowPartialPath
	);

	// Handle result
	switch (MoveResult)
	{
	case EPathFollowingRequestResult::RequestSuccessful:
		// Keep task running to track target
		return EBTNodeResult::InProgress;

	case EPathFollowingRequestResult::AlreadyAtGoal:
		return EBTNodeResult::Succeeded;

	case EPathFollowingRequestResult::Failed:
	default:
		return EBTNodeResult::Failed;
	}
}

void UBTTask_MoveToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!bTrackMovingGoal)
	{
		return;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// Get current target
	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValue<UBlackboardKeyType_Object>(TargetActorKey.SelectedKeyName));
	
	// Check if target is still valid
	if (!TargetActor || !TargetActor->IsValidLowLevel())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// Get memory
	FBTMoveToTargetMemory* Memory = CastInstanceNodeMemory<FBTMoveToTargetMemory>(NodeMemory);
	if (!Memory)
	{
		return;
	}

	// Check if target has moved significantly
	FVector CurrentTargetLocation = TargetActor->GetActorLocation();
	float DistanceMoved = FVector::Dist(Memory->LastTargetLocation, CurrentTargetLocation);

	if (DistanceMoved > TargetMovementThreshold)
	{
		// Target moved significantly - update path
		
		Memory->LastTargetLocation = CurrentTargetLocation;

		// Request new move
		EPathFollowingRequestResult::Type MoveResult = AIController->MoveToActor(
			TargetActor,
			AcceptableRadius,
			bStopOnOverlap,
			bUsePathfinding,
			bCanStrafe,
			nullptr,
			bAllowPartialPath
		);

		if (MoveResult == EPathFollowingRequestResult::Failed)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			return;
		}
	}

	// Check if we reached the target
	UPathFollowingComponent* PathFollowingComp = AIController->GetPathFollowingComponent();
	if (PathFollowingComp && PathFollowingComp->DidMoveReachGoal())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UBTTask_MoveToTarget::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if (AAIController* AIController = OwnerComp.GetAIOwner())
	{
		// Stop movement
		AIController->StopMovement();
	}

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

uint16 UBTTask_MoveToTarget::GetInstanceMemorySize() const
{
	return sizeof(FBTMoveToTargetMemory);
}