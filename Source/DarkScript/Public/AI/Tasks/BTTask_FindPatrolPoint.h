// Copyright 2024 DarkScript. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindPatrolPoint.generated.h"

/**
 * Finds a random patrol point around the enemy's spawn location
 * Stores result in blackboard as a Vector
 */
UCLASS()
class DARKSCRIPT_API UBTTask_FindPatrolPoint : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindPatrolPoint();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	/** Blackboard key to store the patrol point */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PatrolPointKey;

	/** Patrol radius around spawn location */
	UPROPERTY(EditAnywhere, Category = "Patrol", meta = (ClampMin = "100.0"))
	float PatrolRadius = 500.0f;

	/** Use NavMesh for patrol point (recommended) */
	UPROPERTY(EditAnywhere, Category = "Patrol")
	bool bUseNavMesh = true;

	/** Max attempts to find valid point */
	UPROPERTY(EditAnywhere, Category = "Patrol", meta = (ClampMin = "1", ClampMax = "20"))
	int32 MaxAttempts = 10;
};