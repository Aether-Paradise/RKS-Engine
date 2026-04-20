#ifndef GUARD_ITEM_PC_RG_H
#define GUARD_ITEM_PC_RG_H

#include "main.h"

void ItemPc_RG_Init(u8 hasNoCallback, MainCallback callback);
void ItemPc_RG_SetInitializedFlag(bool8 flag);
u16 ItemPc_RG_GetItemIdBySlotId(u16 itemIndex);
u8 ItemPc_RG_GetCursorPosition(void);
void CB2_ReturnToItemPcMenu_RG(void);

#endif //GUARD_ITEM_PC_RG_H
