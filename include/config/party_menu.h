#ifndef GUARD_CONFIG_PARTY_MENU_H
#define GUARD_CONFIG_PARTY_MENU_H

#define PARTY_MENU_DESIGN_EMERALD 0
#define PARTY_MENU_DESIGN_SWSH    1

// General party menu settings
#define RKSE_PARTY_MENU_DESIGN            PARTY_MENU_DESIGN_EMERALD

// SwSh party menu settings
#define SWSH_PARTY_MENU_PC_ACCESS         TRUE
#define SWSH_PARTY_MON_IDLE_ANIMS         TRUE
#define SWSH_PARTY_MON_IDLE_ANIMS_FRAMES  300 // Number of frames before mon animation loops


// Defines for retrocompatibility
#define SWSH_PARTY_MENU (RKSE_PARTY_MENU_DESIGN == PARTY_MENU_DESIGN_SWSH)

// CI config override
#include "config/override/party_menu.h"

#endif // GUARD_CONFIG_PARTY_MENU_H
