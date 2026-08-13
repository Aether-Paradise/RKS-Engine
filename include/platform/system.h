#ifndef GUARD_SYSTEM_H
#define GUARD_SYSTEM_H

void RunDMAsAndVBlank(void);
#define ENTER_VBLANK() REG_VCOUNT = 161
#endif