#pragma once

#include "CoreMinimal.h"
#include "Core/Framework/Base_Character.h"
#include "PlayerCharacter.generated.h"

struct FOnAttributeChangeData;

UCLASS()
class DARKSCRIPT_API APlayerCharacter : public ABase_Character
{
	GENERATED_BODY()

public:
	// ═══════════════════════════════════════════════════════════════════════
	// CONSTRUCTEUR
	// ═══════════════════════════════════════════════════════════════════════
	
	APlayerCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Blueprint API)
	// ═══════════════════════════════════════════════════════════════════════
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Helpers)
	// ═══════════════════════════════════════════════════════════════════════
	
	UFUNCTION(BlueprintPure, Category = "Framework")
	APlayerState* GetLocalPlayerState() const
	{
		if (IsLocallyControlled())
		{
			return GetController()->PlayerState.Get();
		}
		return nullptr;
	}
	void Handle_ArchetypeChanged(const FOnAttributeChangeData& ChangeData);
	
protected:
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Replication)
	// ═══════════════════════════════════════════════════════════════════════
	
	virtual void OnRep_PlayerState() override;
	UFUNCTION()
	void OnRep_IncarnatedArchetype();

public:
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IncarnatedArchetype, Category = "Gameplay|Ability")
	EClassArchetype IncarnatedArchetype;
};
