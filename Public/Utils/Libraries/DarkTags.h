// DDSGameplayTags.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

/**
 * Dark Tags
 * GameplayTags natifs pour le système d'IA
 * Utilise les macros UE5 pour une performance optimale
 */
namespace DarkTags
{
    // ═══════════════════════════════════════════════════════════════════════════════
    // ═══ TAGS DE COMPORTEMENT IA ═══════════════════════════════════════════════════
    // ═══════════════════════════════════════════════════════════════════════════════

    // ─── Combat ───
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Combat_Aggressive);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Combat_Defensive);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Combat_Balanced);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Combat_Kamikaze);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Combat_Cautious);

    // ─── Role ───
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Role_Tank);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Role_DPS);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Role_Healer);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Role_Support);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Role_Summoner);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Role_Controller);

    // ─── Movement ───
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Movement_Melee);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Movement_Ranged);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Movement_Kiting);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Movement_Stationary);

    // ─── Self Preservation ───
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_SelfPreservation_Flee);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_SelfPreservation_LastStand);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_SelfPreservation_SelfHeal);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_SelfPreservation_CallHelp);

    // ─── Social ───
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Social_PackFighter);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Social_Lone);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Social_AlertOthers);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Social_ProtectAllies);

    // ─── Patrol ───
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Patrol_Static);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Patrol_Walk);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Patrol_Guard);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Patrol_Interactive);

    // ─── Interaction ───
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Interaction_ReadBook);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Interaction_SitBench);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Interaction_InspectObject);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Interaction_Converse);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Interaction_Craft);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Interaction_Eat);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Interaction_Sleep);

    // ─── Perception ───
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Perception_HighVision);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Perception_LowVision);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Perception_HighHearing);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Perception_LowHearing);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Perception_Blind);

    // ─── Special ───
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Special_Enrage);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Special_Teleport);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Special_Regenerate);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Special_Phased);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Special_Invulnerable);
}

namespace SetByCallerName
{
    static const FName Health = FName("Health");
}