// Copyright 2024 DarkScript. All Rights Reserved.

#include "AI/Tasks/BTTask_FindPatrolPoint.h"
#include "AI/Enemy/EnemyCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "NavigationSystem.h"

UBTTask_FindPatrolPoint::UBTTask_FindPatrolPoint()
{
	NodeName = "Find Patrol Point";
	
	// Setup key filter
	PatrolPointKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindPatrolPoint, PatrolPointKey));
	
	// This task executes instantly
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_FindPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	// Get spawn location
	FVector SpawnLocation;
	
	// Try to get patrol radius from EnemyCharacter data
	float EffectivePatrolRadius = PatrolRadius; // Default fallback
	
	if (const AEnemyCharacter* EnemyChar = Cast<AEnemyCharacter>(ControlledPawn))
	{
		const FStandardEnemyRow* EnemyData = EnemyChar->GetEnemyData();
		 SpawnLocation = EnemyChar ? EnemyChar->GetSpawnLocation() : ControlledPawn->GetActorLocation();
		if (EnemyData)
		{
			EffectivePatrolRadius = EnemyData->PatrolRadius;
			UE_LOG(LogTemp, Log, TEXT("BTTask_FindPatrolPoint: Using patrol radius from data: %.0f"), EffectivePatrolRadius);
		}
	}
	
	FVector PatrolPoint = FVector::ZeroVector;
	bool bFoundPoint = false;

	if (bUseNavMesh)
	{
		// Use Navigation System to find valid point
		if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
		{
			// Try multiple times to find a valid point
			for (int32 i = 0; i < MaxAttempts; ++i)
			{
				// Random point in radius
				FVector RandomDirection = FMath::VRand();
				RandomDirection.Z = 0.0f; // Keep on ground plane
				RandomDirection.Normalize();
				
				float RandomDistance = FMath::FRandRange(EffectivePatrolRadius * 0.75f, EffectivePatrolRadius);
				FVector TestPoint = SpawnLocation + (RandomDirection * RandomDistance);

				// Project to navmesh
				FNavLocation NavLocation;
				if (NavSys->ProjectPointToNavigation(TestPoint, NavLocation, FVector(500.0f, 500.0f, 500.0f)))
				{
					PatrolPoint = NavLocation.Location;
					bFoundPoint = true;
					break;
				}
			}
		}

		if (!bFoundPoint)
		{
			UE_LOG(LogTemp, Warning, TEXT("BTTask_FindPatrolPoint: Failed to find valid NavMesh point after %d attempts"), MaxAttempts);
			return EBTNodeResult::Failed;
		}
	}
	else
	{
		// Simple random point (no navmesh check)
		FVector RandomDirection = FMath::VRand();
		RandomDirection.Z = 0.0f;
		RandomDirection.Normalize();
		
		float RandomDistance = FMath::FRandRange(EffectivePatrolRadius * 0.3f, EffectivePatrolRadius);
		PatrolPoint = SpawnLocation + (RandomDirection * RandomDistance);
		PatrolPoint.Z = SpawnLocation.Z; // Keep same height
		
		bFoundPoint = true;
	}

	if (bFoundPoint)
	{
		// Store in blackboard
		BlackboardComp->SetValue<UBlackboardKeyType_Vector>(PatrolPointKey.SelectedKeyName, PatrolPoint);
		
		UE_LOG(LogTemp, Log, TEXT("BTTask_FindPatrolPoint: Found patrol point at %s (%.0f units from spawn)"),
			*PatrolPoint.ToCompactString(),
			FVector::Dist(SpawnLocation, PatrolPoint));
		
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}