#include "AI/EnemyDataTester.h"

#include "Utils/Helpers/ContentHelpers.h"
#include "Utils/Libraries/DarkTags.h"

AEnemyDataTester::AEnemyDataTester()
{
    PrimaryActorTick.bCanEverTick = false;
    EnemyDataTable = GetTable(TablePaths::Enemies);
}

void AEnemyDataTester::BeginPlay()
{
    Super::BeginPlay();

    if (EnemyDataTable)
    {
        UE_LOG(LogTemp, Log, TEXT("═══════════════════════════════════════════════════════"));
        UE_LOG(LogTemp, Log, TEXT("🧪 TEST DATATABLE ENNEMIS"));
        UE_LOG(LogTemp, Log, TEXT("═══════════════════════════════════════════════════════"));

        // Test lecture basique
        TestReadEnemyData(FName("Goblin_Berserker"));
        TestReadEnemyData(FName("Mage_Scholar"));
        TestReadEnemyData(FName("Guard_Protector"));

        // Test scaling
        UE_LOG(LogTemp, Log, TEXT("\n🔢 TEST SCALING (Palier %d):"), CurrentFloor);
        TestScaledStats(FName("Goblin_Berserker"), CurrentFloor);
        TestScaledStats(FName("Mage_Scholar"), CurrentFloor);
        TestScaledStats(FName("Guard_Protector"), CurrentFloor);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ EnemyDataTable est NULL! Assignez DT_StandardEnemies dans l'éditeur."));
    }
}

void AEnemyDataTester::TestReadEnemyData(FName RowName)
{
    if (!EnemyDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ EnemyDataTable est NULL!"));
        return;
    }

    FStandardEnemyRow* EnemyData = EnemyDataTable->FindRow<FStandardEnemyRow>(RowName, TEXT(""));

    if (!EnemyData)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Row '%s' introuvable!"), *RowName.ToString());
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("\n┌─────────────────────────────────────────────────────┐"));
    UE_LOG(LogTemp, Log, TEXT("│ ✅ %s"), *EnemyData->DisplayName.ToString());
    UE_LOG(LogTemp, Log, TEXT("├─────────────────────────────────────────────────────┤"));
    
    // Attributs de base
    UE_LOG(LogTemp, Log, TEXT("│ 💚 HP: %.0f | ⚔️ Dégâts: %.0f | 🏃 Vitesse: %.0f"), 
        EnemyData->BaseHealth, EnemyData->BaseDamage, EnemyData->BaseSpeed);
    UE_LOG(LogTemp, Log, TEXT("│ 🛡️ Armure: %.0f"), EnemyData->BaseArmor);

    // Ressource
    if (EnemyData->ResourceType != EResourceType::None)
    {
        FString ResourceTypeName;
        switch (EnemyData->ResourceType)
        {
            case EResourceType::Mana: ResourceTypeName = "Mana"; break;
            case EResourceType::Stamina: ResourceTypeName = "Stamina"; break;
            case EResourceType::Rage: ResourceTypeName = "Rage"; break;
            case EResourceType::Energy: ResourceTypeName = "Energy"; break;
            case EResourceType::Focus: ResourceTypeName = "Focus"; break;
            default: ResourceTypeName = "Unknown"; break;
        }

        UE_LOG(LogTemp, Log, TEXT("│ ⚡ Ressource: %s (%.0f max, %.0f%% au départ)"), 
            *ResourceTypeName, EnemyData->BaseResourceMax, EnemyData->StartingResourcePercent * 100);
        UE_LOG(LogTemp, Log, TEXT("│    Regen: %.1f/sec | En Combat: %s | Délai: %.1fs"), 
            EnemyData->ResourceRegenRate, 
            EnemyData->bRegenInCombat ? TEXT("Oui") : TEXT("Non"),
            EnemyData->RegenDelay);
        UE_LOG(LogTemp, Log, TEXT("│    Seuil Bas: %.0f%% | Réserve Urgence: %.0f%%"),
            EnemyData->LowResourceThreshold * 100,
            EnemyData->bConserveResourceForEmergency ? EnemyData->EmergencyResourceReserve * 100 : 0.0f);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("│ ⚡ Ressource: Aucune"));
    }

    // Tags de comportement
    UE_LOG(LogTemp, Log, TEXT("│"));
    UE_LOG(LogTemp, Log, TEXT("│ 🏷️ Tags de Comportement:"));

    if (EnemyData->BehaviorTags.HasTag(DarkTags::AI_Behavior_Combat_Aggressive))
        UE_LOG(LogTemp, Log, TEXT("│    🔥 Combat: AGRESSIF"));
    if (EnemyData->BehaviorTags.HasTag(DarkTags::AI_Behavior_Combat_Defensive))
        UE_LOG(LogTemp, Log, TEXT("│    🛡️ Combat: DÉFENSIF"));
    if (EnemyData->BehaviorTags.HasTag(DarkTags::AI_Behavior_Combat_Cautious))
        UE_LOG(LogTemp, Log, TEXT("│    🤔 Combat: PRUDENT"));

    if (EnemyData->BehaviorTags.HasTag(DarkTags::AI_Behavior_Role_Tank))
        UE_LOG(LogTemp, Log, TEXT("│    🛡️ Rôle: TANK"));
    if (EnemyData->BehaviorTags.HasTag(DarkTags::AI_Behavior_Role_DPS))
        UE_LOG(LogTemp, Log, TEXT("│    ⚔️ Rôle: DPS"));
    if (EnemyData->BehaviorTags.HasTag(DarkTags::AI_Behavior_Role_Support))
        UE_LOG(LogTemp, Log, TEXT("│    ✨ Rôle: SUPPORT"));

    if (EnemyData->BehaviorTags.HasTag(DarkTags::AI_Behavior_SelfPreservation_Flee))
        UE_LOG(LogTemp, Log, TEXT("│    🏃 Fuit à %.0f%% HP"), EnemyData->FleeHealthThreshold * 100);

    if (EnemyData->BehaviorTags.HasTag(DarkTags::AI_Behavior_Patrol_Interactive))
        UE_LOG(LogTemp, Log, TEXT("│    🎭 Patrouille: INTERACTIVE"));

    UE_LOG(LogTemp, Log, TEXT("└─────────────────────────────────────────────────────┘"));
}

void AEnemyDataTester::TestScaledStats(FName RowName, int32 FloorLevel)
{
    if (!EnemyDataTable)
        return;

    FStandardEnemyRow* EnemyData = EnemyDataTable->FindRow<FStandardEnemyRow>(RowName, TEXT(""));
    if (!EnemyData)
        return;

    // Calculer les stats scalées
    float HealthMultiplier = EnemyData->HealthScalingCurve ? EnemyData->HealthScalingCurve->GetFloatValue(FloorLevel) : 1.0f;
    float DamageMultiplier = EnemyData->DamageScalingCurve ? EnemyData->DamageScalingCurve->GetFloatValue(FloorLevel) : 1.0f;
    float ResourceMultiplier = EnemyData->ResourceScalingCurve ? EnemyData->ResourceScalingCurve->GetFloatValue(FloorLevel) : 1.0f;

    float ScaledHealth = EnemyData->BaseHealth * HealthMultiplier;
    float ScaledDamage = EnemyData->BaseDamage * DamageMultiplier;
    float ScaledResource = EnemyData->BaseResourceMax * ResourceMultiplier;

    UE_LOG(LogTemp, Log, TEXT("\n📊 %s - Palier %d:"), *EnemyData->DisplayName.ToString(), FloorLevel);
    UE_LOG(LogTemp, Log, TEXT("   HP: %.0f (x%.2f)"), ScaledHealth, HealthMultiplier);
    UE_LOG(LogTemp, Log, TEXT("   Dégâts: %.0f (x%.2f)"), ScaledDamage, DamageMultiplier);
    
    if (EnemyData->ResourceType != EResourceType::None)
    {
        UE_LOG(LogTemp, Log, TEXT("   Ressource: %.0f (x%.2f)"), ScaledResource, ResourceMultiplier);
    }
}