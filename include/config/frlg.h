#ifndef GUARD_CONFIG_FRLG_H
#define GUARD_CONFIG_FRLG_H

#define FIRE_RED   1
#define LEAF_GREEN 2

// IMPORTANT: Run `make clean` after changing any of these settings.

// Settings for Emerald mode
#define FRLG_INCLUDE_ALL                FALSE               // Change this to toggle all configs on. You can also individually turn them on by setting each to TRUE.
#define FRLG_INCLUDE_OBJECT_EVENTS      FRLG_INCLUDE_ALL    // Include data for FRLG's object events
#define FRLG_INCLUDE_KANTO_TILESETS     FRLG_INCLUDE_ALL    // Include data for FRLG's tilesets
#define FRLG_INCLUDE_KANTO_MAPS         FRLG_INCLUDE_ALL    // Include data for FRLG's maps. IMPORTANT: Requires FRLG_INCLUDE_OBJECT_EVENTS and FRLG_INCLUDE_KANTO_TILESETS!!
                                                            // Flags and Vars are not handled, so events may be in a broken state.
#define FRLG_KANTO_MAP_WILD_PKMN        FIRE_RED            // Wild encounter data to be used when enabling FRLG_INCLUDE_KANTO_MAPS

// Settings for FRLG mode
#define EM_INCLUDE_ALL                  FALSE               // Change this to toggle all configs on. You can also individually turn them on by setting each to TRUE.
#define EM_INCLUDE_HOENN_TILESETS       EM_INCLUDE_ALL      // Include data for Emerald's tilesets.
#define EM_INCLUDE_HOENN_MAPS           EM_INCLUDE_ALL      // Include data for Emerald's maps. IMPORTANT: Requires EM_INCLUDE_HOENN_TILESETS!!
                                                            // Flags and Vars are not handled, so events may be in a broken state.

#endif // GUARD_CONFIG_FRLG_H
