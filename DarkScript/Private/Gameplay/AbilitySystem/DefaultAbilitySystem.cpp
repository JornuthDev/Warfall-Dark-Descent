#include "Gameplay/AbilitySystem/DefaultAbilitySystem.h"

#include "GameFramework/PlayerState.h"
#include "Gameplay/AbilitySystem/AdvancedAttributeSet.h"
#include "Gameplay/AbilitySystem/DefaultAttributeSet.h"
#include "Gameplay/AbilitySystem/GlobalAbilitySystem.h"
#include "Gameplay/AbilitySystem/GlobalAttributeSet.h"
#include "Gameplay/AbilitySystem/Effects/GE_PerkToCriticals.h"
#include "Gameplay/AbilitySystem/Effects/GE_PerkToDamageMultipliers.h"
#include "Gameplay/AbilitySystem/Effects/GE_PerkToMaxAttributes.h"
#include "Gameplay/AbilitySystem/Effects/GE_PerkToRegenRates.h"
#include "Interfaces/SystemsInterface.h"
#include "Utils/Log.h"

// ═══════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════
UDefaultAbilitySystem::UDefaultAbilitySystem()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	GE_PerkToMaxAttributes = UGE_PerkToMaxAttributes::StaticClass();
	GE_PerkToRegenRates = UGE_PerkToRegenRates::StaticClass();
	GE_PerkToDamageMultipliers = UGE_PerkToDamageMultipliers::StaticClass();
	GE_PerkToCriticals = UGE_PerkToCriticals::StaticClass();
}

void UDefaultAbilitySystem::Init(AActor* InOwnerActor, AActor* InAvatarActor, const EClassArchetype Archetype,
	const FName RowName)
{
	if (GetOwner()->HasAuthority())
	{
		InitAbilityActorInfo(InOwnerActor, InAvatarActor);
		InitAdvanced();
		InitAttributes(Archetype, RowName);
		ApplyPermanentGameplayEffects();
		return;
	}
	InitAbilityActorInfo(InOwnerActor, InAvatarActor);
	Server_Init(InOwnerActor, InAvatarActor, Archetype, RowName);
}

void UDefaultAbilitySystem::InitAdvanced()
{
	if (bIsPlayerCharacter && GetOwnerActor()->GetClass()->ImplementsInterface(USystemsInterface::StaticClass()))
	{
		if (UGlobalAbilitySystem* GlobalSystem = ISystemsInterface::Execute_GetGlobalAbilitySystem(GetOwnerActor()))
		{
			if (UAdvancedAttributeSet* AdvancedSet = const_cast<UAdvancedAttributeSet*>(GetSet<UAdvancedAttributeSet>()))
			{
				AdvancedSet->CurrentLevel = GlobalSystem->GetNumericAttribute(UGlobalAttributeSet::GetLevelAttribute());
				GlobalSystem->GetGameplayAttributeValueChangeDelegate(UGlobalAttributeSet::GetLevelAttribute()).AddUObject(AdvancedSet, &UAdvancedAttributeSet::OnLevelUp);
			}
		}
	}
}

void UDefaultAbilitySystem::InitAttributes(const EClassArchetype Archetype, const FName RowName)
{
	if (GetOwner()->HasAuthority())
	{
		if (UDefaultAttributeSet* DefaultSet = const_cast<UDefaultAttributeSet*>(GetSet<UDefaultAttributeSet>()))
		{
			DefaultSet->Init(Archetype, RowName);
		}
		return;
	}
	Server_InitAttributes(Archetype, RowName);
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Overrides)
// ═══════════════════════════════════════════════════════════════════════

FActiveGameplayEffectHandle UDefaultAbilitySystem::ApplyGameplayEffectSpecToTarget(
	const FGameplayEffectSpec& GameplayEffect, UAbilitySystemComponent* Target, FPredictionKey PredictionKey)
{	
	FGameplayEffectSpec ModifiedSpec = GameplayEffect;
	
	
	if (const UDefaultAttributeSet* DefaultSet = const_cast<UDefaultAttributeSet*>(GetSet<UDefaultAttributeSet>()))
	{
		const float RandValue = FMath::FRand();	
		const bool bIsCritic = RandValue <= DefaultSet->GetCriticalChance();
		
		// Vérifier si le GE a une définition
		if (ModifiedSpec.Def)
		{
			// Collecter tous les DamageType tags
			TArray<FGameplayTag> DamageTypeTags;
			
			// Parcourir tous les modifiers de la DÉFINITION
			for (const FGameplayModifierInfo& ModifierInfo : ModifiedSpec.Def->Modifiers)
			{
				// 1. Vérifier si l'attribut est ApplyDamages
				if (ModifierInfo.Attribute == UDefaultAttributeSet::GetApplyDamagesAttribute())
				{
					// 2. Vérifier si le type de calcul est SetByCaller
					if (ModifierInfo.ModifierMagnitude.GetMagnitudeCalculationType() == 
						EGameplayEffectMagnitudeCalculation::SetByCaller)
					{
						// 3. Récupérer le Data Tag du SetByCaller
						FGameplayTag DataTag = ModifierInfo.ModifierMagnitude.GetSetByCallerFloat().DataTag;
						if (DataTag.IsValid())
						{
							DamageTypeTags.Add(DataTag);
						}
					}
				}
			}
			// Si on a plusieurs types de dégâts, on doit les appliquer séparément
			if (DamageTypeTags.Num() > 1)
			{
				FActiveGameplayEffectHandle LastHandle;
				
				// Pour chaque type de dégât, créer un Spec avec UN SEUL tag dans DynamicAssetTags
				for (const FGameplayTag& DamageTypeTag : DamageTypeTags)
				{
					FGameplayEffectSpec SingleDamageSpec = GameplayEffect;
					
					// Empowerer UNIQUEMENT ce type
					DefaultSet->EmpoweredMagnitude(SingleDamageSpec, DamageTypeTag, bIsCritic);
					
					// Mettre les autres SetByCaller à 0 (ne pas les supprimer sinon erreur)
					for (const FGameplayTag& OtherTag : DamageTypeTags)
					{
						if (OtherTag != DamageTypeTag)
						{
							SingleDamageSpec.SetSetByCallerMagnitude(OtherTag, 0.f);
						}
					}
					
					// Appliquer ce Spec (qui a UN SEUL tag dans DynamicAssetTags)
					LastHandle = Super::ApplyGameplayEffectSpecToTarget(SingleDamageSpec, Target, PredictionKey);
				}
				
				return LastHandle;
			}
			// Si un seul type, procédure normale
			if (DamageTypeTags.Num() == 1)
			{
				DefaultSet->EmpoweredMagnitude(ModifiedSpec, DamageTypeTags[0], bIsCritic);
			}
		}
	}
	return Super::ApplyGameplayEffectSpecToTarget(ModifiedSpec, Target, PredictionKey);
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Helper)
// ═══════════════════════════════════════════════════════════════════════

void UDefaultAbilitySystem::ApplyPermanentGameplayEffects()
{
	FGameplayEffectContextHandle EffectContext = MakeEffectContext();
	EffectContext.AddSourceObject(this);
	
	UE_LOG(LogTemp, Warning, TEXT("ApplyPermanentGameplayEffects"));
	
	auto ApplyPermanentEffect = [this, EffectContext](const TSubclassOf<UGameplayEffect> GameplayEffect) -> void
	{
		if (GameplayEffect)
		{
			const FGameplayEffectSpecHandle Spec = MakeOutgoingSpec(
				GameplayEffect, GetContextLevel(), EffectContext);
			if (Spec.IsValid())
			{
				ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
			}
		}
	};
	ApplyPermanentEffect(GE_PerkToMaxAttributes);
	ApplyPermanentEffect(GE_PerkToRegenRates);
	ApplyPermanentEffect(GE_PerkToDamageMultipliers);
	ApplyPermanentEffect(GE_PerkToCriticals);
	
	UE_LOG(DarkScript, Warning, TEXT("ApplyPermanentGameplayEffects:: MaxHealth %f"), GetNumericAttribute(UDefaultAttributeSet::GetMaxHealthAttribute()));
	SetNumericAttributeBase(
		UDefaultAttributeSet::GetHealthAttribute(), GetNumericAttribute(UDefaultAttributeSet::GetMaxHealthAttribute())
	);
	SetNumericAttributeBase(
		UDefaultAttributeSet::GetManaAttribute(), GetNumericAttribute(UDefaultAttributeSet::GetMaxManaAttribute())	
	);
	SetNumericAttributeBase(
	UDefaultAttributeSet::GetStaminaAttribute(), GetNumericAttribute(UDefaultAttributeSet::GetMaxStaminaAttribute())	
	);
	SetNumericAttributeBase(
	UDefaultAttributeSet::GetFocusAttribute(), GetNumericAttribute(UDefaultAttributeSet::GetMaxFocusAttribute())	
	);
}

UDefaultAbilitySystem* UDefaultAbilitySystem::GetSystem(UObject* WorldContextObject)
{
	if (!WorldContextObject)
		return nullptr;

	const AController* PC;
	APlayerState* PS;
	UDefaultAbilitySystem* DefaultSystem = nullptr;
	if (ACharacter* Character = Cast<ACharacter>(WorldContextObject))
	{
		if (Character->GetClass()->ImplementsInterface(USystemsInterface::StaticClass()))
		{
			DefaultSystem = ISystemsInterface::Execute_GetDefaultAbilitySystem(Character);
		}
		if (DefaultSystem)
		{
			return DefaultSystem;
		}
		PC = Character->GetController();
	}
	else
	{
		PC = Cast<AController>(WorldContextObject);
	}
	
	if (PC && PC->PlayerState)
	{
		PS = PC->PlayerState.Get();
	}
	else
	{
		PS = Cast<APlayerState>(WorldContextObject);
	}
	
	if (PS && PS->GetClass()->ImplementsInterface(USystemsInterface::StaticClass()))
	{
		DefaultSystem = ISystemsInterface::Execute_GetDefaultAbilitySystem(PS);
		if (DefaultSystem)
		{
			return DefaultSystem;
		}
	}
	return nullptr;
}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTIONS (Replication)
// ═══════════════════════════════════════════════════════════════════════

void UDefaultAbilitySystem::Server_Init_Implementation(AActor* InOwnerActor, AActor* InAvatarActor,
	const EClassArchetype Archetype, const FName RowName)
{
	Init(InOwnerActor, InAvatarActor, Archetype, RowName);
}

void UDefaultAbilitySystem::Server_InitAttributes_Implementation(const EClassArchetype Archetype, const FName RowName)
{
	InitAttributes(Archetype, RowName);
}

