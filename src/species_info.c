#include "global.h"
#include "species_info.h"
#include "test_runner.h"
#include "constants/abilities.h"
#include "constants/cries.h"
#include "constants/egg_ids.h"
#include "constants/event_objects.h"
#include "constants/pokemon_animation.h"
#include "constants/species.h"
#include "constants/wild_encounter.h"

#undef gSpeciesInfo

extern const struct BehaviorOWE gOWESpeciesBehavior[OWE_SPECIES_BEHAVIOR_COUNT];

bool32 IsSpeciesEnabled(enum Species species)
{
    // This function should not use the GetSpeciesBaseHP function, as the included sanitation will result in an infinite loop
    return gSpeciesInfo[species].baseHP > 0 || species == SPECIES_EGG;
}

enum Species SanitizeSpeciesId(enum Species species)
{
    assertf(species <= NUM_SPECIES && (species == SPECIES_NONE || IsSpeciesEnabled(species)), "invalid species: %d", species)
    {
        return SPECIES_NONE;
    }

    return species;
}

u32 GetSpeciesBaseHP(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].baseHP;
}

u32 GetSpeciesBaseAttack(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].baseAttack;
}

u32 GetSpeciesBaseDefense(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].baseDefense;
}

u32 GetSpeciesBaseSpAttack(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].baseSpAttack;
}

u32 GetSpeciesBaseSpDefense(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].baseSpDefense;
}

u32 GetSpeciesBaseSpeed(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].baseSpeed;
}

u32 GetSpeciesBaseStat(enum Species species, u32 statIndex)
{
    switch (statIndex)
    {
    case STAT_HP:
        return GetSpeciesBaseHP(species);
    case STAT_ATK:
        return GetSpeciesBaseAttack(species);
    case STAT_DEF:
        return GetSpeciesBaseDefense(species);
    case STAT_SPEED:
        return GetSpeciesBaseSpeed(species);
    case STAT_SPATK:
        return GetSpeciesBaseSpAttack(species);
    case STAT_SPDEF:
        return GetSpeciesBaseSpDefense(species);
    }
    return 0;
}

u32 GetSpeciesBaseStatTotal(enum Species species)
{
    return GetSpeciesBaseHP(species)
         + GetSpeciesBaseAttack(species)
         + GetSpeciesBaseDefense(species)
         + GetSpeciesBaseSpeed(species)
         + GetSpeciesBaseSpAttack(species)
         + GetSpeciesBaseSpDefense(species);
}

enum Type GetSpeciesType(enum Species species, u8 slot)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].types[slot];
}

u32 GetSpeciesCatchRate(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].catchRate;
}

u32 GetSpeciesForcedTeraType(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].forceTeraType;
}

u32 GetSpeciesExpYield(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].expYield;
}

u32 GetSpeciesEVYieldHP(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].evYield_HP;
}

u32 GetSpeciesEVYieldAttack(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].evYield_Attack;
}

u32 GetSpeciesEVYieldDefense(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].evYield_Defense;
}

u32 GetSpeciesEVYieldSpAttack(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].evYield_SpAttack;
}

u32 GetSpeciesEVYieldSpDefense(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].evYield_SpDefense;
}

u32 GetSpeciesEVYieldSpeed(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].evYield_Speed;
}

u32 GetSpeciesEVYield(enum Species species, u32 statIndex)
{
    switch (statIndex)
    {
    case STAT_HP:
        return GetSpeciesEVYieldHP(species);
    case STAT_ATK:
        return GetSpeciesEVYieldAttack(species);
    case STAT_DEF:
        return GetSpeciesEVYieldDefense(species);
    case STAT_SPEED:
        return GetSpeciesEVYieldSpeed(species);
    case STAT_SPATK:
        return GetSpeciesEVYieldSpAttack(species);
    case STAT_SPDEF:
        return GetSpeciesEVYieldSpDefense(species);
    }
    return 0;
}

enum Item GetSpeciesCommonItem(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].itemCommon;
}

enum Item GetSpeciesRareItem(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].itemRare;
}

u32 GetSpeciesGenderRatio(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].genderRatio;
}

u32 GetSpeciesEggCycles(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].eggCycles;
}

u32 GetSpeciesBaseFriendship(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].friendship;
}

enum GrowthRate GetSpeciesGrowthRate(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].growthRate;
}

u32 GetSpeciesEggGroup(enum Species species, u8 slot)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].eggGroups[slot];
}

enum Ability GetSpeciesAbility(enum Species species, u8 slot)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].abilities[slot];
}

const u8 *GetSpeciesCategory(enum Species species)
{
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].categoryName[0] == 0)
        return gSpeciesInfo[SPECIES_NONE].categoryName;
    return gSpeciesInfo[species].categoryName;
}

const u8 *GetSpeciesName(enum Species species)
{
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].speciesName[0] == 0)
        return gSpeciesInfo[SPECIES_NONE].speciesName;
    return gSpeciesInfo[species].speciesName;
}

enum PokemonCry GetSpeciesCryId(enum Species species)
{
    species = SanitizeSpeciesId(species);
    if (P_CRIES_ENABLED == FALSE || gSpeciesInfo[species].cryId >= CRY_COUNT || gTestRunnerHeadless)
        return CRY_NONE;
    return gSpeciesInfo[species].cryId;
}

enum NationalDexOrder SpeciesToNationalPokedexNum(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].natDexNum;
}

u32 GetSpeciesHeight(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].height;
}

u32 GetSpeciesWeight(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].weight;
}

u32 GetSpeciesPokedexScale(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].pokemonScale;
}

u32 GetSpeciesPokedexOffset(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].pokemonOffset;
}

u32 GetSpeciesPokedexTrainerScale(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].trainerScale;
}

u32 GetSpeciesPokedexTrainerOffset(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].trainerOffset;
}

const u8 *GetSpeciesPokedexDescription(enum Species species)
{
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].description == NULL)
        return gSpeciesInfo[SPECIES_NONE].description;
    return gSpeciesInfo[species].description;
}

enum BodyColor GetSpeciesBodyColor(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].bodyColor;
}

const u32 *GetSpeciesFrontPic(enum Species species)
{
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].frontPic == NULL)
        return gSpeciesInfo[SPECIES_NONE].frontPic;
    return gSpeciesInfo[species].frontPic;
}

const u32 *GetSpeciesFrontPicFemale(enum Species species)
{
#if P_GENDER_DIFFERENCES
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].frontPicFemale != NULL)
        return gSpeciesInfo[species].frontPicFemale;
#endif
    return GetSpeciesFrontPic(species);
}

u32 GetSpeciesFrontPicSize(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].frontPicSize;
}

u32 GetSpeciesFrontPicSizeFemale(enum Species species)
{
#if P_GENDER_DIFFERENCES
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].frontPicFemale != NULL)
        return gSpeciesInfo[species].frontPicSizeFemale;
#endif
    return GetSpeciesFrontPicSize(species);
}

u32 GetSpeciesFrontPicYOffset(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].frontPicYOffset;
}

u32 GetSpeciesFrontAnimId(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].frontAnimId;
}

u32 GetSpeciesFrontAnimDelay(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].frontAnimDelay;
}

const union AnimCmd *const *GetSpeciesFrontAnimFrames(enum Species species)
{
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].frontAnimFrames != NULL)
        return gSpeciesInfo[species].frontAnimFrames;
    return gSpeciesInfo[SPECIES_NONE].frontAnimFrames;
}

u32 GetSpeciesEnemyElevation(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].enemyMonElevation;
}

u32 GetSpeciesEnemyShadowXOffset(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].enemyShadowXOffset;
}

u32 GetSpeciesEnemyShadowYOffset(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].enemyShadowYOffset;
}

u32 GetSpeciesEnemyShadowSize(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].enemyShadowSize;
}

u32 IsSpeciesEnemyShadowSuppressed(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].suppressEnemyShadow;
}

bool32 IsMonSpriteNotFlipped(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].noFlip;
}

const u32 *GetSpeciesBackPic(enum Species species)
{
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].backPic == NULL)
        return gSpeciesInfo[SPECIES_NONE].backPic;
    return gSpeciesInfo[species].backPic;
}

const u32 *GetSpeciesBackPicFemale(enum Species species)
{
#if P_GENDER_DIFFERENCES
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].backPicFemale != NULL)
        return gSpeciesInfo[species].backPicFemale;
#endif
    return GetSpeciesBackPic(species);
}

u32 GetSpeciesBackPicSize(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].backPicSize;
}

u32 GetSpeciesBackPicSizeFemale(enum Species species)
{
#if P_GENDER_DIFFERENCES
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].backPicFemale != NULL)
        return gSpeciesInfo[species].backPicSizeFemale;
#endif
    return GetSpeciesBackPicSize(species);
}

u32 GetSpeciesBackPicYOffset(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].backPicYOffset;
}

enum BackAnim GetSpeciesBackAnimSet(enum Species species)
{
    if (gSpeciesInfo[species].backAnimId != BACK_ANIM_NONE)
        return gSpeciesInfo[species].backAnimId - 1;
    else
        return BACK_ANIM_NONE;
}

const u16 *GetSpeciesPalette(enum Species species)
{
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].palette == NULL)
        return gSpeciesInfo[SPECIES_NONE].palette;
    return gSpeciesInfo[species].palette;
}

const u16 *GetSpeciesPaletteFemale(enum Species species)
{
#if P_GENDER_DIFFERENCES
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].paletteFemale != NULL)
        return gSpeciesInfo[species].paletteFemale;
#endif
    return GetSpeciesPalette(species);
}

const u16 *GetSpeciesShinyPalette(enum Species species)
{
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].shinyPalette == NULL)
        return gSpeciesInfo[SPECIES_NONE].shinyPalette;
    return gSpeciesInfo[species].shinyPalette;
}

const u16 *GetSpeciesShinyPaletteFemale(enum Species species)
{
#if P_GENDER_DIFFERENCES
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].shinyPaletteFemale != NULL)
        return gSpeciesInfo[species].shinyPaletteFemale;
#endif
    return GetSpeciesShinyPalette(species);
}

const u8 *GetSpeciesIconSprite(enum Species species)
{
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].iconSprite == NULL)
        return gSpeciesInfo[SPECIES_NONE].iconSprite;
    return gSpeciesInfo[species].iconSprite;
}

const u8 *GetSpeciesIconSpriteFemale(enum Species species)
{
#if P_GENDER_DIFFERENCES
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].iconSpriteFemale != NULL)
        return gSpeciesInfo[species].iconSpriteFemale;
#endif
    return GetSpeciesIconSprite(species);
}

u32 GetSpeciesIconPalIndex(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].iconPalIndex;
}

u32 GetSpeciesIconPalIndexFemale(enum Species species)
{
#if P_GENDER_DIFFERENCES
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].iconSpriteFemale != NULL)
        return gSpeciesInfo[species].iconPalIndexFemale;
#endif
    return GetSpeciesIconPalIndex(species);
}

bool32 IsSpeciesRestrictedLegendary(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].isRestrictedLegendary;
}

bool32 IsSpeciesSubLegendary(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].isSubLegendary;
}

bool32 IsSpeciesMythical(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].isMythical;
}

bool32 IsSpeciesUltraBeast(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].isUltraBeast;
}

bool32 IsSpeciesParadox(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].isParadox;
}

bool32 IsSpeciesTotem(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].isTotem;
}

bool32 IsSpeciesMegaEvolution(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].isMegaEvolution;
}

bool32 IsSpeciesPrimalReversion(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].isPrimalReversion;
}

bool32 IsSpeciesUltraBurst(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].isUltraBurst;
}

bool32 IsSpeciesGigantamax(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].isGigantamax;
}

bool32 IsSpeciesTeraForm(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].isTeraForm;
}

bool32 IsSpeciesAlolanForm(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].isAlolanForm;
}

bool32 IsSpeciesGalarianForm(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].isGalarianForm;
}

bool32 IsSpeciesHisuianForm(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].isHisuianForm;
}

bool32 IsSpeciesPaldeanForm(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].isPaldeanForm;
}

bool32 IsSpeciesRegionalForm(u32 species)
{
    species = SanitizeSpeciesId(species);
    return gSpeciesInfo[species].isAlolanForm
        || gSpeciesInfo[species].isGalarianForm
        || gSpeciesInfo[species].isHisuianForm
        || gSpeciesInfo[species].isPaldeanForm;
}

bool32 IsSpeciesTradeBanned(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].cannotBeTraded;
}

u32 GetSpeciesPerfectIVCount(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].perfectIVCount;
}

bool32 IsSpeciesDexForced(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].dexForceRequired;
}

bool32 IsSpeciesFrontierBanned(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].isFrontierBanned;
}

bool32 IsSpeciesSkyBattleBanned(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].isSkyBattleBanned;
}

bool32 IsSpeciesTelekinesisBanned(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].isTelekinesisBanned;
}

enum EggIds GetSpeciesEggId(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].eggId;
}

u32 GetSpeciesPokemonJumpType(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].pokemonJumpType;
}

bool32 IsSpeciesAllowedInPokemonJump(enum Species species)
{
    return GetSpeciesPokemonJumpType(species) != PKMN_JUMP_TYPE_NONE;
}

const struct LevelUpMove *GetSpeciesLevelUpLearnset(enum Species species)
{
    const struct LevelUpMove *learnset = gSpeciesInfo[SanitizeSpeciesId(species)].levelUpLearnset;
    if (learnset == NULL)
        return gSpeciesInfo[SPECIES_NONE].levelUpLearnset;
    return learnset;
}

const u16 *GetSpeciesTeachableLearnset(enum Species species)
{
    const u16 *learnset = gSpeciesInfo[SanitizeSpeciesId(species)].teachableLearnset;
    if (learnset == NULL)
        return gSpeciesInfo[SPECIES_NONE].teachableLearnset;
    return learnset;
}

const u16 *GetSpeciesEggMoves(enum Species species)
{
    const u16 *learnset = gSpeciesInfo[SanitizeSpeciesId(species)].eggMoveLearnset;
    if (learnset == NULL)
        return gSpeciesInfo[SPECIES_NONE].eggMoveLearnset;
    return learnset;
}

const struct Evolution *GetSpeciesEvolutions(enum Species species)
{
    const struct Evolution *evolutions = gSpeciesInfo[SanitizeSpeciesId(species)].evolutions;
    if (evolutions == NULL)
        return gSpeciesInfo[SPECIES_NONE].evolutions;
    return evolutions;
}

const enum Species *GetSpeciesFormTable(enum Species species)
{
    const enum Species *formTable = gSpeciesInfo[SanitizeSpeciesId(species)].formSpeciesIdTable;
    if (formTable == NULL)
        return gSpeciesInfo[SPECIES_NONE].formSpeciesIdTable;
    return formTable;
}

enum Species GetFormSpeciesId(enum Species species, u8 formId)
{
    const enum Species *formTable = GetSpeciesFormTable(species);
    if (formTable != NULL)
        return formTable[formId];
    else
        return species;
}

enum Species GetBaseSpeciesId(enum Species species)
{
    return GetFormSpeciesId(species, 0);
}

const struct FormChange *GetSpeciesFormChanges(enum Species species)
{
    const struct FormChange *formChanges = gSpeciesInfo[SanitizeSpeciesId(species)].formChangeTable;
    if (formChanges == NULL)
        return gSpeciesInfo[SPECIES_NONE].formChangeTable;
    return formChanges;
}

bool32 SpeciesHasGenderDifferences(enum Species species)
{
#if P_GENDER_DIFFERENCES
    if (gSpeciesInfo[species].frontPicFemale != NULL
     || gSpeciesInfo[species].backPicFemale != NULL
     || gSpeciesInfo[species].paletteFemale != NULL
     || gSpeciesInfo[species].shinyPaletteFemale != NULL
     || gSpeciesInfo[species].iconSpriteFemale != NULL)
        return TRUE;
#endif

    return FALSE;
}

#if P_FOOTPRINTS
const u8 *GetSpeciesFootprint(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].footprint;
}
#endif // P_FOOTPRINTS

#if OW_POKEMON_OBJECT_EVENTS
const struct ObjectEventGraphicsInfo *GetSpeciesOverworldData(enum Species species)
{
    return &gSpeciesInfo[SanitizeSpeciesId(species)].overworldData;
}

const struct ObjectEventGraphicsInfo *GetSpeciesOverworldDataFemale(enum Species species)
{
#if P_GENDER_DIFFERENCES
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].overworldDataFemale.paletteTag == OBJ_EVENT_PAL_TAG_DYNAMIC)
        return &gSpeciesInfo[species].overworldDataFemale;
#endif
    return GetSpeciesOverworldData(species);
}

const void *GetSpeciesOverworldPalette(enum Species species)
{
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].overworldPalette == NULL)
        return gSpeciesInfo[SPECIES_NONE].overworldPalette;
    return gSpeciesInfo[species].overworldPalette;
}

const u16 *GetSpeciesOverworldPaletteFemale(enum Species species)
{
#if P_GENDER_DIFFERENCES
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].overworldPaletteFemale != NULL)
        return gSpeciesInfo[species].overworldPaletteFemale;
#endif
    return GetSpeciesOverworldPalette(species);
}

const u16 *GetSpeciesOverworldShinyPalette(enum Species species)
{
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].overworldShinyPalette == NULL)
        return gSpeciesInfo[SPECIES_NONE].overworldShinyPalette;
    return gSpeciesInfo[species].overworldShinyPalette;
}

const u16 *GetSpeciesOverworldShinyPaletteFemale(enum Species species)
{
#if P_GENDER_DIFFERENCES
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].overworldShinyPaletteFemale != NULL)
        return gSpeciesInfo[species].overworldShinyPaletteFemale;
#endif
    return GetSpeciesOverworldShinyPalette(species);
}

bool32 SpeciesHasDistinctOverworldFemalePalette(enum Species species)
{
#if P_GENDER_DIFFERENCES
    return gSpeciesInfo[SanitizeSpeciesId(species)].overworldPaletteFemale != NULL;
#else
    return FALSE;
#endif
}

#endif // OW_POKEMON_OBJECT_EVENTS

u32 OWE_GetMovementTypeFromSpecies(enum Species species)
{
    species = SanitizeSpeciesId(species);
    enum OverworldWildEncounterBehaviors behavior = gSpeciesInfo[species].overworldEncounterBehavior;
    return gOWESpeciesBehavior[behavior].movementType;
}

u32 OWE_GetViewDistanceFromSpecies(enum Species species)
{
    species = SanitizeSpeciesId(species);
    enum OverworldWildEncounterBehaviors behavior = gSpeciesInfo[species].overworldEncounterBehavior;
    return gOWESpeciesBehavior[behavior].viewDistance;
}

u32 OWE_GetViewWidthFromSpecies(enum Species species)
{
    species = SanitizeSpeciesId(species);
    enum OverworldWildEncounterBehaviors behavior = gSpeciesInfo[species].overworldEncounterBehavior;
    return gOWESpeciesBehavior[behavior].viewWidth;
}

u32 OWE_GetViewActiveDistanceFromSpecies(enum Species species)
{
    species = SanitizeSpeciesId(species);
    enum OverworldWildEncounterBehaviors behavior = gSpeciesInfo[species].overworldEncounterBehavior;
    return gOWESpeciesBehavior[behavior].activeDistance;
}

enum SpeedOWE OWE_GetIdleSpeedFromSpecies(enum Species species)
{
    species = SanitizeSpeciesId(species);
    enum OverworldWildEncounterBehaviors behavior = gSpeciesInfo[species].overworldEncounterBehavior;
    return gOWESpeciesBehavior[behavior].idleSpeed;
}

enum SpeedOWE OWE_GetActiveSpeedFromSpecies(enum Species species)
{
    species = SanitizeSpeciesId(species);
    enum OverworldWildEncounterBehaviors behavior = gSpeciesInfo[species].overworldEncounterBehavior;
    return gOWESpeciesBehavior[behavior].activeSpeed;
}

enum ReturnToIdleOWE OWE_GetReturnToIdleFromSpecies(enum Species species)
{
    species = SanitizeSpeciesId(species);
    enum OverworldWildEncounterBehaviors behavior = gSpeciesInfo[species].overworldEncounterBehavior;
    return gOWESpeciesBehavior[behavior].returnToIdle;
}

#define gSpeciesInfo \
_Pragma("GCC error \"Use getters instead of accessing gSpeciesInfo directly.\"") \
gSpeciesInfo
