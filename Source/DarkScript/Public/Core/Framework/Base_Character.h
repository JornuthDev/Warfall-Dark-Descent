#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/SystemsInterface.h"
#include "AbilitySystemInterface.h"
#include "Utils/Types/PlayerTypes.h"
#include "Base_Character.generated.h"

class UDefaultAttributeSet;
enum class EClassArchetype : uint8;

UCLASS()
class DARKSCRIPT_API ABase_Character : public ACharacter, public ISystemsInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// ═══════════════════════════════════════════════════════════════════════
	// CONSTRUCTEUR
	// ═══════════════════════════════════════════════════════════════════════
	
	ABase_Character();
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Blueprint API)
	// ═══════════════════════════════════════════════════════════════════════
    
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UGlobalAbilitySystem* GetGlobalAbilitySystem_Implementation() override;
	virtual UDefaultAbilitySystem* GetDefaultAbilitySystem_Implementation() override;
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Helper)
	// ═══════════════════════════════════════════════════════════════════════
	
	void InitAbilitySystem(const EClassArchetype Archetype = EClassArchetype::None, const FName RowName = NAME_None);

	
protected:
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay|Ability")
	TObjectPtr<UDefaultAbilitySystem> AbilitySystem;
	
	UPROPERTY()
	TObjectPtr<UDefaultAttributeSet> DefaultAttributes;
};
