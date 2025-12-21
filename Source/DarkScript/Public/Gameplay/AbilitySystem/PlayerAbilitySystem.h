#pragma once
#include "CoreMinimal.h"
#include "DefaultAbilitySystem.h"
#include "PlayerAbilitySystem.generated.h"


enum class EResourceType : uint8;
struct FAdvancedAttributeRow;

UCLASS(ClassGroup=AbilitySystem, hidecategories=(Object,LOD,Lighting,Transform,Sockets,TextureStreaming), editinlinenew, meta=(BlueprintSpawnableComponent))
class DARKSCRIPT_API UPlayerAbilitySystem : public UDefaultAbilitySystem
{
	GENERATED_BODY()

public:
	// ═══════════════════════════════════════════════════════════════════════
	// CONSTRUCTEUR
	// ═══════════════════════════════════════════════════════════════════════
	
    UPlayerAbilitySystem();
	virtual void Init(const FName& RowName) override;
	bool UpdatePlayerAttributeSets(const FName& RowName, const bool bIsFirstInit = false);
	
	void InitPlayerSet(const FAdvancedAttributeRow& AttributeRow, const bool bInitLevel);
	virtual void InitHealthSet(const FDefaultAttributeRow& AttributeRow) override;
	virtual EResourceType InitResourceSet(const FDefaultAttributeRow& AttributeRow) override;
	virtual void InitCombatSet(const FDefaultAttributeRow& AttributeRow) override;
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Blueprint API)
	// ═══════════════════════════════════════════════════════════════════════
    
	UFUNCTION(BlueprintCallable, Category = "Player|Progression")
	void GiveExperience(const float Amount, UAbilitySystemComponent* Target = nullptr);
	
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════
};
