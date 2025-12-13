#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/SaveableInterface.h"
#include "Utils/Types/OnlineTypes.h"
#include "Progression.generated.h"


UCLASS(ClassGroup=(DarkScript), meta=(BlueprintSpawnableComponent))
class DARKSCRIPT_API UProgression : public UActorComponent, public ISaveableInterface
{
	GENERATED_BODY()

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Delegates ─────────────────────────────────────────────────────────────────
	DECLARE_DELEGATE_TwoParams(FOnPlayerDataSync, EClassArchetype, int32);
	DECLARE_DELEGATE_OneParam(FOnArchetypeChanged, EClassArchetype);
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	UProgression();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	virtual bool SaveToPlayer_Implementation(UPlayerSaveGame* SaveGame, const FString& Context) override;
	virtual bool LoadFromPlayer_Implementation(UPlayerSaveGame* SaveGame, const FString& Context) override;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	APlayerController* GetPlayerController() const { return Cast<APlayerController>(GetOwner()); }
	bool IsLocalController() const { return GetPlayerController() ? GetPlayerController()->IsLocalController() : false; }
	USaveSystem* GetSaveSystem() const { return GetPlayerController() ? GetPlayerController()->GetGameInstance()->GetSubsystem<USaveSystem>() : nullptr; }
	
	UFUNCTION(BlueprintPure, Category="Gameplay")
	EClassArchetype GetClassArchetype() const { return Archetype; }
	UFUNCTION(BlueprintPure, Category="Gameplay")
	int32 GetPlayerLevel() const { return PlayerLevel; }
	
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (replicates) ────────────────────────────────────────────────────
	UFUNCTION()
	void OnRep_PlayerSave();
	UFUNCTION(Server, Reliable)
	void Server_SynchronizeProperties(const EClassArchetype InArchetype, const int32 InLevel);
	UFUNCTION(Server, Reliable)
	void SetClassArchetype(const EClassArchetype NewArchetype);
	UFUNCTION(Server, Reliable)
	void SetPlayerLevel(const int32 NewLevel);
	
	UFUNCTION(Client, Reliable)
	void ClientLoadAndSendPlayerData();
	

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	FOnPlayerDataSync OnPlayerDataSync;
	FOnArchetypeChanged OnArchetypeChanged;
	
private:
	UPROPERTY(ReplicatedUsing=OnRep_PlayerSave)
	EClassArchetype Archetype = EClassArchetype::Tank;
	UPROPERTY(ReplicatedUsing=OnRep_PlayerSave)
	int32 PlayerLevel = 1;
};
