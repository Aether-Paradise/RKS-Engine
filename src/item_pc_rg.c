#include "global.h"
#include "bg.h"
#include "data.h"
#include "decompress.h"
#include "fldeff_misc.h"
#include "gpu_regs.h"
#include "graphics.h"
#include "item.h"
#include "item_menu.h"
#include "item_menu_frlg.h"
#include "item_icon.h"
#include "list_menu.h"
#include "item_pc_rg.h"
#include "item_use.h"
#include "item_menu_icons_rg.h"
#include "link.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "palette.h"
#include "party_menu.h"
#include "pc_screen_effect.h"
#include "scanline_effect.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "constants/items.h"
#include "constants/rgb.h"
#include "constants/songs.h"

enum {
    ITEM_PC_WINDOW_ITEM_LIST,
    ITEM_PC_WINDOW_DESCRIPTION,
    ITEM_PC_WINDOW_WITHDRAW_ITEM,
    ITEM_PC_WINDOW_WITHDRAW_AMOUNT,
    ITEM_PC_WINDOW_ITEM_OPTIONS,
    ITEM_PC_WINDOW_MESSAGE,
};

enum {
    ITEM_PC_SUBWINDOW_SELECTED,
    ITEM_PC_SUBWINDOW_HOW_MANY,
    ITEM_PC_SUBWINDOW_WITHDRAW,
    ITEM_PC_SUBWINDOW_COUNT,
};

enum {
    ITEM_PC_COLORID_WHITE,
    ITEM_PC_COLORID_DARK_GRAY,
    ITEM_PC_COLORID_LIGHT_GRAY,
    ITEM_PC_COLORID_MESSAGE,
    ITEM_PC_COLOR_CURSOR_ERASE = 0xFF
};

enum {
    ITEM_PC_OPTION_WITHDRAW,
    ITEM_PC_OPTION_GIVE,
    ITEM_PC_OPTION_CANCEL,
};

struct ItemPcResources
{
    MainCallback savedCallback;
    u8 moveModeOrigPos;
    u8 itemIconSlot;
    u8 maxShowed;
    u8 nItems;
    u8 scrollIndicatorArrowPairId;
    u16 withdrawQuantitySubmenuCursorPos;
    s16 data[3];
};

struct ItemPcStaticResources
{
    MainCallback savedCallback;
    u16 scroll;
    u16 row;
    u8 initialized;
};

static EWRAM_DATA struct ItemPcResources * sItemPcRGResources = NULL;
static EWRAM_DATA u8 * sBg1TilemapBuffer = NULL;
static EWRAM_DATA struct ListMenuItem * sListMenuItems = NULL;
static EWRAM_DATA struct ItemPcStaticResources sItemPcRGStaticResources = {};
static EWRAM_DATA u8 sSubmenuWindowIds[ITEM_PC_SUBWINDOW_COUNT] = {};

static void ItemPc_RunSetup(void);
static bool8 ItemPc_DoGfxSetup(void);
static void ItemPc_FadeAndBail(void);
static void Task_ItemPcWaitFadeAndBail(u8 taskId);
static bool8 ItemPc_InitBgs(void);
static bool8 ItemPc_LoadGraphics(void);
static bool8 ItemPc_AllocateResourcesForListMenu(void);
static void ItemPc_BuildListMenuTemplate(void);
static void ItemPc_MoveCursorCallback(s32 itemIndex, bool8 onInit, struct ListMenu * list);
static void ItemPc_ItemPrintFunc(u8 windowId, u32 itemId, u8 y);
static void ItemPc_PrintOrRemoveCursorAt(u8 y, u8 state);
static void ItemPc_PrintWithdrawItem(void);
static void ItemPc_PlaceTopMenuScrollIndicatorArrows(void);
static void ItemPc_SetCursorPosition(void);
static void ItemPc_FreeResources(void);
static void Task_ItemPcTurnOff2(u8 taskId);
static u16 ItemPc_GetItemQuantityBySlotId(u16 itemIndex);
static void ItemPc_CountPcItems(void);
static void ItemPc_SetScrollPosition(void);
static void Task_ItemPcMain(u8 taskId);
static void ItemPc_MoveItemModeInit(u8 taskId, s16 pos);
static void Task_ItemPcMoveItemModeRun(u8 taskId);
static void ItemPc_InsertItemIntoNewSlot(u8 taskId, u32 pos);
static void ItemPc_MoveItemModeCancel(u8 taskId, u32 pos);
static void Task_ItemPcSubmenuInit(u8 taskId);
static void Task_ItemPcSubmenuRun(u8 taskId);
static void Task_ItemPcWithdraw(u8 taskId);
static void ItemPc_DoWithdraw(u8 taskId);
static void Task_ItemPcWaitButtonAndFinishWithdrawMultiple(u8 taskId);
static void Task_ItemPcWaitButtonWithdrawMultipleFailed(u8 taskId);
static void Task_ItemPcCleanUpWithdraw(u8 taskId);
static void ItemPc_WithdrawMultipleInitWindow(u16 slotId);
static void Task_ItemPcHandleWithdrawMultiple(u8 taskId);
static void Task_ItemPcGive(u8 taskId);
static void gTask_ItemPcWaitButtonAndExitSubmenu(u8 taskId);
static void Task_ItemPcCancel(u8 taskId);
static void ItemPc_InitWindows(void);
static void ItemPc_AddTextPrinterParameterized(u8 windowId, u8 fontId, const u8 * str, u8 x, u8 y, u8 letterSpacing, u8 lineSpacing, u8 speed, u8 colorIdx);
static void ItemPc_SetBorderStyleOnWindow(u8 windowId);
static u8 ItemPc_GetOrCreateSubwindow(u8 idx);
static void ItemPc_DestroySubwindow(u8 idx);
static void ItemPc_PrintOnMessageWithContinueTask(u8 taskId, const u8 * str, TaskFunc taskFunc);

static const struct BgTemplate sBgTemplates[2] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .priority = 0
    }, {
        .bg = 1,
        .charBaseIndex = 3,
        .mapBaseIndex = 30,
        .priority = 1
    }
};

static const u8 sText_Withdraw[] = _("Withdraw");
static const u8 sText_WithdrawItem[] = _("WITHDRAW\nITEM");
static const u8 sText_ReturnToPC[] = _("Return to the PC.");
static const u8 sText_WithdrawHowManyItems[] = _("Withdraw how many\n{STR_VAR_1}?");
static const u8 sText_WithdrawXItems[] = _("Withdrew {STR_VAR_2}\n{STR_VAR_1}.");
static const u8 sText_NoRoomInBag[] = _("There is no more\nroom in the BAG.");
static const u8 sText_WhereShouldTheStrVar1BePlaced[] = _("Where should the {STR_VAR_1}\nbe placed?");

static const struct MenuAction sItemPcSubmenuOptions[] =
{
    [ITEM_PC_OPTION_WITHDRAW] = { sText_Withdraw,     {Task_ItemPcWithdraw} },
    [ITEM_PC_OPTION_GIVE]     = { gText_Give,         {Task_ItemPcGive} },
    [ITEM_PC_OPTION_CANCEL]   = { gText_Cancel,       {Task_ItemPcCancel} }
};

#define TEXT_COLOR_ITEM_PC_TRANSPARENT TEXT_COLOR_TRANSPARENT
#define TEXT_COLOR_ITEM_PC_DARK_GRAY TEXT_COLOR_DARK_GRAY
#define TEXT_COLOR_ITEM_PC_WHITE TEXT_COLOR_WHITE
#define TEXT_COLOR_ITEM_PC_LIGHT_GRAY TEXT_COLOR_LIGHT_GRAY
#define TEXT_COLOR_ITEM_PC_MESSAGE_NORMAL TEXT_COLOR_DARK_GRAY
#define TEXT_COLOR_ITEM_PC_MESSAGE_SHADOW TEXT_COLOR_LIGHT_GRAY

static const u8 sTextColors[][3] =
{
    [ITEM_PC_COLORID_WHITE]      = {TEXT_COLOR_ITEM_PC_TRANSPARENT, TEXT_COLOR_ITEM_PC_WHITE, TEXT_COLOR_ITEM_PC_DARK_GRAY},
    [ITEM_PC_COLORID_DARK_GRAY]  = {TEXT_COLOR_ITEM_PC_TRANSPARENT, TEXT_COLOR_ITEM_PC_DARK_GRAY, TEXT_COLOR_ITEM_PC_LIGHT_GRAY},
    [ITEM_PC_COLORID_LIGHT_GRAY] = {TEXT_COLOR_ITEM_PC_TRANSPARENT, TEXT_COLOR_ITEM_PC_LIGHT_GRAY, TEXT_COLOR_ITEM_PC_DARK_GRAY},
    [ITEM_PC_COLORID_MESSAGE]    = {TEXT_COLOR_ITEM_PC_TRANSPARENT, TEXT_COLOR_ITEM_PC_MESSAGE_NORMAL, TEXT_COLOR_ITEM_PC_MESSAGE_SHADOW}
};

static const struct WindowTemplate sWindowTemplates[] =
{
    [ITEM_PC_WINDOW_ITEM_LIST] =
    {
        .bg = 0,
        .tilemapLeft = 7,
        .tilemapTop = 1,
        .width = 19,
        .height = 12,
        .paletteNum = 15,
        .baseBlock = 0x02bf
    },
    [ITEM_PC_WINDOW_DESCRIPTION] =
    {
        .bg = 0,
        .tilemapLeft = 5,
        .tilemapTop = 14,
        .width = 25,
        .height = 6,
        .paletteNum = 15,
        .baseBlock = 0x0229
    },
    [ITEM_PC_WINDOW_WITHDRAW_ITEM] =
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 5,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x0215
    },
    [ITEM_PC_WINDOW_WITHDRAW_AMOUNT] =
    {
        .bg = 0,
        .tilemapLeft = 24,
        .tilemapTop = 15,
        .width = 5,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x0201
    },
    [ITEM_PC_WINDOW_ITEM_OPTIONS] =
    {
        .bg = 0,
        .tilemapLeft = 22,
        .tilemapTop = 13,
        .width = 7,
        .height = 6,
        .paletteNum = 15,
        .baseBlock = 0x01d7
    },
    [ITEM_PC_WINDOW_MESSAGE] =
    {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 15,
        .width = 26,
        .height = 4,
        .paletteNum = 13,
        .baseBlock = 0x016f
    },
    DUMMY_WIN_TEMPLATE
};

static const struct WindowTemplate sSubwindowTemplates[] =
{
    [ITEM_PC_SUBWINDOW_SELECTED] =
    {
        .bg = 0,
        .tilemapLeft = 6,
        .tilemapTop = 15,
        .width = 14,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x0137
    },
    [ITEM_PC_SUBWINDOW_HOW_MANY] =
    {
        .bg = 0,
        .tilemapLeft = 6,
        .tilemapTop = 15,
        .width = 16,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x0137
    },
    [ITEM_PC_SUBWINDOW_WITHDRAW] =
    {
        .bg = 0,
        .tilemapLeft = 6,
        .tilemapTop = 15,
        .width = 23,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x009b
    }
};

static const u8 sItemPcTiles[] = INCBIN_U8("graphics/item_pc_rg/bg.4bpp.smol");
static const u16 sItemPcBgPals[] = INCBIN_U16("graphics/item_pc_rg/bg.gbapal");
static const u32 sItemPcTilemap[] = INCBIN_U32("graphics/item_pc_rg/bg.bin.smolTM");

#define tListTaskId     data[0]
#define tPosition       data[1]
#define tMaxQuantity    data[2]
#define tQuantity       data[8]

// Item list ID for tPosition to indicate an item is not currently being swapped
#define NOT_SWAPPING 0xFF

void ItemPc_RG_Init(u8 hasNoCallback, MainCallback callback)
{
    u8 i;

    sItemPcRGResources = AllocZeroed(sizeof(*sItemPcRGResources));
    if (sItemPcRGResources == NULL)
    {
        SetMainCallback2(callback);
        return;
    }
    if (!hasNoCallback)
    {
        sItemPcRGStaticResources.savedCallback = callback;
        sItemPcRGStaticResources.scroll = sItemPcRGStaticResources.row = 0;
    }
    sItemPcRGResources->moveModeOrigPos = NOT_SWAPPING;
    sItemPcRGResources->itemIconSlot = 0;
    sItemPcRGResources->scrollIndicatorArrowPairId = TASK_NONE;
    sItemPcRGResources->savedCallback = 0;
    for (i = 0; i < 3; i++)
    {
        sItemPcRGResources->data[i] = 0;
    }
    SetMainCallback2(ItemPc_RunSetup);
}

static void ItemPc_MainCB(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void ItemPc_VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void ItemPc_RunSetup(void)
{
    while (TRUE)
    {
        if (ItemPc_DoGfxSetup())
            break;
        if (MenuHelpers_IsLinkActive())
            break;
    }
}

static bool8 ItemPc_DoGfxSetup(void)
{
    u8 taskId;
    switch (gMain.state)
    {
    case 0:
        SetVBlankHBlankCallbacksToNull();
        ClearScheduledBgCopiesToVram();
        gMain.state++;
        break;
    case 1:
        ScanlineEffect_Stop();
        gMain.state++;
        break;
    case 2:
        FreeAllSpritePalettes();
        gMain.state++;
        break;
    case 3:
        ResetPaletteFade();
        gMain.state++;
        break;
    case 4:
        ResetSpriteData();
        gMain.state++;
        break;
    case 5:
        ItemRG_ResetItemMenuIconState();
        gMain.state++;
        break;
    case 6:
        ResetTasks();
        gMain.state++;
        break;
    case 7:
        if (ItemPc_InitBgs())
        {
            sItemPcRGResources->data[0] = 0;
            gMain.state++;
        }
        else
        {
            ItemPc_FadeAndBail();
            return TRUE;
        }
        break;
    case 8:
        if (ItemPc_LoadGraphics())
            gMain.state++;
        break;
    case 9:
        ItemPc_InitWindows();
        gMain.state++;
        break;
    case 10:
        ItemPc_CountPcItems();
        ItemPc_SetCursorPosition();
        ItemPc_SetScrollPosition();
        gMain.state++;
        break;
    case 11:
        if (ItemPc_AllocateResourcesForListMenu())
            gMain.state++;
        else
        {
            ItemPc_FadeAndBail();
            return TRUE;
        }
        break;
    case 12:
        ItemPc_BuildListMenuTemplate();
        gMain.state++;
        break;
    case 13:
        ItemPc_PrintWithdrawItem();
        gMain.state++;
        break;
    case 14:
        ItemRG_CreateSwapLine();
        gMain.state++;
        break;
    case 15:
        taskId = CreateTask(Task_ItemPcMain, 0);
        gTasks[taskId].tListTaskId = ListMenuInit(&gMultiuseListMenuTemplate, sItemPcRGStaticResources.scroll, sItemPcRGStaticResources.row);
        gMain.state++;
        break;
    case 16:
        ItemPc_PlaceTopMenuScrollIndicatorArrows();
        gMain.state++;
        break;
    case 17:
        if (sItemPcRGStaticResources.initialized)
        {
            BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
        }
        gMain.state++;
        break;
    case 18:
        if (sItemPcRGStaticResources.initialized)
        {
            BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        }
        else
        {
            ComputerScreenOpenEffect(0, 0, 0);
            ItemPc_RG_SetInitializedFlag(TRUE);
            PlaySE(SE_PC_LOGIN);
        }
        gMain.state++;
        break;
    case 19:
        if (MenuHelpers_ShouldWaitForLinkRecv() != TRUE)
            gMain.state++;
        break;
    default:
        SetVBlankCallback(ItemPc_VBlankCB);
        SetMainCallback2(ItemPc_MainCB);
        return TRUE;
    }
    return FALSE;
}

static void ItemPc_FadeAndBail(void)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    CreateTask(Task_ItemPcWaitFadeAndBail, 0);
    SetVBlankCallback(ItemPc_VBlankCB);
    SetMainCallback2(ItemPc_MainCB);
}

static void Task_ItemPcWaitFadeAndBail(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sItemPcRGStaticResources.savedCallback);
        ItemPc_FreeResources();
        DestroyTask(taskId);
    }
}

static bool8 ItemPc_InitBgs(void)
{
    ResetAllBgsCoordinatesAndBgCntRegs();
    sBg1TilemapBuffer = Alloc(0x800);
    if (sBg1TilemapBuffer == NULL)
        return FALSE;
    memset(sBg1TilemapBuffer, 0, 0x800);
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sBgTemplates, NELEMS(sBgTemplates));
    SetBgTilemapBuffer(1, sBg1TilemapBuffer);
    ScheduleBgCopyTilemapToVram(1);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON);
    SetGpuReg(REG_OFFSET_BLDCNT , 0);
    ShowBg(0);
    ShowBg(1);
    return TRUE;
}

static bool8 ItemPc_LoadGraphics(void)
{
    switch (sItemPcRGResources->data[0])
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(1, sItemPcTiles, 0, 0, 0);
        sItemPcRGResources->data[0]++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            DecompressDataWithHeaderWram(sItemPcTilemap, sBg1TilemapBuffer);
            sItemPcRGResources->data[0]++;
        }
        break;
    case 2:
        LoadPalette(sItemPcBgPals, BG_PLTT_ID(0), 3 * PLTT_SIZE_4BPP);
        sItemPcRGResources->data[0]++;
        break;
    case 3:
        LoadCompressedSpriteSheet(&gBagSwapSpriteSheet);
        sItemPcRGResources->data[0]++;
        break;
    default:
        LoadSpritePalette(&gBagSwapSpritePalette);
        sItemPcRGResources->data[0] = 0;
        return TRUE;
    }
    return FALSE;
}

#define try_alloc(ptr__, size) ({ \
    void ** ptr = (void **)&(ptr__);             \
    *ptr = Alloc(size);                 \
    if (*ptr == NULL)                   \
    {                                   \
        ItemPc_FreeResources();                  \
        ItemPc_FadeAndBail();                  \
        return FALSE;                   \
    }                                   \
})

static bool8 ItemPc_AllocateResourcesForListMenu(void)
{
    try_alloc(sListMenuItems, sizeof(struct ListMenuItem) * (PC_ITEMS_COUNT + 1));
    return TRUE;
}

static void ItemPc_BuildListMenuTemplate(void)
{
    u16 i;

    for (i = 0; i < sItemPcRGResources->nItems; i++)
    {
        sListMenuItems[i].name = GetItemName(gSaveBlock1Ptr->pcItems[i].itemId);
        sListMenuItems[i].id = i;
    }
    sListMenuItems[i].name = gText_Cancel;
    sListMenuItems[i].id = LIST_CANCEL;

    gMultiuseListMenuTemplate.items = sListMenuItems;
    gMultiuseListMenuTemplate.totalItems = sItemPcRGResources->nItems + 1;
    gMultiuseListMenuTemplate.windowId = ITEM_PC_WINDOW_ITEM_LIST;
    gMultiuseListMenuTemplate.header_X = 0;
    gMultiuseListMenuTemplate.item_X = 9;
    gMultiuseListMenuTemplate.cursor_X = 1;
    gMultiuseListMenuTemplate.lettersSpacing = 1;
    gMultiuseListMenuTemplate.itemVerticalPadding = 2;
    gMultiuseListMenuTemplate.upText_Y = 2;
    gMultiuseListMenuTemplate.maxShowed = sItemPcRGResources->maxShowed;
    gMultiuseListMenuTemplate.fontId = FONT_SHORT;
    gMultiuseListMenuTemplate.cursorPal = TEXT_COLOR_ITEM_PC_DARK_GRAY;
    gMultiuseListMenuTemplate.fillValue = TEXT_COLOR_ITEM_PC_TRANSPARENT;
    gMultiuseListMenuTemplate.cursorShadowPal = TEXT_COLOR_ITEM_PC_LIGHT_GRAY;
    gMultiuseListMenuTemplate.moveCursorFunc = ItemPc_MoveCursorCallback;
    gMultiuseListMenuTemplate.itemPrintFunc = ItemPc_ItemPrintFunc;
    gMultiuseListMenuTemplate.scrollMultiple = 0;
    gMultiuseListMenuTemplate.cursorKind = 0;
}

static void ItemPc_MoveCursorCallback(s32 itemIndex, bool8 onInit, struct ListMenu * list)
{
    u16 itemId;
    const u8 * desc;
    if (onInit != TRUE)
        PlaySE(SE_SELECT);

    if (sItemPcRGResources->moveModeOrigPos == NOT_SWAPPING)
    {
        ItemRG_EraseItemIcon(sItemPcRGResources->itemIconSlot ^ 1);
        if (itemIndex != LIST_CANCEL)
        {
            itemId = ItemPc_RG_GetItemIdBySlotId(itemIndex);
            ItemRG_DrawItemIcon(itemId, sItemPcRGResources->itemIconSlot);
            desc = GetItemLongDescription(itemId);
        }
        else
        {
            ItemRG_DrawItemIcon(ITEM_LIST_END, sItemPcRGResources->itemIconSlot);
            desc = sText_ReturnToPC;
        }
        sItemPcRGResources->itemIconSlot ^= 1;
        FillWindowPixelBuffer(ITEM_PC_WINDOW_DESCRIPTION, 0);
        ItemPc_AddTextPrinterParameterized(ITEM_PC_WINDOW_DESCRIPTION, FONT_SHORT, desc, 0, 3, 2, 0, 0, ITEM_PC_COLORID_WHITE);
    }
}

static void ItemPc_ItemPrintFunc(u8 windowId, u32 itemId, u8 y)
{
    if (sItemPcRGResources->moveModeOrigPos != NOT_SWAPPING)
    {
        if (sItemPcRGResources->moveModeOrigPos == (u8)itemId)
            ItemPc_PrintOrRemoveCursorAt(y, ITEM_PC_COLORID_LIGHT_GRAY);
        else
            ItemPc_PrintOrRemoveCursorAt(y, ITEM_PC_COLOR_CURSOR_ERASE);
    }
    if (itemId != LIST_CANCEL)
    {
        u16 quantity = ItemPc_GetItemQuantityBySlotId(itemId);
        ConvertIntToDecimalStringN(gStringVar1, quantity, STR_CONV_MODE_RIGHT_ALIGN, 3);
        StringExpandPlaceholders(gStringVar4, gText_xVar1);
        ItemPc_AddTextPrinterParameterized(windowId, FONT_SMALL, gStringVar4, 110, y, 0, 0, TEXT_SKIP_DRAW, ITEM_PC_COLORID_DARK_GRAY);
    }
}

static void ItemPc_PrintOrRemoveCursor(u8 listMenuId, u8 colorIdx)
{
    ItemPc_PrintOrRemoveCursorAt(ListMenuGetYCoordForPrintingArrowCursor(listMenuId), colorIdx);
}

static void ItemPc_PrintOrRemoveCursorAt(u8 y, u8 colorIdx)
{
    if (colorIdx == ITEM_PC_COLOR_CURSOR_ERASE)
    {
        u8 maxWidth = GetFontAttribute(FONT_SHORT, FONTATTR_MAX_LETTER_WIDTH);
        u8 maxHeight = GetFontAttribute(FONT_SHORT, FONTATTR_MAX_LETTER_HEIGHT);
        FillWindowPixelRect(ITEM_PC_WINDOW_ITEM_LIST, 0, 0, y, maxWidth, maxHeight);
    }
    else
    {
        ItemPc_AddTextPrinterParameterized(ITEM_PC_WINDOW_ITEM_LIST, FONT_SHORT, gText_SelectorArrow2, 1, y, 0, 0, 0, colorIdx);
    }
}

static void ItemPc_PrintWithdrawItem(void)
{
    ItemPc_AddTextPrinterParameterized(ITEM_PC_WINDOW_WITHDRAW_ITEM, FONT_SMALL, sText_WithdrawItem, 0, 1, 0, 1, 0, ITEM_PC_COLORID_WHITE);
}

static void ItemPc_PlaceTopMenuScrollIndicatorArrows(void)
{
    sItemPcRGResources->scrollIndicatorArrowPairId = AddScrollIndicatorArrowPairParameterized(SCROLL_ARROW_UP, 128, 8, 104, sItemPcRGResources->nItems - sItemPcRGResources->maxShowed + 1, 110, 110, &sItemPcRGStaticResources.scroll);
}

static void ItemPc_PlaceWithdrawQuantityScrollIndicatorArrows(void)
{
    sItemPcRGResources->withdrawQuantitySubmenuCursorPos = 1;
    sItemPcRGResources->scrollIndicatorArrowPairId = AddScrollIndicatorArrowPairParameterized(SCROLL_ARROW_UP, 212, 120, 152, 2, 110, 110, &sItemPcRGResources->withdrawQuantitySubmenuCursorPos);
}

static void ItemPc_RemoveScrollIndicatorArrowPair(void)
{
    if (sItemPcRGResources->scrollIndicatorArrowPairId != TASK_NONE)
    {
        RemoveScrollIndicatorArrowPair(sItemPcRGResources->scrollIndicatorArrowPairId);
        sItemPcRGResources->scrollIndicatorArrowPairId = TASK_NONE;
    }
}

static void ItemPc_SetCursorPosition(void)
{
    if (sItemPcRGStaticResources.scroll != 0 && sItemPcRGStaticResources.scroll + sItemPcRGResources->maxShowed > sItemPcRGResources->nItems + 1)
        sItemPcRGStaticResources.scroll = (sItemPcRGResources->nItems + 1) - sItemPcRGResources->maxShowed;
    if (sItemPcRGStaticResources.scroll + sItemPcRGStaticResources.row >= sItemPcRGResources->nItems + 1)
    {
        if (sItemPcRGResources->nItems + 1 < 2)
            sItemPcRGStaticResources.row = 0;
        else
            sItemPcRGStaticResources.row = sItemPcRGResources->nItems;
    }
}

#define try_free(ptr) ({        \
    void ** ptr__ = (void **)&(ptr);   \
    if (*ptr__ != NULL)                \
        Free(*ptr__);                  \
})

static void ItemPc_FreeResources(void)
{
    try_free(sItemPcRGResources);
    try_free(sBg1TilemapBuffer);
    try_free(sListMenuItems);
    FreeAllWindowBuffers();
}

static void Task_ItemPcTurnOff1(u8 taskId)
{
    if (sItemPcRGStaticResources.initialized)
    {
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    }
    else
    {
        ComputerScreenCloseEffect(0, 0, 0);
        PlaySE(SE_PC_OFF);
    }
    gTasks[taskId].func = Task_ItemPcTurnOff2;
}

static void Task_ItemPcTurnOff2(u8 taskId)
{
    s16 * data = gTasks[taskId].data;

    if (!gPaletteFade.active && !IsComputerScreenCloseEffectActive())
    {
        DestroyListMenuTask(tListTaskId, &sItemPcRGStaticResources.scroll, &sItemPcRGStaticResources.row);
        if (sItemPcRGResources->savedCallback != NULL)
            SetMainCallback2(sItemPcRGResources->savedCallback);
        else
            SetMainCallback2(sItemPcRGStaticResources.savedCallback);
        ItemPc_RemoveScrollIndicatorArrowPair();
        ItemPc_FreeResources();
        DestroyTask(taskId);
    }
}

u8 ItemPc_RG_GetCursorPosition(void)
{
    return sItemPcRGStaticResources.scroll + sItemPcRGStaticResources.row;
}

u16 ItemPc_RG_GetItemIdBySlotId(u16 idx)
{
    return gSaveBlock1Ptr->pcItems[idx].itemId;
}

static u16 ItemPc_GetItemQuantityBySlotId(u16 idx)
{
    return gSaveBlock1Ptr->pcItems[idx].quantity;
}

static void ItemPc_CountPcItems(void)
{
    u16 i;

    CompactPCItems();
    sItemPcRGResources->nItems = 0;
    for (i = 0; i < PC_ITEMS_COUNT; sItemPcRGResources->nItems++, i++)
    {
        if (gSaveBlock1Ptr->pcItems[i].itemId == ITEM_NONE)
            break;
    }
    sItemPcRGResources->maxShowed = sItemPcRGResources->nItems + 1 <= 6 ? sItemPcRGResources->nItems + 1 : 6;
}

static void ItemPc_SetScrollPosition(void)
{
    u8 i;

    if (sItemPcRGStaticResources.row > 3)
    {
        for (i = 0; i <= sItemPcRGStaticResources.row - 3; sItemPcRGStaticResources.row--, sItemPcRGStaticResources.scroll++, i++)
        {
            if (sItemPcRGStaticResources.scroll + sItemPcRGResources->maxShowed == sItemPcRGResources->nItems + 1)
                break;
        }
    }
}

static void ItemPc_SetMessageWindowPalette(int palIdx)
{
    SetBgTilemapPalette(1, 0, 14, 30, 6, palIdx + 1);
    ScheduleBgCopyTilemapToVram(1);
}

void ItemPc_RG_SetInitializedFlag(bool8 flag)
{
    sItemPcRGStaticResources.initialized = flag;
}

static void Task_ItemPcMain(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    u16 scroll;
    u16 row;
    s32 input;

    if (!gPaletteFade.active && !IsComputerScreenOpenEffectActive())
    {
        if (JOY_NEW(SELECT_BUTTON))
        {
            ListMenuGetScrollAndRow(tListTaskId, &scroll, &row);
            if (scroll + row != sItemPcRGResources->nItems)
            {
                PlaySE(SE_SELECT);
                ItemPc_MoveItemModeInit(taskId, scroll + row);
                return;
            }
        }
        input = ListMenu_ProcessInput(tListTaskId);
        ListMenuGetScrollAndRow(tListTaskId, &sItemPcRGStaticResources.scroll, &sItemPcRGStaticResources.row);
        switch (input)
        {
        case LIST_NOTHING_CHOSEN:
            break;
        case LIST_CANCEL:
            PlaySE(SE_SELECT);
            ItemPc_RG_SetInitializedFlag(FALSE);
            gTasks[taskId].func = Task_ItemPcTurnOff1;
            break;
        default:
            PlaySE(SE_SELECT);
            ItemPc_SetMessageWindowPalette(1);
            ItemPc_RemoveScrollIndicatorArrowPair();
            tPosition = input;
            tMaxQuantity = ItemPc_GetItemQuantityBySlotId(input);
            ItemPc_PrintOrRemoveCursor(tListTaskId, ITEM_PC_COLORID_LIGHT_GRAY);
            gTasks[taskId].func = Task_ItemPcSubmenuInit;
            break;
        }
    }
}

static void ItemPc_ReturnFromSubmenu(u8 taskId)
{
    ItemPc_SetMessageWindowPalette(0);
    ItemPc_PlaceTopMenuScrollIndicatorArrows();
    gTasks[taskId].func = Task_ItemPcMain;
}

static void ItemPc_MoveItemModeInit(u8 taskId, s16 pos)
{
    s16 * data = gTasks[taskId].data;

    ListMenuSetTemplateField(tListTaskId, LISTFIELD_CURSORKIND, 1);
    tPosition = pos;
    sItemPcRGResources->moveModeOrigPos = pos;
    StringCopy(gStringVar1, GetItemName(ItemPc_RG_GetItemIdBySlotId(tPosition)));
    CopyItemNameHandlePlural(ItemPc_RG_GetItemIdBySlotId(tPosition), gStringVar1, ItemPc_GetItemQuantityBySlotId(tPosition));
    StringExpandPlaceholders(gStringVar4, sText_WhereShouldTheStrVar1BePlaced);
    FillWindowPixelBuffer(ITEM_PC_WINDOW_DESCRIPTION, 0x00);
    ItemPc_AddTextPrinterParameterized(ITEM_PC_WINDOW_DESCRIPTION, FONT_SHORT, gStringVar4, 0, 3, 2, 3, 0, ITEM_PC_COLORID_WHITE);
    ItemRG_UpdateSwapLinePos(-32, ListMenuGetYCoordForPrintingArrowCursor(tListTaskId));
    ItemRG_SetSwapLineInvisibility(FALSE);
    ItemPc_PrintOrRemoveCursor(tListTaskId, ITEM_PC_COLORID_LIGHT_GRAY);
    gTasks[taskId].func = Task_ItemPcMoveItemModeRun;
}

static void Task_ItemPcMoveItemModeRun(u8 taskId)
{
    s16 * data = gTasks[taskId].data;

    ListMenu_ProcessInput(tListTaskId);
    ListMenuGetScrollAndRow(tListTaskId, &sItemPcRGStaticResources.scroll, &sItemPcRGStaticResources.row);
    ItemRG_UpdateSwapLinePos(-32, ListMenuGetYCoordForPrintingArrowCursor(tListTaskId));
    if (JOY_NEW(A_BUTTON | SELECT_BUTTON))
    {
        PlaySE(SE_SELECT);
        sItemPcRGResources->moveModeOrigPos = NOT_SWAPPING;
        ItemPc_InsertItemIntoNewSlot(taskId, sItemPcRGStaticResources.scroll + sItemPcRGStaticResources.row);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        sItemPcRGResources->moveModeOrigPos = NOT_SWAPPING;
        ItemPc_MoveItemModeCancel(taskId, sItemPcRGStaticResources.scroll + sItemPcRGStaticResources.row);
    }
}

static void ItemPc_InsertItemIntoNewSlot(u8 taskId, u32 pos)
{
    s16 * data = gTasks[taskId].data;
    if (tPosition == pos || tPosition == pos - 1)
    {
        ItemPc_MoveItemModeCancel(taskId, pos);
    }
    else
    {
        MoveItemSlotInPC(gSaveBlock1Ptr->pcItems, tPosition, pos);
        DestroyListMenuTask(tListTaskId, &sItemPcRGStaticResources.scroll, &sItemPcRGStaticResources.row);
        if (tPosition < pos)
            sItemPcRGStaticResources.row--;
        ItemPc_BuildListMenuTemplate();
        tListTaskId = ListMenuInit(&gMultiuseListMenuTemplate, sItemPcRGStaticResources.scroll, sItemPcRGStaticResources.row);
        ItemRG_SetSwapLineInvisibility(TRUE);
        gTasks[taskId].func = Task_ItemPcMain;
    }
}

static void ItemPc_MoveItemModeCancel(u8 taskId, u32 pos)
{
    s16 * data = gTasks[taskId].data;

    DestroyListMenuTask(tListTaskId, &sItemPcRGStaticResources.scroll, &sItemPcRGStaticResources.row);
    if (tPosition < pos)
        sItemPcRGStaticResources.row--;
    ItemPc_BuildListMenuTemplate();
    tListTaskId = ListMenuInit(&gMultiuseListMenuTemplate, sItemPcRGStaticResources.scroll, sItemPcRGStaticResources.row);
    ItemRG_SetSwapLineInvisibility(TRUE);
    gTasks[taskId].func = Task_ItemPcMain;
}

static void Task_ItemPcSubmenuInit(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    u8 windowId;

    ItemPc_SetBorderStyleOnWindow(ITEM_PC_WINDOW_ITEM_OPTIONS);
    windowId = ItemPc_GetOrCreateSubwindow(ITEM_PC_SUBWINDOW_SELECTED);
    PrintMenuActionTextsAtPos(ITEM_PC_WINDOW_ITEM_OPTIONS, FONT_SHORT, 8, 2, GetFontAttribute(FONT_SHORT, FONTATTR_MAX_LETTER_HEIGHT) + 2, 3, sItemPcSubmenuOptions);
    InitMenuNormal(ITEM_PC_WINDOW_ITEM_OPTIONS, FONT_SHORT, 0, 2, GetFontAttribute(FONT_SHORT, FONTATTR_MAX_LETTER_HEIGHT) + 2, 3, 0);
    CopyItemName(ItemPc_RG_GetItemIdBySlotId(tPosition), gStringVar1);
    StringExpandPlaceholders(gStringVar4, gText_Var1IsSelected);
    ItemPc_AddTextPrinterParameterized(windowId, FONT_SHORT, gStringVar4, 0, 2, 1, 0, 0, ITEM_PC_COLORID_MESSAGE);
    ScheduleBgCopyTilemapToVram(0);
    gTasks[taskId].func = Task_ItemPcSubmenuRun;
}

static void Task_ItemPcSubmenuRun(u8 taskId)
{
    s8 input = Menu_ProcessInputNoWrap();
    switch (input)
    {
    case MENU_B_PRESSED:
        PlaySE(SE_SELECT);
        Task_ItemPcCancel(taskId);
        break;
    case MENU_NOTHING_CHOSEN:
        break;
    default:
        PlaySE(SE_SELECT);
        sItemPcSubmenuOptions[input].func.void_u8(taskId);
    }
}

static void Task_ItemPcWithdraw(u8 taskId)
{
    s16 * data = gTasks[taskId].data;

    ClearStdWindowAndFrameToTransparent(ITEM_PC_WINDOW_ITEM_OPTIONS, FALSE);
    ItemPc_DestroySubwindow(ITEM_PC_SUBWINDOW_SELECTED);
    ClearWindowTilemap(ITEM_PC_WINDOW_ITEM_OPTIONS);
    tQuantity = 1;
    if (ItemPc_GetItemQuantityBySlotId(tPosition) == 1)
    {
        PutWindowTilemap(ITEM_PC_WINDOW_ITEM_LIST);
        ScheduleBgCopyTilemapToVram(0);
        ItemPc_DoWithdraw(taskId);
    }
    else
    {
        PutWindowTilemap(ITEM_PC_WINDOW_ITEM_LIST);
        ItemPc_WithdrawMultipleInitWindow(tPosition);
        ItemPc_PlaceWithdrawQuantityScrollIndicatorArrows();
        gTasks[taskId].func = Task_ItemPcHandleWithdrawMultiple;
    }
}

static void ItemPc_DoWithdraw(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    u16 itemId = ItemPc_RG_GetItemIdBySlotId(tPosition);
    u8 windowId;

    if (AddBagItem(itemId, tQuantity))
    {
        CopyItemNameHandlePlural(itemId, gStringVar1, tQuantity);
        ConvertIntToDecimalStringN(gStringVar2, tQuantity, STR_CONV_MODE_LEFT_ALIGN, 3);
        StringExpandPlaceholders(gStringVar4, sText_WithdrawXItems);
        windowId = ItemPc_GetOrCreateSubwindow(ITEM_PC_SUBWINDOW_WITHDRAW);
        AddTextPrinterParameterized(windowId, FONT_SHORT, gStringVar4, 0, 2, 0, NULL);
        gTasks[taskId].func = Task_ItemPcWaitButtonAndFinishWithdrawMultiple;
    }
    else
    {
        windowId = ItemPc_GetOrCreateSubwindow(ITEM_PC_SUBWINDOW_WITHDRAW);
        AddTextPrinterParameterized(windowId, FONT_SHORT, sText_NoRoomInBag, 0, 2, 0, NULL);
        gTasks[taskId].func = Task_ItemPcWaitButtonWithdrawMultipleFailed;
    }
}

static void Task_ItemPcWaitButtonAndFinishWithdrawMultiple(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    u16 itemId;

    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        itemId = ItemPc_RG_GetItemIdBySlotId(tPosition);
        RemovePCItem(itemId, tQuantity);
        CompactPCItems();
        Task_ItemPcCleanUpWithdraw(taskId);
    }
}

static void Task_ItemPcWaitButtonWithdrawMultipleFailed(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Task_ItemPcCleanUpWithdraw(taskId);
    }
}

static void Task_ItemPcCleanUpWithdraw(u8 taskId)
{
    s16 * data = gTasks[taskId].data;

    ItemPc_DestroySubwindow(ITEM_PC_SUBWINDOW_WITHDRAW);
    PutWindowTilemap(ITEM_PC_WINDOW_DESCRIPTION);
    DestroyListMenuTask(tListTaskId, &sItemPcRGStaticResources.scroll, &sItemPcRGStaticResources.row);
    ItemPc_CountPcItems();
    ItemPc_SetCursorPosition();
    ItemPc_BuildListMenuTemplate();
    tListTaskId = ListMenuInit(&gMultiuseListMenuTemplate, sItemPcRGStaticResources.scroll, sItemPcRGStaticResources.row);
    ScheduleBgCopyTilemapToVram(0);
    ItemPc_ReturnFromSubmenu(taskId);
}

static void ItemPc_WithdrawMultipleInitWindow(u16 slotId)
{
    u16 itemId = ItemPc_RG_GetItemIdBySlotId(slotId);

    CopyItemNameHandlePlural(itemId, gStringVar1, 2);
    StringExpandPlaceholders(gStringVar4, sText_WithdrawHowManyItems);
    AddTextPrinterParameterized(ItemPc_GetOrCreateSubwindow(ITEM_PC_SUBWINDOW_HOW_MANY), FONT_SHORT, gStringVar4, 0, 2, 0, NULL);
    ConvertIntToDecimalStringN(gStringVar1, 1, STR_CONV_MODE_LEADING_ZEROS, 3);
    StringExpandPlaceholders(gStringVar4, gText_xVar1);
    ItemPc_SetBorderStyleOnWindow(ITEM_PC_WINDOW_WITHDRAW_AMOUNT);
    ItemPc_AddTextPrinterParameterized(ITEM_PC_WINDOW_WITHDRAW_AMOUNT, FONT_SMALL, gStringVar4, 8, 10, 1, 0, 0, ITEM_PC_COLORID_MESSAGE);
    ScheduleBgCopyTilemapToVram(0);
}

static void UpdateWithdrawQuantityDisplay(s16 quantity)
{
    FillWindowPixelRect(ITEM_PC_WINDOW_WITHDRAW_AMOUNT, PIXEL_FILL(1), 10, 10, 28, 12);
    ConvertIntToDecimalStringN(gStringVar1, quantity, STR_CONV_MODE_LEADING_ZEROS, 3);
    StringExpandPlaceholders(gStringVar4, gText_xVar1);
    ItemPc_AddTextPrinterParameterized(ITEM_PC_WINDOW_WITHDRAW_AMOUNT, FONT_SMALL, gStringVar4, 8, 10, 1, 0, 0, ITEM_PC_COLORID_MESSAGE);
}

static void Task_ItemPcHandleWithdrawMultiple(u8 taskId)
{
    s16 * data = gTasks[taskId].data;

    if (AdjustQuantityAccordingToDPadInput(&tQuantity, tMaxQuantity))
        UpdateWithdrawQuantityDisplay(tQuantity);
    else if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        ItemPc_DestroySubwindow(ITEM_PC_SUBWINDOW_HOW_MANY);
        ClearWindowTilemap(ITEM_PC_WINDOW_WITHDRAW_AMOUNT);
        PutWindowTilemap(ITEM_PC_WINDOW_ITEM_LIST);
        ItemPc_PrintOrRemoveCursor(tListTaskId, ITEM_PC_COLORID_DARK_GRAY);
        ScheduleBgCopyTilemapToVram(0);
        ItemPc_RemoveScrollIndicatorArrowPair();
        ItemPc_DoWithdraw(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        ClearStdWindowAndFrameToTransparent(ITEM_PC_WINDOW_WITHDRAW_AMOUNT, FALSE);
        ItemPc_DestroySubwindow(ITEM_PC_SUBWINDOW_HOW_MANY);
        ClearWindowTilemap(ITEM_PC_WINDOW_WITHDRAW_AMOUNT);
        PutWindowTilemap(ITEM_PC_WINDOW_ITEM_LIST);
        PutWindowTilemap(ITEM_PC_WINDOW_DESCRIPTION);
        ItemPc_PrintOrRemoveCursor(tListTaskId, ITEM_PC_COLORID_DARK_GRAY);
        ScheduleBgCopyTilemapToVram(0);
        ItemPc_RemoveScrollIndicatorArrowPair();
        ItemPc_ReturnFromSubmenu(taskId);
    }
}

static void Task_ItemPcGive(u8 taskId)
{
    if (CalculatePlayerPartyCount() == 0)
    {
        ClearStdWindowAndFrameToTransparent(ITEM_PC_WINDOW_ITEM_OPTIONS, FALSE);
        ItemPc_DestroySubwindow(ITEM_PC_SUBWINDOW_SELECTED);
        ClearWindowTilemap(ITEM_PC_WINDOW_ITEM_OPTIONS);
        PutWindowTilemap(ITEM_PC_WINDOW_ITEM_LIST);
        ItemPc_PrintOnMessageWithContinueTask(taskId, gText_NoPokemon, gTask_ItemPcWaitButtonAndExitSubmenu);
    }
    else
    {
        sItemPcRGResources->savedCallback = CB2_ChooseMonToGiveItem_ItemPc_RG;
        Task_ItemPcTurnOff1(taskId);
    }
}

void CB2_ReturnToItemPcMenu_RG(void)
{
    ItemPc_RG_Init(TRUE, NULL);
}

static void gTask_ItemPcWaitButtonAndExitSubmenu(u8 taskId)
{
    s16 * data = gTasks[taskId].data;

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        ClearDialogWindowAndFrameToTransparent(ITEM_PC_WINDOW_MESSAGE, FALSE);
        ClearWindowTilemap(ITEM_PC_WINDOW_MESSAGE);
        PutWindowTilemap(ITEM_PC_WINDOW_DESCRIPTION);
        ItemPc_PrintOrRemoveCursor(tListTaskId, ITEM_PC_COLORID_DARK_GRAY);
        ScheduleBgCopyTilemapToVram(0);
        ItemPc_ReturnFromSubmenu(taskId);
    }
}

static void Task_ItemPcCancel(u8 taskId)
{
    s16 * data = gTasks[taskId].data;

    ClearStdWindowAndFrameToTransparent(ITEM_PC_WINDOW_ITEM_OPTIONS, FALSE);
    ItemPc_DestroySubwindow(ITEM_PC_SUBWINDOW_SELECTED);
    ClearWindowTilemap(ITEM_PC_WINDOW_ITEM_OPTIONS);
    PutWindowTilemap(ITEM_PC_WINDOW_ITEM_LIST);
    PutWindowTilemap(ITEM_PC_WINDOW_DESCRIPTION);
    ItemPc_PrintOrRemoveCursor(tListTaskId, ITEM_PC_COLORID_DARK_GRAY);
    ScheduleBgCopyTilemapToVram(0);
    ItemPc_ReturnFromSubmenu(taskId);
}

static void ItemPc_InitWindows(void)
{
    u8 i;

    InitWindows(sWindowTemplates);
    DeactivateAllTextPrinters();
    LoadUserWindowBorderGfx(ITEM_PC_WINDOW_ITEM_LIST, 0x3C0, BG_PLTT_ID(14));
    LoadMessageBoxGfx(ITEM_PC_WINDOW_ITEM_LIST, 0x3A3, BG_PLTT_ID(13));
    LoadPalette(gStandardMenuPalette, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
    for (i = ITEM_PC_WINDOW_ITEM_LIST; i < ITEM_PC_WINDOW_WITHDRAW_AMOUNT; i++)
    {
        FillWindowPixelBuffer(i, 0x00);
        PutWindowTilemap(i);
    }
    ScheduleBgCopyTilemapToVram(0);
    for (i = 0; i < ITEM_PC_SUBWINDOW_COUNT; i++)
        sSubmenuWindowIds[i] = WINDOW_NONE;
}

static void ItemPc_AddTextPrinterParameterized(u8 windowId, u8 fontId, const u8 * str, u8 x, u8 y, u8 letterSpacing, u8 lineSpacing, u8 speed, u8 colorIdx)
{
    AddTextPrinterParameterized4(windowId, fontId, x, y, letterSpacing, lineSpacing, sTextColors[colorIdx], speed, str);
}

static void ItemPc_SetBorderStyleOnWindow(u8 windowId)
{
    DrawStdFrameWithCustomTileAndPalette(windowId, FALSE, 0x3C0, 14);
}

static u8 ItemPc_GetOrCreateSubwindow(u8 idx)
{
    if (sSubmenuWindowIds[idx] == WINDOW_NONE)
    {
        sSubmenuWindowIds[idx] = AddWindow(&sSubwindowTemplates[idx]);
        DrawStdFrameWithCustomTileAndPalette(sSubmenuWindowIds[idx], TRUE, 0x3C0, 14);
    }

    return sSubmenuWindowIds[idx];
}

static void ItemPc_DestroySubwindow(u8 idx)
{
    ClearStdWindowAndFrameToTransparent(sSubmenuWindowIds[idx], FALSE);
    ClearWindowTilemap(sSubmenuWindowIds[idx]); // redundant
    RemoveWindow(sSubmenuWindowIds[idx]);
    sSubmenuWindowIds[idx] = WINDOW_NONE;
}

static void ItemPc_PrintOnMessageWithContinueTask(u8 taskId, const u8 * str, TaskFunc taskFunc)
{
    DisplayMessageAndContinueTask(taskId, ITEM_PC_WINDOW_MESSAGE, 0x3A3, 13, FONT_SHORT, GetPlayerTextSpeedDelay(), str, taskFunc);
    ScheduleBgCopyTilemapToVram(0);
}

#undef tListTaskId
#undef tPosition
#undef tMaxQuantity
#undef tQuantity