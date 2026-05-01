#ifndef GUARD_SPECIES_H
#define GUARD_SPECIES_H

#include "constants/abilities.h"
#include "constants/cries.h"
#include "constants/egg_ids.h"
#include "constants/event_objects.h"
#include "constants/pokemon_animation.h"
#include "constants/wild_encounter.h"

struct EvolutionParam
{
    u16 condition;
    u16 arg1;
    u16 arg2;
    u16 arg3;
};

struct Evolution
{
    u16 method;
    u16 param;
    enum Species targetSpecies;
    const struct EvolutionParam *params;
};

struct SpeciesInfo /*0xC4*/
{
    u8 baseHP;
    u8 baseAttack;
    u8 baseDefense;
    u8 baseSpeed;
    u8 baseSpAttack;
    u8 baseSpDefense;
    enum Type types[2];
    u8 catchRate;
    u8 forceTeraType;
    u16 expYield; // expYield was changed from u8 to u16 for the new Exp System.
    u16 evYield_HP:2;
    u16 evYield_Attack:2;
    u16 evYield_Defense:2;
    u16 evYield_Speed:2;
    u16 evYield_SpAttack:2;
    u16 evYield_SpDefense:2;
    u16 padding2:4;
    enum Item itemCommon;
    enum Item itemRare;
    u8 genderRatio;
    u8 eggCycles;
    u8 friendship;
    u8 growthRate;
    u8 eggGroups[2];
    enum Ability abilities[NUM_ABILITY_SLOTS]; // 3 abilities, no longer u8 because we have over 255 abilities now.
    u8 safariZoneFleeRate;

    // Pokédex data
    u8 categoryName[13];
    u8 speciesName[POKEMON_NAME_LENGTH + 1];
    enum PokemonCry cryId:16;
    enum NationalDexOrder natDexNum:16;
    u16 height; //in decimeters
    u16 weight; //in hectograms
    u16 pokemonScale;
    u16 pokemonOffset;
    u16 trainerScale;
    u16 trainerOffset;
    const u8 *description;
    enum BodyColor bodyColor:7;
    // Graphical Data
    u8 noFlip:1;
    u8 frontAnimDelay;
    u8 frontAnimId;
    u8 backAnimId;
    const union AnimCmd *const *frontAnimFrames;
    const u32 *frontPic;
    const u32 *backPic;
    const u16 *palette;
    const u16 *shinyPalette;
    const u8 *iconSprite;
#if P_GENDER_DIFFERENCES
    const u32 *frontPicFemale;
    const u32 *backPicFemale;
    const u16 *paletteFemale;
    const u16 *shinyPaletteFemale;
    const u8 *iconSpriteFemale;
#endif //P_GENDER_DIFFERENCES
#if P_FOOTPRINTS
    const u8 *footprint;
#endif
    // All Pokémon pics are 64x64, but this data table defines where in this 64x64 frame the sprite's non-transparent pixels actually are.
    u8 frontPicSize; // The dimensions of this drawn pixel area.
    u8 frontPicYOffset; // The number of pixels between the drawn pixel area and the bottom edge.
    u8 backPicSize; // The dimensions of this drawn pixel area.
    u8 backPicYOffset; // The number of pixels between the drawn pixel area and the bottom edge.
#if P_GENDER_DIFFERENCES
    u8 frontPicSizeFemale; // The dimensions of this drawn pixel area.
    u8 backPicSizeFemale; // The dimensions of this drawn pixel area.
#endif //P_GENDER_DIFFERENCES
    u8 iconPalIndex:3;
#if P_GENDER_DIFFERENCES
    u8 iconPalIndexFemale:3;
#else
    u8 paddingF:3;
#endif //P_GENDER_DIFFERENCES
    u8 pokemonJumpType:2; // According to the clerk, the Pokémon allowed in Pokémon Jump are all <= 28 inches/71 cm, and do not only swim, burrow, or fly.
    u8 enemyMonElevation; // This determines how much higher above the usual position the enemy Pokémon is during battle. Species that float or fly have nonzero values.
    // Flags
    u32 isRestrictedLegendary:1;
    u32 isSubLegendary:1;
    u32 isMythical:1;
    u32 isUltraBeast:1;
    u32 isParadox:1;
    u32 isTotem:1;
    u32 isMegaEvolution:1;
    u32 isPrimalReversion:1;
    u32 isUltraBurst:1;
    u32 isGigantamax:1;
    u32 isTeraForm:1;
    u32 isAlolanForm:1;
    u32 isGalarianForm:1;
    u32 isHisuianForm:1;
    u32 isPaldeanForm:1;
    u32 cannotBeTraded:1;
    u32 perfectIVCount:3;   // This species will always generate with the specified amount of perfect IVs.
    u32 dexForceRequired:1; // This species will be taken into account for Pokédex ratings even if they have the "isMythical" flag set.
    u32 teachingType:1; // Not used in the ROM but used in compilation (check constants/teaching_types.h for explanations)
    u32 isFrontierBanned:1; // This species is not allowed to participate in Battle Frontier facilities.
    u32 isSkyBattleBanned:1;
    u32 isTelekinesisBanned:1;
    u32 padding4:8;
    // Shadow settings
    s8 enemyShadowXOffset; // This determines the X-offset for an enemy Pokémon's shadow during battle; negative values point left, positive values point right.
    s8 enemyShadowYOffset; // This determines the Y-offset for an enemy Pokémon's shadow during battle; negative values point up, positive values point down.
    u16 enemyShadowSize:3; // This determines the size of the shadow sprite used for an enemy Pokémon's front sprite during battle.
    u16 suppressEnemyShadow:1; // If set to true, then a shadow will not be drawn beneath an enemy Pokémon's front sprite during battle.
    enum EggIds eggId:12;
    // Move Data
    const struct LevelUpMove *levelUpLearnset;
    const u16 *teachableLearnset;
    const u16 *eggMoveLearnset;
    const struct Evolution *evolutions;
    const enum Species *formSpeciesIdTable;
    const struct FormChange *formChangeTable;
#if OW_POKEMON_OBJECT_EVENTS
    struct ObjectEventGraphicsInfo overworldData;
#if P_GENDER_DIFFERENCES
    struct ObjectEventGraphicsInfo overworldDataFemale;
#endif //P_GENDER_DIFFERENCES
#if OW_PKMN_OBJECTS_SHARE_PALETTES == FALSE
    const void* overworldPalette;
    const void* overworldShinyPalette;
#if P_GENDER_DIFFERENCES
    const void* overworldPaletteFemale;
    const void* overworldShinyPaletteFemale;
#endif //P_GENDER_DIFFERENCES
#endif //OW_PKMN_OBJECTS_SHARE_PALETTES
#endif //OW_POKEMON_OBJECT_EVENTS
    enum OverworldWildEncounterBehaviors overworldEncounterBehavior;
};

extern const struct SpeciesInfo gSpeciesInfo[];

bool32 IsSpeciesEnabled(enum Species species);
enum Species SanitizeSpeciesId(enum Species species);
u32 GetSpeciesBaseHP(enum Species species);
u32 GetSpeciesBaseAttack(enum Species species);
u32 GetSpeciesBaseDefense(enum Species species);
u32 GetSpeciesBaseSpAttack(enum Species species);
u32 GetSpeciesBaseSpDefense(enum Species species);
u32 GetSpeciesBaseSpeed(enum Species species);
u32 GetSpeciesBaseStat(enum Species species, u32 statIndex);
u32 GetSpeciesBaseStatTotal(enum Species species);
enum Type GetSpeciesType(enum Species species, u8 slot);
u32 GetSpeciesCatchRate(enum Species species);
u32 GetSpeciesForcedTeraType(enum Species species);
u32 GetSpeciesExpYield(enum Species species);
u32 GetSpeciesEVYieldHP(enum Species species);
u32 GetSpeciesEVYieldAttack(enum Species species);
u32 GetSpeciesEVYieldDefense(enum Species species);
u32 GetSpeciesEVYieldSpAttack(enum Species species);
u32 GetSpeciesEVYieldSpDefense(enum Species species);
u32 GetSpeciesEVYieldSpeed(enum Species species);
u32 GetSpeciesEVYield(enum Species species, u32 statIndex);
enum Item GetSpeciesCommonItem(enum Species species);
enum Item GetSpeciesRareItem(enum Species species);
u32 GetSpeciesGenderRatio(enum Species species);
u32 GetSpeciesEggCycles(enum Species species);
u32 GetSpeciesBaseFriendship(enum Species species);
enum GrowthRate GetSpeciesGrowthRate(enum Species species);
u32 GetSpeciesEggGroup(enum Species species, u8 slot);
enum Ability GetSpeciesAbility(enum Species species, u8 slot);
const u8 *GetSpeciesCategory(enum Species species);
const u8 *GetSpeciesName(enum Species species);
enum PokemonCry GetSpeciesCryId(enum Species species);
enum NationalDexOrder SpeciesToNationalPokedexNum(enum Species species);
u32 GetSpeciesHeight(enum Species species);
u32 GetSpeciesWeight(enum Species species);
u32 GetSpeciesPokedexScale(enum Species species);
u32 GetSpeciesPokedexOffset(enum Species species);
u32 GetSpeciesPokedexTrainerScale(enum Species species);
u32 GetSpeciesPokedexTrainerOffset(enum Species species);
const u8 *GetSpeciesPokedexDescription(enum Species species);
enum BodyColor GetSpeciesBodyColor(enum Species species);
const u32 *GetSpeciesFrontPic(enum Species species);
const u32 *GetSpeciesFrontPicFemale(enum Species species);
u32 GetSpeciesFrontPicSize(enum Species species);
u32 GetSpeciesFrontPicSizeFemale(enum Species species);
u32 GetSpeciesFrontPicYOffset(enum Species species);
u32 GetSpeciesFrontAnimId(enum Species species);
u32 GetSpeciesFrontAnimDelay(enum Species species);
const union AnimCmd *const *GetSpeciesFrontAnimFrames(enum Species species);
u32 GetSpeciesEnemyElevation(enum Species species);
u32 GetSpeciesEnemyShadowXOffset(enum Species species);
u32 GetSpeciesEnemyShadowYOffset(enum Species species);
u32 GetSpeciesEnemyShadowSize(enum Species species);
u32 IsSpeciesEnemyShadowSuppressed(enum Species species);
bool32 IsMonSpriteNotFlipped(enum Species species);
const u32 *GetSpeciesBackPic(enum Species species);
const u32 *GetSpeciesBackPicFemale(enum Species species);
u32 GetSpeciesBackPicSize(enum Species species);
u32 GetSpeciesBackPicSizeFemale(enum Species species);
u32 GetSpeciesBackPicYOffset(enum Species species);
enum BackAnim GetSpeciesBackAnimSet(enum Species species);
const u16 *GetSpeciesPalette(enum Species species);
const u16 *GetSpeciesPaletteFemale(enum Species species);
const u16 *GetSpeciesShinyPalette(enum Species species);
const u16 *GetSpeciesShinyPaletteFemale(enum Species species);
const u8 *GetSpeciesIconSprite(enum Species species);
const u8 *GetSpeciesIconSpriteFemale(enum Species species);
u32 GetSpeciesIconPalIndex(enum Species species);
u32 GetSpeciesIconPalIndexFemale(enum Species species);
bool32 IsSpeciesRestrictedLegendary(enum Species species);
bool32 IsSpeciesSubLegendary(enum Species species);
bool32 IsSpeciesMythical(enum Species species);
bool32 IsSpeciesUltraBeast(enum Species species);
bool32 IsSpeciesParadox(enum Species species);
bool32 IsSpeciesTotem(enum Species species);
bool32 IsSpeciesMegaEvolution(enum Species species);
bool32 IsSpeciesPrimalReversion(enum Species species);
bool32 IsSpeciesUltraBurst(enum Species species);
bool32 IsSpeciesGigantamax(enum Species species);
bool32 IsSpeciesTeraForm(enum Species species);
bool32 IsSpeciesAlolanForm(enum Species species);
bool32 IsSpeciesGalarianForm(enum Species species);
bool32 IsSpeciesHisuianForm(enum Species species);
bool32 IsSpeciesPaldeanForm(enum Species species);
bool32 IsSpeciesRegionalForm(u32 species);
bool32 IsSpeciesTradeBanned(enum Species species);
u32 GetSpeciesPerfectIVCount(enum Species species);
bool32 IsSpeciesDexForced(enum Species species);
bool32 IsSpeciesFrontierBanned(enum Species species);
bool32 IsSpeciesSkyBattleBanned(enum Species species);
bool32 IsSpeciesTelekinesisBanned(enum Species species);
enum EggIds GetSpeciesEggId(enum Species species);
u32 GetSpeciesPokemonJumpType(enum Species species);
bool32 IsSpeciesAllowedInPokemonJump(enum Species species);
const struct LevelUpMove *GetSpeciesLevelUpLearnset(enum Species species);
const u16 *GetSpeciesTeachableLearnset(enum Species species);
const u16 *GetSpeciesEggMoves(enum Species species);
const struct Evolution *GetSpeciesEvolutions(enum Species species);
const enum Species *GetSpeciesFormTable(enum Species species);
enum Species GetFormSpeciesId(enum Species species, u8 formId);
enum Species GetBaseSpeciesId(enum Species species);
const struct FormChange *GetSpeciesFormChanges(enum Species species);
bool32 SpeciesHasGenderDifferences(enum Species species);
#if P_FOOTPRINTS
const u8 *GetSpeciesFootprint(enum Species species);
#endif // P_FOOTPRINTS
#if OW_POKEMON_OBJECT_EVENTS
const struct ObjectEventGraphicsInfo *GetSpeciesOverworldData(enum Species species);
const struct ObjectEventGraphicsInfo *GetSpeciesOverworldDataFemale(enum Species species);
const void *GetSpeciesOverworldPalette(enum Species species);
const u16 *GetSpeciesOverworldPaletteFemale(enum Species species);
const u16 *GetSpeciesOverworldShinyPalette(enum Species species);
const u16 *GetSpeciesOverworldShinyPaletteFemale(enum Species species);
bool32 SpeciesHasDistinctOverworldFemalePalette(enum Species species);
#endif // OW_POKEMON_OBJECT_EVENTS

u32 GetSpeciesOWEMovementType(enum Species species);
u32 GetSpeciesOWEViewDistance(enum Species species);
u32 GetSpeciesOWEViewWidth(enum Species species);
u32 GetSpeciesOWEViewActiveDistance(enum Species species);
enum SpeedOWE GetSpeciesOWEIdleSpeed(enum Species species);
enum SpeedOWE GetSpeciesOWEActiveSpeed(enum Species species);
enum ReturnToIdleOWE GetSpeciesOWEReturnToIdle(enum Species species);

#define gSpeciesInfo \
_Pragma("GCC error \"Use getters instead of accessing gSpeciesInfo directly.\"") \
gSpeciesInfo

#endif // GUARD_SPECIES_H
