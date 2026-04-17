#ifndef GUARD_ITEM_MENU_FRLG_H
#define GUARD_ITEM_MENU_FRLG_H

#include "global.h"
#include "task.h"
#include "constants/item_menu_frlg.h"

// Exported type declarations

// Exported RAM declarations

enum {
    FRLG_POCKET_ITEMS,
    FRLG_POCKET_KEYITEMS,
    FRLG_POCKET_POKEBALLS,
    FRLG_POCKET_COUNT,
    OPEN_BAG_LAST = FRLG_POCKET_COUNT,
};

struct BagStruct
{
    MainCallback bagCallback;
    u8 location;
    bool8 bagOpen;
    u16 pocket;
    u16 bagTab;
    u16 cursorPos[FRLG_POCKET_COUNT];
    u16 scrollPosition[FRLG_POCKET_COUNT];
};

extern struct BagStruct gBagMenuState;

extern const struct CompressedSpriteSheet gBagSwapSpriteSheet;
extern const struct SpritePalette gBagSwapSpritePalette;

// Exported ROM declarations
void ResetBagCursorPositions(void);
void CB2_BagMenuFromStartMenuFrlg(void);
void ItemMenu_StartFadeToExitCallback(u8 taskId);
void Bag_BeginCloseWin0Animation(void);
void ItemMenu_SetExitCallback(void (*)(void));
void DisplayItemMessageFrlg(u8 taskId, u8 fontId, const u8 * string, TaskFunc followUpFunc);
void Task_ReturnToBagFromContextMenu(u8 taskId);
void InitOldManBagFrlg(void);
void UpdatePocketItemListFrlg(u8 pocketId);
void UpdatePocketListPositionFrlg(u8 pocketId);
void GoToBagMenuFrlg(u8 menuType, u8 pocket, MainCallback callback);

#endif //GUARD_ITEM_MENU_FRLG_H
