#ifndef GUARD_WILD_ENCOUNTER_OW_H
#define GUARD_WILD_ENCOUNTER_OW_H

#define OWE_SPAWNS_MAX                  4
#define OWE_APPROACH_DISTANCE           2
#define OWE_APPROACH_JUMP_TIMER_MIN     16
#define OWE_APPROACH_JUMP_TIMER_MAX     64
#define OWE_FLEE_COLLISION_TIME         6   // If a fleeing mon is unable to take a step for this many tries it will despawn. (Multiply this value by 16 to get number of frames.)
#define OWE_DESPAWN_FRAMES              30  // Number of frames before a mon despawns after noticing the player (OWE_BEHAVIOR_DESPAWN)

extern const struct FieldEffectInfoOWE gOverworldWildEncounterFieldEffectInfo[];

void UpdateOverworldWildEncounter(void);
bool32 IsOverworldWildEncounter(struct ObjectEvent *owe, enum TypeOWE oweType);
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
