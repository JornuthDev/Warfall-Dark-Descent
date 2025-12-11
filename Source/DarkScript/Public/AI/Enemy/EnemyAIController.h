// Copyright 2024 DarkScript. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "EnemyAIController.generated.h"

class AEnemyCharacter;
class UBehaviorTree;
class UBlackboardComponent;

/**
 * AI Controller for standard enemies
 * - Manages AI perception (sight + hearing)
 * - Initializes blackboard from enemy data
 * - Runs behavior tree automatically
 * - Handles target tracking and threat management
 */
UCLASS()
class DARKSCRIPT_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	//~ Perception
	
	/** Sight sense configuration */
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	/** Hearing sense configuration */
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;

	/** Called when perception is updated */
	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
public:
	/** Initialize perception settings from enemy data */
	void InitializePerception();

	/** Initialize blackboard values from enemy data */
	void SetupBlackboardValues();

	/** Start behavior tree */
	void StartBehaviorTree();
	
protected:
	//~ Blackboard & Behavior Tree
	/** Shared blackboard asset for standard enemies */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Behavior")
	TObjectPtr<UBlackboardData> BlackboardAsset;

	/** Behavior tree to run */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Behavior")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;
	
	//~ Target Management
	/** Current target actor */
	UPROPERTY(BlueprintReadOnly, Category = "AI|Combat")
	TObjectPtr<AActor> CurrentTarget;

	/** Set the current target and update blackboard */
	UFUNCTION(BlueprintCallable, Category = "AI|Combat")
	void SetTarget(AActor* NewTarget);

	/** Clear the current target */
	UFUNCTION(BlueprintCallable, Category = "AI|Combat")
	void ClearTarget();

	/** Get the current target */
	UFUNCTION(BlueprintPure, Category = "AI|Combat")
	AActor* GetTarget() const { return CurrentTarget; }

	//~ Enemy Reference
	/** Cached reference to controlled enemy */
	UPROPERTY(BlueprintReadOnly, Category = "AI|Enemy")
	TObjectPtr<AEnemyCharacter> ControlledEnemy;

	//~ Combat State
	/** Is this AI in combat? */
	UPROPERTY(BlueprintReadOnly, Category = "AI|Combat")
	bool bIsInCombat;

	/** Enter combat state */
	UFUNCTION(BlueprintCallable, Category = "AI|Combat")
	void EnterCombat();

	/** Exit combat state */
	UFUNCTION(BlueprintCallable, Category = "AI|Combat")
	void ExitCombat();

public:
	//~ Getters
	UFUNCTION(BlueprintPure, Category = "AI|Combat")
	bool IsInCombat() const { return bIsInCombat; }

	UFUNCTION(BlueprintPure, Category = "AI|Enemy")
	AEnemyCharacter* GetControlledEnemy() const { return ControlledEnemy; }
};