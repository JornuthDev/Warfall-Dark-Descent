// Copyright 2024 DarkScript. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_CheckTargetVisibility.generated.h"

/**
 * Service that checks if the target is still visible
 * Clears target from blackboard if lost for too long
 */
UCLASS()
class DARKSCRIPT_API UBTS_CheckTargetVisibility : public UBTService
{
	GENERATED_BODY()

public:
	UBTS_CheckTargetVisibility();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	uint16 GetInstanceMemorySize() const;

	/** Blackboard key for target actor */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	/** How long to keep target after losing sight (seconds) */
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = "0.0"))
	float LostTargetTimeout = 5.0f;

	/** Update last known location in blackboard */
	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bUpdateLastKnownLocation = true;

	/** Blackboard key for last known location */
	UPROPERTY(EditAnywhere, Category = "Blackboard", meta = (EditCondition = "bUpdateLastKnownLocation"))
	FBlackboardKeySelector LastKnownLocationKey;
};