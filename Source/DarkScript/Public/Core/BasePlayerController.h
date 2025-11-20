#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/SaveableInterface.h"
#include "Tools/Types/ProfileTypes.h"
#include "BasePlayerController.generated.h"

enum class EPlayerArchetype : uint8;
/**
 * 
 */
UCLASS()
class DARKSCRIPT_API ABasePlayerController : public APlayerController, public ISaveableInterface
{
	GENERATED_BODY()
	
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	UFUNCTION(BlueprintPure, Category="PlayerProfile")
	EPlayerArchetype GetPlayedArchetype() const { return PlayedArchetype; }
		
	UFUNCTION(BlueprintCallable, Category="Input|Enhanced")
	void ApplyInputs(FName RowName);
	UFUNCTION(BlueprintCallable, Category="Input|Enhanced")
	void ApplyInputsArray(const TArray<FName>& RowNames);
	
	virtual bool SaveToPlayerProfile_Implementation(UPlayerProfile* SaveGame, const FString& Context) override;
	virtual bool LoadToPlayerProfile_Implementation(const UPlayerProfile* SaveGame, const FString& Context) override;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	UFUNCTION(Server, Reliable)
	void SynchronizeProperties_Internal(EPlayerArchetype InArchetype, int32 InPlayerLevel);
	UFUNCTION(Server,Reliable)
	void SwitchArchetype();
	UFUNCTION(Client, Reliable)
	void Call_SaveGame(const bool bIsPlayerProfile = true);
	
	virtual void SetupInputComponent() override;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── OnRep_Functions (Internal) ────────────────────────────────────────────────
	UFUNCTION()
	void OnRep_PlayedArchetype();
	
private:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_PlayedArchetype)
	EPlayerArchetype PlayedArchetype = EPlayerArchetype::Tank;
	UPROPERTY(VisibleAnywhere, Replicated)
	int32 PlayerLevel = 1;
};
