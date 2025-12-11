// EnemyCharacter.cpp
#include "AI/Enemy/EnemyCharacter.h"
#include "AI/Enemy/EnemyAIController.h"
#include "AI/Enemy/EnemyAbilityData.h"
#include "Gameplay/AbilitySystem/DarkAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Utils/Helpers/ContentHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"

// ═══════════════════════════════════════════════════════════════
// CONSTRUCTION
// ═══════════════════════════════════════════════════════════════

AEnemyCharacter::AEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    // Create Ability System Component
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    // Create Attribute Set
    AttributeSet = CreateDefaultSubobject<UDarkAttributeSet>(TEXT("AttributeSet"));

    // Set AI controller class
    AIControllerClass = AEnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    // Default values
    ResourceType = EResourceType::None;
    bRegenInCombat = false;
    RegenDelay = 0.0f;
    RegenDelayTimer = 0.0f;
    bIsInCombat = false;

    bConserveResourceForEmergency = false;
    EmergencyResourceReserve = 0.0f;
    LowResourceThreshold = 0.3f;

    CurrentFloor = 1;
    CachedEnemyData = nullptr;
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        // Initialize from DataTable if RowName is set
        if (!EnemyRowName.IsNone())
        {
            InitializeFromDataTable(EnemyRowName, CurrentFloor);
        }
    }
    SpawnLocation = GetActorLocation();
}

void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (HasAuthority())
    {
        // Handle resource regeneration
        HandleResourceRegeneration(DeltaTime);
    }
}

void AEnemyCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    // Initialize Ability System (Server only)
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
        
        if (AEnemyAIController* Aic = Cast<AEnemyAIController>(NewController))
        {
            Aic->InitializePerception();
            Aic->SetupBlackboardValues();
            Aic->StartBehaviorTree();
        }
    }
}

void AEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AEnemyCharacter, ResourceType);
    DOREPLIFETIME(AEnemyCharacter, bRegenInCombat);
    DOREPLIFETIME(AEnemyCharacter, RegenDelay);
}

// ═══════════════════════════════════════════════════════════════
// GAMEPLAY ABILITY SYSTEM
// ═══════════════════════════════════════════════════════════════

UAbilitySystemComponent* AEnemyCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

// ═══════════════════════════════════════════════════════════════
// INITIALIZATION
// ═══════════════════════════════════════════════════════════════

void AEnemyCharacter::InitializeFromDataTable(const FName RowName, const int32 FloorLevel)
{
    if (!HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("[EnemyCharacter] InitializeFromDataTable appelé sur client, ignoré"));
        return;
    }

    // Get DataTable
    UDataTable* EnemyTable = GetTable(TablePaths::Enemies);
    if (!EnemyTable)
    {
        UE_LOG(LogTemp, Error, TEXT("[EnemyCharacter] DT_StandardEnemies introuvable!"));
        return;
    }

    // Find row
    const FStandardEnemyRow* EnemyData = EnemyTable->FindRow<FStandardEnemyRow>(RowName, TEXT("InitEnemy"));
    if (!EnemyData)
    {
        UE_LOG(LogTemp, Error, TEXT("[EnemyCharacter] Row '%s' introuvable!"), *RowName.ToString());
        return;
    }

    // Cache enemy data (for AI controller access)
    CachedEnemyData = EnemyData;

    UE_LOG(LogTemp, Log, TEXT("═══════════════════════════════════════════════════════"));
    UE_LOG(LogTemp, Log, TEXT("🎮 INITIALIZING ENEMY: %s (Floor %d)"), *EnemyData->DisplayName.ToString(), FloorLevel);
    UE_LOG(LogTemp, Log, TEXT("═══════════════════════════════════════════════════════"));

    // 1. Apply base stats
    ApplyBaseStats(EnemyData);

    // 2. Apply floor scaling
    ApplyFloorScaling(EnemyData, FloorLevel);

    // 3. Initialize resource system
    InitializeResource(EnemyData);

    // 4. Grant abilities
    GrantAbilities(EnemyData);

    // 5. Cache behavior tags
    BehaviorTags = EnemyData->BehaviorTags;

    // 6. Cache AI resource management settings
    bConserveResourceForEmergency = EnemyData->bConserveResourceForEmergency;
    EmergencyResourceReserve = EnemyData->EmergencyResourceReserve;
    LowResourceThreshold = EnemyData->LowResourceThreshold;

    // Debug print
    DebugPrintStats();
}

void AEnemyCharacter::ApplyBaseStats(const FStandardEnemyRow* EnemyData) const
{
    if (!AttributeSet || !EnemyData)
    {
        return;
    }

    // Set base stats
    AttributeSet->SetMaxHealth(EnemyData->BaseHealth);
    AttributeSet->SetHealth(EnemyData->BaseHealth);
    AttributeSet->SetDamage(EnemyData->BaseDamage);
    AttributeSet->SetArmor(EnemyData->BaseArmor);
    AttributeSet->SetMoveSpeed(EnemyData->BaseMoveSpeed);

    // Set resistances
    AttributeSet->SetIceResistance(EnemyData->BaseIceResistance);
    AttributeSet->SetFireResistance(EnemyData->BaseFireResistance);
    AttributeSet->SetLightningResistance(EnemyData->BaseLightningResistance);
    AttributeSet->SetHolyResistance(EnemyData->BaseHolyResistance);

    // Update movement component
    if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
    {
        MovementComp->MaxWalkSpeed = EnemyData->BaseMoveSpeed;
    }

    UE_LOG(LogTemp, Log, TEXT("  📊 Base Stats: HP=%.0f, DMG=%.0f, Speed=%.0f, Armor=%.0f"),
        EnemyData->BaseHealth, EnemyData->BaseDamage, EnemyData->BaseMoveSpeed, EnemyData->BaseArmor);
    UE_LOG(LogTemp, Log, TEXT("  🛡️ Resistances: Ice=%.0f, Fire=%.0f, Lightning=%.0f, Holy=%.0f"),
        EnemyData->BaseIceResistance, EnemyData->BaseFireResistance,
        EnemyData->BaseLightningResistance, EnemyData->BaseHolyResistance);
}

void AEnemyCharacter::ApplyFloorScaling(const FStandardEnemyRow* EnemyData, const int32 FloorLevel) const
{
    if (!AttributeSet || !EnemyData)
    {
        return;
    }

    // Health scaling
    if (EnemyData->HealthScalingCurve)
    {
        const float Multiplier = EnemyData->HealthScalingCurve->GetFloatValue(FloorLevel);
        const float ScaledHealth = EnemyData->BaseHealth * Multiplier;
        
        AttributeSet->SetMaxHealth(ScaledHealth);
        AttributeSet->SetHealth(ScaledHealth);

        UE_LOG(LogTemp, Log, TEXT("  ⬆️ HP Scaling: x%.2f → %.0f HP"), Multiplier, ScaledHealth);
    }

    // Damage scaling
    if (EnemyData->DamageScalingCurve)
    {
        const float Multiplier = EnemyData->DamageScalingCurve->GetFloatValue(FloorLevel);
        const float ScaledDamage = EnemyData->BaseDamage * Multiplier;
        
        AttributeSet->SetDamage(ScaledDamage);

        UE_LOG(LogTemp, Log, TEXT("  ⬆️ DMG Scaling: x%.2f → %.0f DMG"), Multiplier, ScaledDamage);
    }
}

void AEnemyCharacter::InitializeResource(const FStandardEnemyRow* EnemyData)
{
    if (!AttributeSet || !EnemyData)
    {
        return;
    }

    ResourceType = EnemyData->ResourceType;

    if (ResourceType == EResourceType::None)
    {
        return;
    }

    // Calculate max resource with scaling
    float MaxResourceValue = EnemyData->BaseResourceMax;

    if (EnemyData->ResourceScalingCurve)
    {
        const float Multiplier = EnemyData->ResourceScalingCurve->GetFloatValue(CurrentFloor);
        MaxResourceValue *= Multiplier;

        UE_LOG(LogTemp, Log, TEXT("  ⬆️ Resource Scaling: x%.2f → %.0f max"), Multiplier, MaxResourceValue);
    }

    // Set resource values
    AttributeSet->SetMaxResource(MaxResourceValue);
    AttributeSet->SetResource(MaxResourceValue * EnemyData->StartingResourcePercent);
    AttributeSet->SetResourceRegenRate(EnemyData->ResourceRegenRate);

    // Cache regen settings
    bRegenInCombat = EnemyData->bRegenInCombat;
    RegenDelay = EnemyData->RegenDelay;

    // Get resource type name
    FString ResourceName;
    switch (ResourceType)
    {
    case EResourceType::Mana: ResourceName = "Mana"; break;
    case EResourceType::Stamina: ResourceName = "Stamina"; break;
    case EResourceType::Rage: ResourceName = "Rage"; break;
    case EResourceType::Energy: ResourceName = "Energy"; break;
    case EResourceType::Focus: ResourceName = "Focus"; break;
    default: ResourceName = "Unknown"; break;
    }

    UE_LOG(LogTemp, Log, TEXT("  ⚡ Resource: %s (%.0f/%.0f, Regen: %.1f/s)"),
        *ResourceName,
        AttributeSet->GetResource(),
        AttributeSet->GetMaxResource(),
        AttributeSet->GetResourceRegenRate());
}

void AEnemyCharacter::GrantAbilities(const FStandardEnemyRow* EnemyData)
{
    if (!AbilitySystemComponent || !EnemyData)
    {
        return;
    }

    if (EnemyData->AbilityRows.Num() == 0)
    {
        return;
    }

    // Get Abilities DataTable
    UDataTable* AbilitiesTable = GetTable(TablePaths::EnemiesAbility);
    if (!AbilitiesTable)
    {
        UE_LOG(LogTemp, Error, TEXT("[EnemyCharacter] DT_Abilities introuvable!"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("  ⚔️ Granting Abilities:"));

    for (const FName& AbilityRowName : EnemyData->AbilityRows)
    {
        const FEnemyAbilityRow* AbilityData = AbilitiesTable->FindRow<FEnemyAbilityRow>(AbilityRowName, TEXT("GrantAbility"));
        if (!AbilityData)
        {
            UE_LOG(LogTemp, Warning, TEXT("    ⚠️ Ability row '%s' not found"), *AbilityRowName.ToString());
            continue;
        }

        if (!AbilityData->AbilityClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("    ❌ Ability '%s' sans AbilityClass"), *AbilityData->DisplayName.ToString());
            continue;
        }

        // Grant ability to ASC
        FGameplayAbilitySpec AbilitySpec(AbilityData->AbilityClass, 1, INDEX_NONE, this);
        AbilitySystemComponent->GiveAbility(AbilitySpec);

        UE_LOG(LogTemp, Log, TEXT("    ✅ %s (%.0f %s, CD: %.1fs)"),
            *AbilityData->DisplayName.ToString(),
            AbilityData->ResourceCost,
            *UEnum::GetValueAsString(AbilityData->ResourceType),
            AbilityData->Cooldown);
    }
}

// ═══════════════════════════════════════════════════════════════
// RESOURCE MANAGEMENT
// ═══════════════════════════════════════════════════════════════

void AEnemyCharacter::HandleResourceRegeneration(const float DeltaTime)
{
    if (ResourceType == EResourceType::None)
    {
        return;
    }

    if (AttributeSet->GetResourceRegenRate() <= 0.0f)
    {
        return;
    }

    // Rage doesn't regen naturally
    if (ResourceType == EResourceType::Rage)
    {
        return;
    }

    // Check combat regen setting
    if (!bRegenInCombat && bIsInCombat)
    {
        return;
    }

    // Handle regen delay
    if (RegenDelayTimer > 0.0f)
    {
        RegenDelayTimer -= DeltaTime;
        return;
    }

    // Regenerate resource
    const float CurrentResource = AttributeSet->GetResource();
    const float MaxResource = AttributeSet->GetMaxResource();

    if (CurrentResource < MaxResource)
    {
        float NewResource = CurrentResource + (AttributeSet->GetResourceRegenRate() * DeltaTime);
        NewResource = FMath::Clamp(NewResource, 0.0f, MaxResource);
        AttributeSet->SetResource(NewResource);
    }
}

bool AEnemyCharacter::ConsumeResource(const float Amount)
{
    if (!HasEnoughResource(Amount))
    {
        return false;
    }

    const float CurrentResource = AttributeSet->GetResource();
    const float NewResource = FMath::Max(0.0f, CurrentResource - Amount);
    AttributeSet->SetResource(NewResource);

    // Reset regen delay
    RegenDelayTimer = RegenDelay;

    return true;
}

void AEnemyCharacter::AddResource(const float Amount)
{
    const float CurrentResource = AttributeSet->GetResource();
    const float MaxResource = AttributeSet->GetMaxResource();
    const float NewResource = FMath::Clamp(CurrentResource + Amount, 0.0f, MaxResource);
    AttributeSet->SetResource(NewResource);
}

bool AEnemyCharacter::HasEnoughResource(const float Cost) const
{
    const float CurrentResource = AttributeSet->GetResource();

    // If conserving resource for emergency, check reserve
    if (bConserveResourceForEmergency)
    {
        const float AvailableResource = CurrentResource - EmergencyResourceReserve;
        return AvailableResource >= Cost;
    }

    return CurrentResource >= Cost;
}

float AEnemyCharacter::GetCurrentResource() const
{
    return AttributeSet ? AttributeSet->GetResource() : 0.0f;
}

float AEnemyCharacter::GetMaxResource() const
{
    return AttributeSet ? AttributeSet->GetMaxResource() : 0.0f;
}

float AEnemyCharacter::GetResourcePercent() const
{
    const float MaxResource = GetMaxResource();
    return MaxResource > 0.0f ? (GetCurrentResource() / MaxResource) : 0.0f;
}

// ═══════════════════════════════════════════════════════════════
// GETTERS (Stats via AttributeSet)
// ═══════════════════════════════════════════════════════════════

float AEnemyCharacter::GetHealth() const
{
    return AttributeSet ? AttributeSet->GetHealth() : 0.0f;
}

float AEnemyCharacter::GetMaxHealth() const
{
    return AttributeSet ? AttributeSet->GetMaxHealth() : 0.0f;
}

float AEnemyCharacter::GetDamage() const
{
    return AttributeSet ? AttributeSet->GetDamage() : 0.0f;
}

float AEnemyCharacter::GetArmor() const
{
    return AttributeSet ? AttributeSet->GetArmor() : 0.0f;
}

// ═══════════════════════════════════════════════════════════════
// DEBUG
// ═══════════════════════════════════════════════════════════════

void AEnemyCharacter::DebugPrintStats() const
{
    if (!AttributeSet)
    {
        return;
    }

    // Get resource type name
    FString ResourceName;
    switch (ResourceType)
    {
    case EResourceType::Mana: ResourceName = "Mana"; break;
    case EResourceType::Stamina: ResourceName = "Stamina"; break;
    case EResourceType::Rage: ResourceName = "Rage"; break;
    case EResourceType::Energy: ResourceName = "Energy"; break;
    case EResourceType::Focus: ResourceName = "Focus"; break;
    default: ResourceName = "None"; break;
    }

    UE_LOG(LogTemp, Log, TEXT("═══════════════════════════════════════════════════════"));
    UE_LOG(LogTemp, Log, TEXT("📋 FINAL STATS"));
    UE_LOG(LogTemp, Log, TEXT("═══════════════════════════════════════════════════════"));
    UE_LOG(LogTemp, Log, TEXT("  💚 Health: %.0f / %.0f"), AttributeSet->GetHealth(), AttributeSet->GetMaxHealth());
    UE_LOG(LogTemp, Log, TEXT("  ⚔️ Damage: %.0f"), AttributeSet->GetDamage());
    UE_LOG(LogTemp, Log, TEXT("  🛡️ Armor: %.0f"), AttributeSet->GetArmor());
    UE_LOG(LogTemp, Log, TEXT("  🧊 Resistances: Ice=%.0f, Fire=%.0f, Lightning=%.0f, Holy=%.0f"),
        AttributeSet->GetIceResistance(),
        AttributeSet->GetFireResistance(),
        AttributeSet->GetLightningResistance(),
        AttributeSet->GetHolyResistance());

    if (ResourceType != EResourceType::None)
    {
        UE_LOG(LogTemp, Log, TEXT("  ⚡ %s: %.0f / %.0f (Regen: %.1f/s)"),
            *ResourceName,
            AttributeSet->GetResource(),
            AttributeSet->GetMaxResource(),
            AttributeSet->GetResourceRegenRate());
    }

    UE_LOG(LogTemp, Log, TEXT("  🏷️ Behavior Tags: %d tags"), BehaviorTags.Num());
    UE_LOG(LogTemp, Log, TEXT("═══════════════════════════════════════════════════════"));
}