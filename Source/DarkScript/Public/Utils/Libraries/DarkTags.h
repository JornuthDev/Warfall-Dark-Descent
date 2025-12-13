// DDSGameplayTags.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

/**
 * GameplayTags natifs pour le système d'IA
 * Utilise les macros UE5 pour une performance optimale
 */
namespace DarkTags
{
    // ═══════════════════════════════════════════════════════════════
    // TAGS DE COMPORTEMENT IA
    // ═══════════════════════════════════════════════════════════════

    // ─── COMBAT ───
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Combat_Aggressive);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Combat_Defensive);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Combat_Balanced);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Combat_Kamikaze);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Combat_Cautious);

    // ─── ROLE ───
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Role_Tank);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Role_DPS);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Role_Healer);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Role_Support);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Role_Summoner);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Role_Controller);

    // ─── MOVEMENT ───
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Movement_Melee);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Movement_Ranged);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Movement_Kiting);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Movement_Stationary);

    // ─── SELF PRESERVATION ───
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_SelfPreservation_Flee);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_SelfPreservation_LastStand);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_SelfPreservation_SelfHeal);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_SelfPreservation_CallHelp);

    // ─── SOCIAL ───
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Social_PackFighter);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Social_Lone);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Social_AlertOthers);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Social_ProtectAllies);

    // ─── PATROL ───
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Patrol_Static);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Patrol_Walk);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Patrol_Guard);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Patrol_Interactive);

    // ─── INTERACTION ───
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Interaction_ReadBook);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Interaction_SitBench);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Interaction_InspectObject);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Interaction_Converse);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Interaction_Craft);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Interaction_Eat);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Interaction_Sleep);

    // ─── PERCEPTION ───
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Perception_HighVision);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Perception_LowVision);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Perception_HighHearing);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Perception_LowHearing);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behavior_Perception_Blind);

    // ─── SPECIAL ───
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