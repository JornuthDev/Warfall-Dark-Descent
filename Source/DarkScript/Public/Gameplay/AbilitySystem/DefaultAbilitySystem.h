#pragma once

#include "AbilitySystemComponent.h"
#include "AttributeTypes.h"
#include "DefaultAbilitySystem.generated.h"

#define UE_API DARKSCRIPT_API

struct FDynamicEffect;
struct FDynamicEffectCalculation;
struct FDynamicEffectBasic;
struct FDefaultAttributeRow;
enum class EGameplayTagResistances : uint8;
/**
 * 
 */
UCLASS(ClassGroup=AbilitySystem, hidecategories=(Object,LOD,Lighting,Transform,Sockets,TextureStreaming), editinlinenew, meta=(BlueprintSpawnableComponent))
class UDefaultAbilitySystem : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	// ═══════════════════════════════════════════════════════════════════════
	// CONSTRUCTEUR
	// ═══════════════════════════════════════════════════════════════════════
	
	UDefaultAbilitySystem();
	virtual void Init(const FName& RowName);
	bool UpdateDefaultAttributeSets(const FDefaultAttributeRow*& AttributeRow);
	void InitDefaultAttributeSets(const FDefaultAttributeRow& AttributeRow);
	
	virtual void InitHealthSet(const FDefaultAttributeRow& AttributeRow);
	virtual EResourceType InitResourceSet(const FDefaultAttributeRow& AttributeRow);
	virtual void InitCombatSet(const FDefaultAttributeRow& AttributeRow);
	virtual void InitResistancesSet(const FDefaultAttributeRow& AttributeRow);
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Internal)
	// ═══════════════════════════════════════════════════════════════════════
	
	FActiveGameplayEffectHandle ApplyDynamicGameplayEffect(FDynamicEffect& Params, AActor* Causer = nullptr, UAbilitySystemComponent* Target = nullptr);
		
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Blueprint API)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintCallable, Category="Effect", meta = (DisplayName = "Apply Property (Character)"))
	FActiveGameplayEffectHandle ApplyPropertyToCharacter(UPARAM(meta = (Categories = "Ability.Character.PropertyType.")) const FGameplayTag PropertyTag, const EGameplayModOp::Type Operation, const float Magnitude, 
		AActor* Causer = nullptr, UAbilitySystemComponent* Target = nullptr, const bool bHasDuration = false, const float Duration = 1.f);
	
	UFUNCTION(BlueprintCallable, Category="Effect", meta = (DisplayName = "Apply Property (Causer)"))
	FActiveGameplayEffectHandle ApplyPropertyToCauser(UPARAM(meta = (Categories = "Ability.Causer.PropertyType.")) const FGameplayTag PropertyTag, const EGameplayModOp::Type Operation, const float Magnitude,
		const bool bHasDuration = false, const float Duration = 1.f);
	
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════
	
	FDefaultAttributeRow ActualAttributeData;
	
	FActiveGameplayEffectHandle HealthPerPerkEffectHandle;
	FActiveGameplayEffectHandle StaminaPerPerkEffectHandle;
	FActiveGameplayEffectHandle ManaPerPerkEffectHandle;
	FActiveGameplayEffectHandle FocusPerPerkEffectHandle;
};

#undef UE_API