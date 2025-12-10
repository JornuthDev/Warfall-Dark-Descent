// DDSGameplayTags.cpp
#include "Utils/Libraries/DarkTags.h"

namespace DarkTags
{
    // ═══════════════════════════════════════════════════════════════
    // DÉFINITIONS DES TAGS
    // ═══════════════════════════════════════════════════════════════

    // ─── COMBAT ───
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Combat_Aggressive, "AI.Behavior.Combat.Aggressive", "IA attaque de manière agressive");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Combat_Defensive, "AI.Behavior.Combat.Defensive", "IA joue la défense");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Combat_Balanced, "AI.Behavior.Combat.Balanced", "IA équilibrée");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Combat_Kamikaze, "AI.Behavior.Combat.Kamikaze", "IA suicide rush");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Combat_Cautious, "AI.Behavior.Combat.Cautious", "IA attend les opportunités");

    // ─── ROLE ───
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Role_Tank, "AI.Behavior.Role.Tank", "IA tanke les dégâts");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Role_DPS, "AI.Behavior.Role.DPS", "IA focus les dégâts");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Role_Healer, "AI.Behavior.Role.Healer", "IA soigne les alliés");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Role_Support, "AI.Behavior.Role.Support", "IA buff/debuff");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Role_Summoner, "AI.Behavior.Role.Summoner", "IA invoque des adds");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Role_Controller, "AI.Behavior.Role.Controller", "IA contrôle avec CC");

    // ─── MOVEMENT ───
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Movement_Melee, "AI.Behavior.Movement.Melee", "IA reste au corps à corps");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Movement_Ranged, "AI.Behavior.Movement.Ranged", "IA maintient une distance");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Movement_Kiting, "AI.Behavior.Movement.Kiting", "IA fait du hit and run");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Movement_Stationary, "AI.Behavior.Movement.Stationary", "IA ne bouge pas");

    // ─── SELF PRESERVATION ───
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_SelfPreservation_Flee, "AI.Behavior.SelfPreservation.Flee", "IA fuit si low HP");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_SelfPreservation_LastStand, "AI.Behavior.SelfPreservation.LastStand", "IA enrage si low HP");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_SelfPreservation_SelfHeal, "AI.Behavior.SelfPreservation.SelfHeal", "IA se soigne");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_SelfPreservation_CallHelp, "AI.Behavior.SelfPreservation.CallHelp", "IA appelle des renforts");

    // ─── SOCIAL ───
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Social_PackFighter, "AI.Behavior.Social.PackFighter", "IA combat en groupe");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Social_Lone, "AI.Behavior.Social.Lone", "IA combat seule");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Social_AlertOthers, "AI.Behavior.Social.AlertOthers", "IA alerte les alliés");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Social_ProtectAllies, "AI.Behavior.Social.ProtectAllies", "IA défend les alliés");

    // ─── PATROL ───
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Patrol_Static, "AI.Behavior.Patrol.Static", "IA ne bouge pas");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Patrol_Walk, "AI.Behavior.Patrol.Walk", "IA marche lentement");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Patrol_Guard, "AI.Behavior.Patrol.Guard", "IA surveille une zone");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Patrol_Interactive, "AI.Behavior.Patrol.Interactive", "IA interagit avec environnement");

    // ─── INTERACTION ───
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Interaction_ReadBook, "AI.Behavior.Interaction.ReadBook", "IA lit un livre");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Interaction_SitBench, "AI.Behavior.Interaction.SitBench", "IA s'assoit sur un banc");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Interaction_InspectObject, "AI.Behavior.Interaction.InspectObject", "IA inspecte un objet");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Interaction_Converse, "AI.Behavior.Interaction.Converse", "IA discute avec alliés");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Interaction_Craft, "AI.Behavior.Interaction.Craft", "IA utilise une table de craft");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Interaction_Eat, "AI.Behavior.Interaction.Eat", "IA mange");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Interaction_Sleep, "AI.Behavior.Interaction.Sleep", "IA dort");

    // ─── PERCEPTION ───
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Perception_HighVision, "AI.Behavior.Perception.HighVision", "IA voit loin");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Perception_LowVision, "AI.Behavior.Perception.LowVision", "IA voit mal");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Perception_HighHearing, "AI.Behavior.Perception.HighHearing", "IA entend bien");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Perception_LowHearing, "AI.Behavior.Perception.LowHearing", "IA entend mal");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Perception_Blind, "AI.Behavior.Perception.Blind", "IA aveugle, basée sur le son");

    // ─── SPECIAL ───
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Special_Enrage, "AI.Behavior.Special.Enrage", "IA enrage à % HP");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Special_Teleport, "AI.Behavior.Special.Teleport", "IA peut téléporter");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Special_Regenerate, "AI.Behavior.Special.Regenerate", "IA régénère HP");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Special_Phased, "AI.Behavior.Special.Phased", "IA combat par phases");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behavior_Special_Invulnerable, "AI.Behavior.Special.Invulnerable", "IA invulnérable par défaut");
}