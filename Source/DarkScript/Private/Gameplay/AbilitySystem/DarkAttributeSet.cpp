// DarkAttributeSet.cpp
#include "Gameplay/AbilitySystem/DarkAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UDarkAttributeSet::UDarkAttributeSet()
{
    // Default values
    InitHealth(100.0f);
    InitMaxHealth(100.0f);
    InitDamage(10.0f);
    InitArmor(0.0f);
    InitIceResistance(0.0f);
    InitFireResistance(0.0f);
    InitLightningResistance(0.0f);
    InitHolyResistance(0.0f);
    InitMoveSpeed(400.0f);
    InitResource(0.0f);
    InitMaxResource(0.0f);
    InitResourceRegenRate(0.0f);
}

void UDarkAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    // Clamp Health between 0 and MaxHealth
    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
    }

    // Clamp Resource between 0 and MaxResource
    if (Attribute == GetResourceAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxResource());
    }

    // Clamp Resistances between -100 and +100
    if (Attribute == GetIceResistanceAttribute() || 
        Attribute == GetFireResistanceAttribute() ||
        Attribute == GetLightningResistanceAttribute() ||
        Attribute == GetHolyResistanceAttribute())
    {
        NewValue = FMath::Clamp(NewValue, -100.0f, 100.0f);
    }
}

void UDarkAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    // ═══════════════════════════════════════════════════════════════
    // INCOMING DAMAGE (avec calcul selon EDamageType)
    // ═══════════════════════════════════════════════════════════════

    if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
    {
        const float RawDamage = GetIncomingDamage();
        SetIncomingDamage(0.0f); // Reset meta attribute

        if (RawDamage > 0.0f)
        {
            float FinalDamage;

            // Récupère le DamageType
            const EDamageType DamageType = PendingDamageType;

            // ═══════════════════════════════════════════════════════════
            // CALCUL SELON LE TYPE DE DÉGÂT
            // ═══════════════════════════════════════════════════════════

            switch (DamageType)
            {
                case EDamageType::Flesh:
                {
                    // Formule : Damage * (100 / (100 + Armor))
                    // Ex: 100 dmg, 50 armor → 100 * (100/150) = 66 dmg
                    const float EffectiveArmor = FMath::Max(0.0f, GetArmor());
                    FinalDamage = RawDamage * (100.0f / (100.0f + EffectiveArmor));
                    break;
                }

                case EDamageType::Penetration:
                {
                    // Ignore 70% de l'armor
                    constexpr float PenetrationPercent = 0.7f;
                    const float EffectiveArmor = FMath::Max(0.0f, GetArmor() * (1.0f - PenetrationPercent));
                    FinalDamage = RawDamage * (100.0f / (100.0f + EffectiveArmor));
                    break;
                }

                case EDamageType::Ice:
                {
                    // Resistance: -50 = prend 150% dégâts, +75 = prend 25% dégâts
                    const float Resistance = GetIceResistance();
                    const float Multiplier = 1.0f - (Resistance / 100.0f);
                    FinalDamage = RawDamage * FMath::Max(0.0f, Multiplier);
                    break;
                }

                case EDamageType::Fire:
                {
                    const float Resistance = GetFireResistance();
                    const float Multiplier = 1.0f - (Resistance / 100.0f);
                    FinalDamage = RawDamage * FMath::Max(0.0f, Multiplier);
                    break;
                }

                case EDamageType::Lightning:
                {
                    const float Resistance = GetLightningResistance();
                    const float Multiplier = 1.0f - (Resistance / 100.0f);
                    FinalDamage = RawDamage * FMath::Max(0.0f, Multiplier);
                    break;
                }

                case EDamageType::Holy:
                {
                    const float Resistance = GetHolyResistance();
                    const float Multiplier = 1.0f - (Resistance / 100.0f);
                    FinalDamage = RawDamage * FMath::Max(0.0f, Multiplier);
                    break;
                }

                case EDamageType::None:
                default:
                {
                    // True damage (ignore tout)
                    FinalDamage = RawDamage;
                    break;
                }
            }

            // Apply final damage to health
            const float NewHealth = GetHealth() - FinalDamage;
            SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));

            // Log pour debug
            UE_LOG(LogTemp, Log, TEXT("💥 Damage: %.0f (%s) → %.0f final"), 
                RawDamage, 
                *UEnum::GetValueAsString(DamageType),
                FinalDamage);

            // Reset damage type
            PendingDamageType = EDamageType::None;

            // Death check
            if (GetHealth() <= 0.0f)
            {
                UE_LOG(LogTemp, Warning, TEXT("☠️ Character died!"));
            }
        }
    }

    // ═══════════════════════════════════════════════════════════════
    // INCOMING HEALING
    // ═══════════════════════════════════════════════════════════════

    if (Data.EvaluatedData.Attribute == GetIncomingHealingAttribute())
    {
        const float LocalIncomingHealing = GetIncomingHealing();
        SetIncomingHealing(0.0f); // Reset meta attribute

        if (LocalIncomingHealing > 0.0f)
        {
            const float NewHealth = GetHealth() + LocalIncomingHealing;
            SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));
        }
    }

    // ═══════════════════════════════════════════════════════════════
    // MOVE SPEED UPDATE
    // ═══════════════════════════════════════════════════════════════

    if (Data.EvaluatedData.Attribute == GetMoveSpeedAttribute())
    {
        // Update CharacterMovementComponent speed
        if (AActor* Owner = GetOwningActor())
        {
            if (ACharacter* Character = Cast<ACharacter>(Owner))
            {
                if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
                {
                    MoveComp->MaxWalkSpeed = GetMoveSpeed();
                }
            }
        }
    }
}

void UDarkAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UDarkAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDarkAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDarkAttributeSet, Damage, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDarkAttributeSet, Armor, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDarkAttributeSet, IceResistance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDarkAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDarkAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDarkAttributeSet, HolyResistance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDarkAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDarkAttributeSet, Resource, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDarkAttributeSet, MaxResource, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDarkAttributeSet, ResourceRegenRate, COND_None, REPNOTIFY_Always);
}

// ═══════════════════════════════════════════════════════════════
// REPLICATION CALLBACKS
// ═══════════════════════════════════════════════════════════════

void UDarkAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDarkAttributeSet, Health, OldHealth);
}

void UDarkAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDarkAttributeSet, MaxHealth, OldMaxHealth);
}

void UDarkAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldDamage)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDarkAttributeSet, Damage, OldDamage);
}

void UDarkAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDarkAttributeSet, Armor, OldArmor);
}

void UDarkAttributeSet::OnRep_IceResistance(const FGameplayAttributeData& OldIceResistance)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDarkAttributeSet, IceResistance, OldIceResistance);
}

void UDarkAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDarkAttributeSet, FireResistance, OldFireResistance);
}

void UDarkAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDarkAttributeSet, LightningResistance, OldLightningResistance);
}

void UDarkAttributeSet::OnRep_HolyResistance(const FGameplayAttributeData& OldHolyResistance)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDarkAttributeSet, HolyResistance, OldHolyResistance);
}

void UDarkAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDarkAttributeSet, MoveSpeed, OldMoveSpeed);
}

void UDarkAttributeSet::OnRep_Resource(const FGameplayAttributeData& OldResource)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDarkAttributeSet, Resource, OldResource);
}

void UDarkAttributeSet::OnRep_MaxResource(const FGameplayAttributeData& OldMaxResource)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDarkAttributeSet, MaxResource, OldMaxResource);
}

void UDarkAttributeSet::OnRep_ResourceRegenRate(const FGameplayAttributeData& OldResourceRegenRate)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDarkAttributeSet, ResourceRegenRate, OldResourceRegenRate);
}

// ═══════════════════════════════════════════════════════════════
// HELPERS
// ═══════════════════════════════════════════════════════════════

void UDarkAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, const float MinValue, const float MaxValue) const
{
    if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
    {
        const float CurrentValue = ASC->GetNumericAttribute(Attribute);
        const float ClampedValue = FMath::Clamp(CurrentValue, MinValue, MaxValue);
        
        if (CurrentValue != ClampedValue)
        {
            ASC->SetNumericAttributeBase(Attribute, ClampedValue);
        }
    }
}
