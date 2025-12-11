// Copyright 2024 DarkScript. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveToTarget.generated.h"

/**
 * Custom BT Task - Move to an Actor target
 * Unlike the default MoveTo task, this accepts an Actor object directly
 * Supports tracking moving targets
 */
UCLASS()
class DARKSCRIPT_API UBTTask_MoveToTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MoveToTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	uint16 GetInstanceMemorySize() const;

protected:
	/** Blackboard key selector for target actor */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	/** Acceptable radius to target */
	UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "0.0"))
	float AcceptableRadius = 200.0f;

	/** Stop on overlap? */
	UPROPERTY(EditAnywhere, Category = "Movement")
	bool bStopOnOverlap = true;

	/** Use pathfinding? */
	UPROPERTY(EditAnywhere, Category = "Movement")
	bool bUsePathfinding = true;

	/** Can strafe? */
	UPROPERTY(EditAnywhere, Category = "Movement")
	bool bCanStrafe = false;

	/** Allow partial path? */
	UPROPERTY(EditAnywhere, Category = "Movement")
	bool bAllowPartialPath = true;

	/** Track moving target? (continuously update path) */
	UPROPERTY(EditAnywhere, Category = "Movement")
	bool bTrackMovingGoal = true;

	/** Minimum distance target must move before updating path */
	UPROPERTY(EditAnywhere, Category = "Movement", meta = (EditCondition = "bTrackMovingGoal", ClampMin = "10.0"))
	float TargetMovementThreshold = 50.0f;
};