#pragma once

#include "AbilitySystemComponent.h"
#include "AttributeTypes.h"
#include "DefaultAbilitySystem.generated.h"

#define UE_API DARKSCRIPT_API

struct FDynamicEffectConfig;
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
	
	// ═══════════════════════════════════════════════════════════════════════
	// DELEGATES
	// ═══════════════════════════════════════════════════════════════════════
	
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnEffectAppliedDelegate, FActiveGameplayEffectHandle, EffectHandle);
	
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
		
	/**
	 * Applique un effet dynamique configuré via une structure
	 * ✅ FONCTIONNE EN RÉPLICATION - L'effet est créé comme un vrai UObject
	 * 
	 * @param Config - Configuration de l'effet
	 * @param Causer - Acteur causant l'effet (optionnel)
	 * @param Target - ASC cible (optionnel, par défaut = self)
	 * @return Handle de l'effet appliqué
	 */
	UE_API FActiveGameplayEffectHandle ApplyDynamicEffect(
		const FDynamicEffectConfig& Config,
		AActor* Causer = nullptr,
		UAbilitySystemComponent* Target = nullptr);

	/**
	 * Version avec SetByCaller pour magnitude et durée dynamiques
	 */
	UE_API FActiveGameplayEffectHandle ApplyDynamicEffectWithValues(
		const FDynamicEffectConfig& Config,
		float MagnitudeValue,
		float DurationValue = 0.f,
		AActor* Causer = nullptr,
		UAbilitySystemComponent* Target = nullptr);

	/**
	 * Version simplifiée pour un seul attribut
	 */
	UE_API FActiveGameplayEffectHandle ApplySimpleEffect(
		const FGameplayAttribute& Attribute,
		float Magnitude,
		EGameplayModOp::Type Operation = EGameplayModOp::Additive,
		EGameplayEffectDurationType DurationType = EGameplayEffectDurationType::Instant,
		float Duration = 0.f,
		AActor* Causer = nullptr,
		UAbilitySystemComponent* Target = nullptr);

	// ═══════════════════════════════════════════════════════════════════════
	// BLUEPRINT API (backward compatibility)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintCallable, Category="Effect", meta = (DisplayName = "Apply Property (Character)"))
	UE_API FActiveGameplayEffectHandle ApplyPropertyToCharacter(
		UPARAM(meta = (Categories = "Ability.Character.PropertyType.")) const FGameplayTag PropertyTag, 
		const EGameplayModOp::Type Operation, 
		const float Magnitude, 
		AActor* Causer = nullptr, 
		UAbilitySystemComponent* Target = nullptr, 
		const bool bHasDuration = false, 
		const float Duration = 1.f);
	
	UFUNCTION(BlueprintCallable, Category="Effect", meta = (DisplayName = "Apply Property (Causer)"))
	UE_API FActiveGameplayEffectHandle ApplyPropertyToCauser(
		UPARAM(meta = (Categories = "Ability.Causer.PropertyType.")) const FGameplayTag PropertyTag, 
		const EGameplayModOp::Type Operation, 
		const float Magnitude,
		const bool bHasDuration = false, 
		const float Duration = 1.f);

	UE_API bool HasAuthority() const { return GetOwner()->HasAuthority(); }
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Replication)
	// ═══════════════════════════════════════════════════════════════════════
	
	UFUNCTION(Server, Reliable)
	UE_API void Server_ApplyDynamicEffectWithValues(
		const FDynamicEffectConfig& Config,
		float MagnitudeValue,
		float DurationValue = 0.f,
		AActor* Causer = nullptr,
		UAbilitySystemComponent* Target = nullptr);
	
	UFUNCTION(Server, Reliable)
	UE_API void Server_ApplyDynamicEffect(
		const FDynamicEffectConfig& Config,
		AActor* Causer = nullptr,
		UAbilitySystemComponent* Target = nullptr);
	
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════
	
	FDefaultAttributeRow ActualAttributeData;
	
public:
	FActiveGameplayEffectHandle HealthPerPerkEffectHandle;
	FActiveGameplayEffectHandle StaminaPerPerkEffectHandle;
	FActiveGameplayEffectHandle ManaPerPerkEffectHandle;
	FActiveGameplayEffectHandle FocusPerPerkEffectHandle;
};

#undef UE_API