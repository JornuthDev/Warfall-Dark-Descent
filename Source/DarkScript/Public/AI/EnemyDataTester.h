// DDSEnemyDataTester.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AI/EnemyData.h"
#include "EnemyDataTester.generated.h"

UCLASS()
class DARKSCRIPT_API AEnemyDataTester : public AActor
{
	GENERATED_BODY()

public:
	AEnemyDataTester();

	/** Reference à la DataTable */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Testing")
	UDataTable* EnemyDataTable;

	/** Palier actuel pour tester le scaling */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Testing", Meta = (ClampMin = "1", ClampMax = "30"))
	int32 CurrentFloor = 1;

	/** Test : Lire une row et afficher toutes les infos */
	UFUNCTION(BlueprintCallable, Category = "Testing")
	void TestReadEnemyData(FName RowName);

	/** Test : Calculer les stats scalées pour un palier donné */
	UFUNCTION(BlueprintCallable, Category = "Testing")
	void TestScaledStats(FName RowName, int32 FloorLevel);

protected:
	virtual void BeginPlay() override;
};