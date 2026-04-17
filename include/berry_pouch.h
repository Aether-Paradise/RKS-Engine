#ifndef GUARD_BERRY_POUCH_H
#define GUARD_BERRY_POUCH_H

#include "task.h"

enum BerryPouchType
{
    BERRYPOUCH_FIELD,
    BERRYPOUCH_GIVE_PARTY,
    BERRYPOUCH_SELL,
    BERRYPOUCH_GIVE_PC,
    BERRYPOUCH_BATTLE,
    BERRYPOUCH_BERRY_BLENDER_CRUSH,
    BERRYPOUCH_BERRY_TREE,
    BERRYPOUCH_REOPENING
};

// Alternative value for 'allowSelectClose' argument to InitTMCase.
// Indicates that the previous value should be preserved
#define BERRYPOUCH_KEEP_PREV 0xFF

void BerryPouch_StartFadeToExitCallback(u8 taskId);
void BerryPouch_SetExitCallback(void (*exitCallback)(void));
void InitBerryPouch(u8 type, void (*savedCallback)(void), u8 allowSelect);
void DisplayItemMessageInBerryPouch(u8 taskId, u8 fontId, const u8 * str, TaskFunc followUpFunc);
void Task_BerryPouch_DestroyDialogueWindowAndRefreshListMenu(u8 taskId);
void BerryPouch_CursorResetToTop(void);
bool32 CheckIfInBerryPouch(void);

#endif //GUARD_BERRY_POUCH_H
