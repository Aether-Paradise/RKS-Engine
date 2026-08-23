#ifndef GUARD_CONSTANTS_WILD_ENCOUNTER_OW_H
#define GUARD_CONSTANTS_WILD_ENCOUNTER_OW_H

enum SpawnDespawnTypeOWE
{
    OWE_SPAWN_ANIM_GRASS,
    OWE_SPAWN_ANIM_LONG_GRASS,
    OWE_SPAWN_ANIM_WATER,
    OWE_SPAWN_ANIM_UNDERWATER,
    OWE_SPAWN_ANIM_CAVE,
    OWE_SPAWN_ANIM_SHINY
};

enum TypeOWE
{
    OWE_ANY,
    OWE_GENERATED,
    OWE_MANUAL,
    OWE_NONE
};

// OWE_SPEED_FASTER seems to visually bug out sometimes.
enum SpeedOWE
{
    OWE_SPEED_NORMAL,
    OWE_SPEED_SLOW,
    OWE_SPEED_FAST,
    OWE_SPEED_FASTER
};

enum ReturnToIdleOWE
{
    NEVER_RETURN,
    PLAYER_OUTSIDE_ACTIVE_RANGE,
    PLAYER_CANT_BE_SEEN
};

struct BehaviorOWE
{
    u32 movementType:8;
    u32 viewDistance:4;
    u32 viewWidth:4;
    u32 activeDistance:4;
    enum SpeedOWE idleSpeed:2;
    enum SpeedOWE activeSpeed:2;
    enum ReturnToIdleOWE returnToIdle:2;
    u32 padding:6;
};

enum __attribute__((packed)) OverworldWildEncounterBehaviors
{
    OWE_IGNORE_PLAYER,
    OWE_CHASE_PLAYER_SLOW,
    OWE_FLEE_PLAYER_NORMAL,
    OWE_WATCH_PLAYER_NORMAL,
    OWE_APPROACH_PLAYER_SLOW,
    OWE_DESPAWN_ON_NOTICE,
    OWE_SPECIES_BEHAVIOR_COUNT
};

struct FieldEffectInfoOWE
{
    s16 xOffset;
    s16 yOffset;
    u32 visual;
};

#endif // GUARD_CONSTANTS_WILD_ENCOUNTER_OW_H
