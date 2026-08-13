#ifndef GUARD_WILD_ENCOUNTER_OW_H
#define GUARD_WILD_ENCOUNTER_OW_H

#include "constants/wild_encounter.h"

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

struct FieldEffectInfoOWE
{
    s16 xOffset;
    s16 yOffset;
    u32 visual;
};

extern const struct FieldEffectInfoOWE gOverworldWildEncounterFieldEffectInfo[];

void UpdateOverworldWildEncounter(void);
bool32 IsOverworldWildEncounter(struct ObjectEvent *owe, enum TypeOWE oweType);
void SetInstantOWESpawnTimer(void);
void SetMinimumOWESpawnTimer(void);
void TryTriggerOverworldWildEncounter(struct ObjectEvent *obstacle, struct ObjectEvent *collider);
const u8 *GetOverworlWildEncounterScript(u32 objectEventId);
void OnOverworldWildEncounterSpawn(struct ObjectEvent *owe);
void OnOverworldWildEncounterDespawn(struct ObjectEvent *owe);
bool32 IsOWEDespawnExempt(struct ObjectEvent *owe);
bool32 DespawnOWEDueToNPCCollision(struct ObjectEvent *obstacle, struct ObjectEvent *activeObject);
void DespawnAllOverworldWildEncounters(enum TypeOWE oweType, u32 flags);
u32 TryAndDespawnOldestGeneratedOWE_ToFreeObject();
void DespawnOWEOnBattleStart(void);
void TryDespawnOWEsCrossingMapConnection(void);
void RestoreSavedOWEBehaviorState(struct ObjectEvent *owe, struct Sprite *sprite);
void SetSavedOWEMovementState(struct ObjectEvent *owe);
void ClearSavedOWEMovementState(struct ObjectEvent *owe);
bool32 CheckRestrictedOWEMovement(struct ObjectEvent *owe, enum Direction direction);
bool32 CanAwareOWESeePlayer(struct ObjectEvent *owe);
bool32 IsPlayerInsideOWEActiveDistance(struct ObjectEvent *owe);
bool32 IsOWENextToPlayer(struct ObjectEvent *owe);
enum Direction DirectionOfOWEToPlayerFromCollision(struct ObjectEvent *owe);
u32 GetApproachingOWEDistanceToPlayer(struct ObjectEvent *owe, bool32 *equalDistances);
u32 GetOWEWalkMovementActionInDirectionWithSpeed(enum Direction direction, enum SpeedOWE speed);
bool32 TryPlayAmbientCryOWE(void);
u32 GetNumberOfActiveOWEs(enum TypeOWE oweType);
const struct ObjectEventTemplate TryGetObjectEventTemplateForOWE(const struct ObjectEventTemplate *template);
struct SpritePalette GetOWESpawnDespawnAnimFldEffPalette(enum SpawnDespawnTypeOWE spawnAnim);
u32 RemoveOldestGeneratedOWE(void);
bool32 CanRemoveObjectForOWEMovement(struct ObjectEvent *objectEvent);

extern const u8 InteractWithOverworldWildEncounter[];

#endif // GUARD_WILD_ENCOUNTER_OW_H
