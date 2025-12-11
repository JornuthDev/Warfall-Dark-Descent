// Copyright 2024 DarkScript. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PatrolWait.generated.h"

/**
 * Wait task that reads idle time from Enemy Data
 * Uses IdleTimeMin and IdleTimeMax from DataTable
 */
UCLASS()
class DARKSCRIPT_API UBTTask_PatrolWait : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_PatrolWait();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	uint16 GetInstanceMemorySize() const;

protected:
	/** Fallback wait time if no enemy data (seconds) */
	UPROPERTY(EditAnywhere, Category = "Wait", meta = (ClampMin = "0.1"))
	float FallbackWaitTime = 3.0f;

	/** Fallback random deviation if no enemy data */
	UPROPERTY(EditAnywhere, Category = "Wait", meta = (ClampMin = "0.0"))
	float FallbackRandomDeviation = 2.0f;
};