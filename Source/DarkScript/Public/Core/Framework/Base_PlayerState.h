#pragma once
#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/SaveInterface.h"
#include "Interfaces/SystemsInterface.h"
#include "Utils/Types/SaveTypes.h"
#include "Base_PlayerState.generated.h"

class UGlobalAttributeSet;
class UGlobalAbilitySystem;
enum class EClassArchetype : uint8;
/**
 * 
 */
UCLASS()
class DARKSCRIPT_API ABase_PlayerState : public APlayerState, 
	public ISaveInterface,
	public ISystemsInterface,
	public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	// ═══════════════════════════════════════════════════════════════════════
	// CONSTRUCTEUR
	// ═══════════════════════════════════════════════════════════════════════
    
	ABase_PlayerState();
	virtual void BeginPlay() override;
	void InitAbilitySystem();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Blueprint API)
	// ═══════════════════════════════════════════════════════════════════════
    
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UGlobalAbilitySystem* GetGlobalAbilitySystem_Implementation() override;
	virtual UDefaultAbilitySystem* GetDefaultAbilitySystem_Implementation() override;
	
	virtual bool SaveToPlayer_Implementation(UPlayerSave* SaveGame, const FString& Context) override;
	virtual bool LoadFromPlayer_Implementation(UPlayerSave* SaveGame, const FString& Context) override;
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Replicates)
	// ═══════════════════════════════════════════════════════════════════════
	
	UFUNCTION(Server, Reliable)
	void Server_InitAbiltiySystem();
	void Server_InitAbiltiySystem_Implementation();
	
	UFUNCTION(Server, Reliable)
	void Server_LoadFromPlayer(const FGlobalAbilitySave& AbilityLoad, const FString& Context);
	void Server_LoadFromPlayer_Implementation(const FGlobalAbilitySave& AbilityLoad, const FString& Context);
	
	UFUNCTION()
	virtual void OnRep_ActifArchetype();
	
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════
	UPROPERTY(ReplicatedUsing = OnRep_ActifArchetype, BlueprintReadOnly, Category = "Gameplay|Ability")
	EClassArchetype ActifArchetype;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay|Ability")
	TObjectPtr<UGlobalAbilitySystem> AbilitySystem;
	
	UPROPERTY()
	TObjectPtr<UGlobalAttributeSet> Attributes;
};
