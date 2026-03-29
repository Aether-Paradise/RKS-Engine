#ifndef GUARD_CONFIG_SAVE_H
#define GUARD_CONFIG_SAVE_H

// Menu configs
#define SKIP_SAVE_CONFIRMATION              FALSE   // If TRUE, skips the "There is already a saved file" confirmation when overwriting a save.

// SaveBlock1 configs
#define FREE_TRAINER_HILL                   TRUE   // Frees up Trainer Hill data (28 bytes).
#define FREE_TRAINER_TOWER                  TRUE   // Frees up Trainer Tower data (52 bytes).
#define FREE_MYSTERY_EVENT_BUFFERS          TRUE   // Frees up ramScript (1104 bytes).
#define FREE_MATCH_CALL                     TRUE   // Frees up match call and rematch / VS Seeker data. (104 bytes).
#define FREE_UNION_ROOM_CHAT                TRUE   // Frees up union room chat (212 bytes).
#define FREE_ENIGMA_BERRY                   TRUE   // Frees up E-Reader Enigma Berry data (52 bytes).
#define FREE_LINK_BATTLE_RECORDS            TRUE   // Frees up link battle record data (88 bytes).
#define FREE_MYSTERY_GIFT                   TRUE   // Frees up Mystery Gift data (876 bytes).
#define FREE_SECRET_BASES                   TRUE   // Frees up Secret Base data (3200 bytes).
                                            // SaveBlock1 total: 5716 bytes
// SaveBlock2 configs
#define FREE_BATTLE_TOWER_E_READER          TRUE   // Frees up Battle Tower E-Reader data (188 bytes).
#define FREE_POKEMON_JUMP                   TRUE   // Frees up Pokémon Jump data (16 bytes).
#define FREE_RECORD_MIXING_HALL_RECORDS     TRUE   // Frees up hall records for record mixing (1032 bytes).
#define FREE_FRONTIER_APPRENTICES           TRUE   // Frees up Battle Frontier Apprentices (396 bytes).
                                            // SaveBlock2 total: 1632 bytes

                                            // Grand Total: 7348 bytes

// CI config override
#include "config/override/save.h"

#endif // GUARD_CONFIG_SAVE_H
