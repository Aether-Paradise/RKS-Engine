#include "global.h"
#include "main.h"
#include "battle.h"
#include "battle_controllers.h"
#include "bg.h"
#include "berry_pouch.h"
#include "decompress.h"
#include "event_scripts.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "graphics.h"
#include "gpu_regs.h"
#include "item.h"
#include "item_menu.h"
#include "item_menu_frlg.h"
#include "item_menu_icons_rg.h"
#include "item_use.h"
#include "link.h"
#include "list_menu.h"
#include "load_save.h"
#include "map_name_popup.h"
#include "malloc.h"
#include "mail.h"
#include "menu.h"
#include "money.h"
#include "overworld.h"
#include "party_menu.h"
#include "palette.h"
#include "pokemon_storage_system.h"
#include "scanline_effect.h"
#include "script.h"
#include "shop.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "tm_case.h"
#include "text_window.h"
#include "constants/items.h"
#include "constants/rgb.h"
#include "constants/songs.h"

#define FREE_IF_SET(ptr) ({ if (ptr) Free(ptr); })

struct BagMenuAlloc
{
    MainCallback exitCB;
    u8 itemOriginalLocation;
    u8 pocketSwitchMode:4;
    u8 itemMenuIcon:2;
    u8 inhibitItemDescriptionPrint:2;
    u16 contextMenuSelectedItem;
    u8 pocketScrollArrowsTask;
    u8 pocketSwitchArrowsTask;
    u8 numItemStacks[FRLG_POCKET_COUNT];
    u8 maxShowed[FRLG_POCKET_COUNT];
    u8 data[4];
};

struct BagSlots
{
    struct ItemSlot bagPocket_Items[BAG_ITEMS_COUNT];
    struct ItemSlot bagPocket_KeyItems[BAG_KEYITEMS_COUNT];
    struct ItemSlot bagPocket_PokeBalls[BAG_POKEBALLS_COUNT];
    u16 cursorPos[FRLG_POCKET_COUNT];
    u16 scrollPosition[FRLG_POCKET_COUNT];
    u16 registeredItem;
    u16 pocket;
};

enum {
    FRLG_BAG_WINDOW_ITEM_LIST,
    FRLG_BAG_WINDOW_DESCRIPTION,
    FRLG_BAG_WINDOW_POCKET_NAME,
    FRLG_BAG_WINDOW_COUNT,
};

enum {
    FRLG_BAG_SUBWINDOW_1,
    FRLG_BAG_SUBWINDOW_2,
    FRLG_BAG_SUBWINDOW_MONEY,
    FRLG_BAG_SUBWINDOW_TOSS_YESNO,
    FRLG_BAG_SUBWINDOW_SELL_YESNO,
    FRLG_BAG_SUBWINDOW_MESSAGE,
    FRLG_BAG_SUBWINDOW_7,
    FRLG_BAG_SUBWINDOW_8,
    FRLG_BAG_SUBWINDOW_9,
    FRLG_BAG_SUBWINDOW_10,
    FRLG_BAG_SUBWINDOW_11,
    FRLG_BAG_SUBWINDOW_12,
    FRLG_BAG_SUBWINDOW_13,
    FRLG_BAG_SUBWINDOW_14,
};

static const u8 sTextColors[][3] = {
    {0, 1, 2},
    {0, 2, 3},
    {0, 3, 2},
    {0, 8, 9}
};

static const struct WindowTemplate sDefaultBagWindowsStd[] = {
    [FRLG_BAG_WINDOW_ITEM_LIST] =
    {
        .bg = 0,
        .tilemapLeft = 11,
        .tilemapTop = 1,
        .width = 18,
        .height = 12,
        .paletteNum = 15,
        .baseBlock = 0x008a
    }, {
        .bg = 0,
        .tilemapLeft = 5,
        .tilemapTop = 14,
        .width = 25,
        .height = 6,
        .paletteNum = 15,
        .baseBlock = 0x0162
    }, {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 9,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 0x01f8
    },
    DUMMY_WIN_TEMPLATE
};

static const struct WindowTemplate sDefaultBagWindowsDeposit[] = {
    [FRLG_BAG_WINDOW_ITEM_LIST] =
    {
        .bg = 0,
        .tilemapLeft = 11,
        .tilemapTop = 1,
        .width = 18,
        .height = 12,
        .paletteNum = 15,
        .baseBlock = 0x008a
    }, {
        .bg = 0,
        .tilemapLeft = 5,
        .tilemapTop = 14,
        .width = 25,
        .height = 6,
        .paletteNum = 15,
        .baseBlock = 0x0162
    }, {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 8,
        .height = 2,
        .paletteNum = 13,
        .baseBlock = 0x01f8
    },
    DUMMY_WIN_TEMPLATE
};

static const struct WindowTemplate sWindowTemplates[] = {
    [FRLG_BAG_SUBWINDOW_1] =
    {
        .bg = 0,
        .tilemapLeft = 24,
        .tilemapTop = 15,
        .width = 5,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x242
    },
    [FRLG_BAG_SUBWINDOW_2] =
    {
        .bg = 0,
        .tilemapLeft = 17,
        .tilemapTop = 9,
        .width = 12,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x242
    },
    [FRLG_BAG_SUBWINDOW_MONEY] =
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 8,
        .height = 3,
        .paletteNum = 13,
        .baseBlock = 0x272
    },
    [FRLG_BAG_SUBWINDOW_TOSS_YESNO] =
    {
        .bg = 0,
        .tilemapLeft = 23,
        .tilemapTop = 15,
        .width = 6,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x28a
    },
    [FRLG_BAG_SUBWINDOW_SELL_YESNO] =
    {
        .bg = 0,
        .tilemapLeft = 21,
        .tilemapTop = 9,
        .width = 6,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x28a
    },
    [FRLG_BAG_SUBWINDOW_MESSAGE] =
    {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 15,
        .width = 26,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x2a2
    },
    [FRLG_BAG_SUBWINDOW_7] =
    {
        .bg = 0,
        .tilemapLeft = 6,
        .tilemapTop = 15,
        .width = 14,
        .height = 4,
        .paletteNum = 13,
        .baseBlock = 0x2a2
    },
    [FRLG_BAG_SUBWINDOW_8] =
    {
        .bg = 0,
        .tilemapLeft = 6,
        .tilemapTop = 15,
        .width = 15,
        .height = 4,
        .paletteNum = 13,
        .baseBlock = 0x2da
    },
    [FRLG_BAG_SUBWINDOW_9] =
    {
        .bg = 0,
        .tilemapLeft = 6,
        .tilemapTop = 15,
        .width = 16,
        .height = 4,
        .paletteNum = 13,
        .baseBlock = 0x316
    },
    [FRLG_BAG_SUBWINDOW_10] =
    {
        .bg = 0,
        .tilemapLeft = 6,
        .tilemapTop = 15,
        .width = 23,
        .height = 4,
        .paletteNum = 13,
        .baseBlock = 0x356
    },
    [FRLG_BAG_SUBWINDOW_11] =
    {
        .bg = 0,
        .tilemapLeft = 22,
        .tilemapTop = 17,
        .width = 7,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 0x20a
    },
    [FRLG_BAG_SUBWINDOW_12] =
    {
        .bg = 0,
        .tilemapLeft = 22,
        .tilemapTop = 15,
        .width = 7,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x20a
    },
    [FRLG_BAG_SUBWINDOW_13] =
    {
        .bg = 0,
        .tilemapLeft = 22,
        .tilemapTop = 13,
        .width = 7,
        .height = 6,
        .paletteNum = 15,
        .baseBlock = 0x20a
    },
    [FRLG_BAG_SUBWINDOW_14] =
    {
        .bg = 0,
        .tilemapLeft = 22,
        .tilemapTop = 11,
        .width = 7,
        .height = 8,
        .paletteNum = 15,
        .baseBlock = 0x20a
    }
};

static EWRAM_DATA u8 sOpenWindows[11] = {};

EWRAM_DATA struct BagStruct gBagMenuState = {};
static EWRAM_DATA struct BagMenuAlloc * sBagMenuDisplay = NULL;
static EWRAM_DATA void *sBagBgTilemapBuffer = NULL;
static EWRAM_DATA struct ListMenuItem * sListMenuItems = NULL;
static EWRAM_DATA u8 (*sListMenuItemStrings)[19] = NULL;
static EWRAM_DATA u8 sContextMenuItemsBuffer[4] = {};
static EWRAM_DATA const u8 *sContextMenuItemsPtr = NULL;
static EWRAM_DATA u8 sContextMenuNumItems = 0;
static EWRAM_DATA struct BagSlots * sBackupPlayerBag = NULL;

static void CB2_OpenBagMenu(void);
static bool8 LoadBagMenuGraphics(void);
static void FadeOutOfBagMenu(void);
static void Task_WaitFadeOutOfBagMenu(u8 taskId);
static void NullBagMenuBufferPtrs(void);
static bool8 BagMenuInitBgsAndAllocTilemapBuffer(void);
static bool8 DoLoadBagGraphics(void);
static u8 CreateBagInputHandlerTask(u8 location);
static bool8 TryAllocListMenuBuffers(void);
static void Bag_BuildListMenuTemplate(u8 pocket, u8 bagTab);
static void BagListMenuGetItemNameColored(u8 *dest, u16 itemId);
static void BagListMenuMoveCursorFunc(s32 itemIndex, bool8 onInit, struct ListMenu *list);
static void BagListMenuItemPrintFunc(u8 windowId, u32 itemId, u8 y);
static void bag_menu_print_cursor(u8 y, u8 colorIdx);
static void PrintBagPocketName(void);
static void PrintItemDescriptionOnMessageWindow(s32 itemIndex);
static void CreatePocketScrollArrowPair(void);
static void CreatePocketSwitchArrowPair(void);
static void BagDestroyPocketSwitchArrowPair(void);
static void CalculateInitialCursorPosAndItemsAbove(void);
static void InitPocketScrollPositionsFrlg(void);
static void DestroyBagMenuResources(void);
static void Task_ItemMenu_WaitFadeAndSwitchToExitCallback(u8 taskId);
static void Task_AnimateWin0v(u8 taskId);
static void ShowBagOrBeginWin0OpenTask(void);
static void UpdatePocketItemListsFrlg(void);
static void Task_BagMenu_HandleInput(u8 taskId);
static void Task_ItemContextMenuByLocation(u8 taskId);
static void Bag_FillMessageBoxWithPalette(u32 a0);
static u8 ProcessPocketSwitchInput(u8 taskId, u8 pocketId);
static void SwitchPockets(u8 taskId, s16 direction, bool16 a2);
static void Task_AnimateSwitchPockets(u8 taskId);
static void BeginMovingItemInPocket(u8 taskId, s16 itemIndex);
static void Task_MoveItemInPocket_HandleInput(u8 taskId);
static void ExecuteMoveItemInPocket(u8 taskId, u32 itemIndex);
static void AbortMovingItemInPocket(u8 taskId, u32 itemIndex);
static void DrawItemListRow(u8 row);
static void Task_ItemContext_FieldOrBattle(u8 taskId);
static void Task_FieldItemContextMenuHandleInput(u8 taskId);
static void Task_ItemMenuAction_Use(u8 taskId);
static void Task_ItemMenuAction_Toss(u8 taskId);
static void Task_ConfirmTossItems(u8 taskId);
static void Task_TossItem_No(u8 taskId);
static void Task_SelectQuantityToToss(u8 taskId);
static void Task_TossItem_Yes(u8 taskId);
static void Task_WaitAB_RedrawAndReturnToBag(u8 taskId);
static void Task_ItemMenuAction_ToggleSelect(u8 taskId);
static void Task_ItemMenuAction_Give(u8 taskId);
static void Task_PrintThereIsNoPokemon(u8 taskId);
static void Task_ItemMenuAction_Cancel(u8 taskId);
static void Task_PrintItemCantBeHeld(u8 taskId);
static void Task_WaitAButtonAndCloseContextMenu(u8 taskId);
static void Task_ItemMenuAction_BattleUse(u8 taskId);
static void Task_ItemContext_FieldGive(u8 taskId);
static void GoToTMCase_Give(void);
static void GoToBerryPouch_Give(void);
static void ReturnToBagMenuFromSubmenu_Give(void);
static void Task_ItemContext_PcBoxGive(u8 taskId);
static void GoToTMCase_PCBox(void);
static void GoToBerryPouch_PCBox(void);
static void ReturnToBagMenuFromSubmenu_PCBox(void);
static void Task_ItemContext_Sell(u8 taskId);
static void GoToTMCase_Sell(void);
static void GoToBerryPouch_Sell(void);
static void ReturnToBagMenuFromSubmenu_Sell(void);
static void Task_PrintSaleConfirmationText(u8 taskId);
static void Task_ShowSellYesNoMenu(u8 taskId);
static void Task_SellItem_No(u8 taskId);
static void Task_InitSaleQuantitySelectInterface(u8 taskId);
static void UpdateSalePriceDisplay(s32 price);
static void Task_SelectQuantityToSell(u8 taskId);
static void Task_SellItem_Yes(u8 taskId);
static void Task_FinalizeSaleToShop(u8 taskId);
static void Task_WaitPressAB_AfterSell(u8 taskId);
static void Task_ItemContext_Deposit(u8 taskId);
static void Task_SelectQuantityToDeposit(u8 taskId);
static void Task_TryDoItemDeposit(u8 taskId);
static bool8 BagIsTutorial(void);
static void Task_Bag_OldManTutorial(u8 taskId);
static void Task_Pokedude_FadeFromBag(u8 taskId);
static void Task_Pokedude_WaitFadeAndExitBag(u8 taskId);
static void InitBagWindows(void);
static void BagPrintTextOnWindow(u8 windowId, u8 fontId, const u8 * str, u8 x, u8 y, u8 letterSpacing, u8 lineSpacing, u8 speed, u8 colorIdx);
static void BagPrintTextOnWin1CenteredColor0(const u8 * str);
static void BagDrawDepositItemTextBox(void);
static u8 ShowBagWindow(u8 whichWindow, u8 nItems);
static void HideBagWindow(u8 whichWindow);
static u8 OpenBagWindow(u8 whichWindow);
static void CloseBagWindow(u8 whichWindow);
static u8 GetBagWindow(u8 whichWindow);
static void BagCreateYesNoMenuBottomRight(u8 taskId, const struct YesNoFuncTable * ptrs);
static void BagCreateYesNoMenuTopRight(u8 taskId, const struct YesNoFuncTable * ptrs);
static void BagPrintMoneyAmount(void);
static void BagDrawTextBoxOnWindow(u8 windowId);

const u32 gBagBg_Gfx[] = INCBIN_U32("graphics/item_menu_frlg/bg.4bpp.lz");
const u32 gBagBg_Tilemap[] = INCBIN_U32("graphics/item_menu_frlg/bg.bin.lz");
const u32 gBagBg_ItemPC_Tilemap[] = INCBIN_U32("graphics/item_menu_frlg/bg_item_pc.bin.lz");
const u16 gBagBgPalette[] = INCBIN_U16("graphics/item_menu_frlg/bg.gbapal"); // palette 1 (Boy + misc Pal)
const u16 gBagBgPalette_FemaleOverride[] = INCBIN_U16("graphics/item_menu_frlg/bg_female.gbapal"); // palette 2 (Girl)

const u32 gBagMale_Gfx[] = INCBIN_U32("graphics/item_menu_frlg/bag_male.4bpp.lz");
const u32 gBagFemale_Gfx[] = INCBIN_U32("graphics/item_menu_frlg/bag_female.4bpp.lz");
const u16 gBag_Pal[] = INCBIN_U16("graphics/item_menu_frlg/bag.gbapal");

static const u16 sBagWindowPalF[] = INCBIN_U16("graphics/item_menu_frlg/bag_window_pal.gbapal");

static const struct CompressedSpriteSheet gSpriteSheet_BagMale = {
    .data = gBagMale_Gfx,
    .size = 0x2000,
    .tag = RG_TAG_BAG
};

static const struct CompressedSpriteSheet gSpriteSheet_BagFemale = {
    .data = gBagFemale_Gfx,
    .size = 0x2000,
    .tag = RG_TAG_BAG
};

static const struct SpritePalette gSpritePalette_Bag = {
    .data = gBag_Pal,
    .tag = RG_TAG_BAG
};

const struct CompressedSpriteSheet gBagSwapSpriteSheet =
{
    .data = gSwapLineGfx,
    .size = 0x100,
    .tag = RG_TAG_SWAP_LINE
};

const struct SpritePalette gBagSwapSpritePalette =
{
    .data = gSwapLinePal,
    .tag = RG_TAG_SWAP_LINE
};

static const struct BgTemplate sBgTemplates[2] = {
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0x000
    }, {
        .bg = 1,
        .charBaseIndex = 3,
        .mapBaseIndex = 30,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0x000
    }
};

#define LIST_TILES_WIDTH  18
#define LIST_TILES_HEIGHT 12

static const u16 sItemListTilemap[LIST_TILES_WIDTH * LIST_TILES_HEIGHT] = INCBIN_U16("graphics/item_menu_frlg/list.bin");

static const u8 sListItemTextColor_RegularItem[] = _("{COLOR_HIGHLIGHT_SHADOW DARK_GRAY TRANSPARENT LIGHT_GRAY}");

static const u8 sListItemTextColor_TmCase_BerryPouch[] = _("{COLOR_HIGHLIGHT_SHADOW BLUE TRANSPARENT LIGHT_BLUE}");

static const u8 sText_TossOutHowManyStrVar1s[] = _("Toss out how many\n{STR_VAR_1}?");
static const u8 sText_ThrowAwayStrVar2OfThisItemQM[] = _("Throw away {STR_VAR_2} of\nthis item?");
static const u8 sText_DepositItem[] = _("DEPOSIT ITEM");
static const u8 sText_NoRoomForItems[] = _("There's no room to\nstore items.");
static const u8 sText_DepositHowManyVar1[] = _("Deposit how many\n{STR_VAR_1}?");
static const u8 sText_DepositedVar2Var1s[] = _("Deposited {STR_VAR_2}\n{STR_VAR_1}.");
static const u8 sText_WhereShouldTheStrVar1BePlaced[] = _("Where should the {STR_VAR_1}\nbe placed?");
static const u8 sText_Check[] = _("CHECK");
static const u8 sText_Walk[] = _("WALK");
static const u8 sText_Open[] = _("OPEN");

static const struct MenuAction sItemMenuContextActions[] = {
    [ITEMMENUACTION_USE] =
    {
        gMenuText_Use,
        {.void_u8 = Task_ItemMenuAction_Use}
    },
    [ITEMMENUACTION_TOSS] = {gMenuText_Toss, {.void_u8 = Task_ItemMenuAction_Toss}},
    [ITEMMENUACTION_REGISTER] = {gMenuText_Register, {.void_u8 = Task_ItemMenuAction_ToggleSelect}},
    [ITEMMENUACTION_GIVE] = {gMenuText_Give, {.void_u8 = Task_ItemMenuAction_Give}},
    [ITEMMENUACTION_CANCEL] = {gText_Cancel, {.void_u8 = Task_ItemMenuAction_Cancel}},
    [ITEMMENUACTION_BATTLE_USE] = {gMenuText_Use, {.void_u8 = Task_ItemMenuAction_BattleUse}},
    [ITEMMENUACTION_CHECK] = {sText_Check, {.void_u8 = Task_ItemMenuAction_Use}},
    [ITEMMENUACTION_OPEN] = {sText_Open, {.void_u8 = Task_ItemMenuAction_Use}},
    [ITEMMENUACTION_OPEN_BERRIES] = {sText_Open, {.void_u8 = Task_ItemMenuAction_BattleUse}},
    [ITEMMENUACTION_WALK] = {sText_Walk, {.void_u8 = Task_ItemMenuAction_Use}},
    [ITEMMENUACTION_DESELECT] = {gText_Deselect, {.void_u8 = Task_ItemMenuAction_ToggleSelect}},
    [ITEMMENUACTION_DUMMY] = {COMPOUND_STRING(""), {.void_u8 = NULL}}
};

static const u8 sBagTabToPocket[FRLG_POCKET_COUNT] =
{
    [FRLG_POCKET_ITEMS] = POCKET_ITEMS,
    [FRLG_POCKET_KEYITEMS] = POCKET_KEY_ITEMS,
    [FRLG_POCKET_POKEBALLS] = POCKET_POKE_BALLS,
};

static const u8 sPocketToBagTab[POCKETS_COUNT] =
{
    [POCKET_ITEMS] = FRLG_POCKET_ITEMS,
    [POCKET_KEY_ITEMS] = FRLG_POCKET_KEYITEMS,
    [POCKET_POKE_BALLS] = FRLG_POCKET_POKEBALLS,
    [POCKET_BERRIES] = FRLG_POCKET_COUNT,
    [POCKET_TM_HM] = FRLG_POCKET_COUNT,
};

static const u8 sContextMenuItems_Field[][4] = {
    [FRLG_POCKET_ITEMS] =
    {
        ITEMMENUACTION_USE,
        ITEMMENUACTION_GIVE,
        ITEMMENUACTION_TOSS,
        ITEMMENUACTION_CANCEL
    },
    [FRLG_POCKET_KEYITEMS] =
    {
        ITEMMENUACTION_USE,
        ITEMMENUACTION_REGISTER,
        ITEMMENUACTION_CANCEL,
        ITEMMENUACTION_DUMMY
    },
    [FRLG_POCKET_POKEBALLS] =
    {
        ITEMMENUACTION_GIVE,
        ITEMMENUACTION_TOSS,
        ITEMMENUACTION_CANCEL,
        ITEMMENUACTION_DUMMY
    }
};

static const u8 sContextMenuItems_CheckGiveTossCancel[] = {
    ITEMMENUACTION_CHECK,
    ITEMMENUACTION_GIVE,
    ITEMMENUACTION_TOSS,
    ITEMMENUACTION_CANCEL
};

static const u8 sContextMenuItems_GiveIfNotKeyItemPocket[][2] = {
    [FRLG_POCKET_ITEMS] =
    {
        ITEMMENUACTION_GIVE,
        ITEMMENUACTION_CANCEL
    },
    [FRLG_POCKET_KEYITEMS] =
    {
        ITEMMENUACTION_CANCEL,
        ITEMMENUACTION_DUMMY
    },
    [FRLG_POCKET_POKEBALLS] =
    {
        ITEMMENUACTION_GIVE,
        ITEMMENUACTION_CANCEL
    }
};

static const u8 sContextMenuItems_Open[] = {
    ITEMMENUACTION_OPEN,
    ITEMMENUACTION_CANCEL
};

static const u8 sContextMenuItems_BattleUse[] = {
    ITEMMENUACTION_BATTLE_USE,
    ITEMMENUACTION_CANCEL
};

static const u8 sContextMenuItems_Cancel[] = {
    ITEMMENUACTION_CANCEL,
    ITEMMENUACTION_DUMMY
};

static const TaskFunc sItemContextTaskFuncs[] = {
    [ITEMMENULOCATION_FIELD]  = Task_ItemContext_FieldOrBattle,
    [ITEMMENULOCATION_PARTY]  = Task_ItemContext_FieldGive,
    [ITEMMENULOCATION_SHOP]   = Task_ItemContext_Sell,
    [ITEMMENULOCATION_ITEMPC] = Task_ItemContext_Deposit,
    [ITEMMENULOCATION_PCBOX]  = Task_ItemContext_PcBoxGive,
    [ITEMMENULOCATION_BATTLE] = Task_ItemContext_FieldOrBattle,
    [ITEMMENULOCATION_WALLY]  = NULL
};

static const struct YesNoFuncTable sYesNoMenu_Toss = {
    Task_TossItem_Yes,
    Task_TossItem_No
};

static const struct YesNoFuncTable sYesNoMenu_Sell = {
    Task_SellItem_Yes,
    Task_SellItem_No
};

static const struct ScrollArrowsTemplate sPocketSwitchArrowPairTemplate = {
    .firstArrowType = SCROLL_ARROW_LEFT,
    .firstX = 8,
    .firstY = 72,
    .secondArrowType = SCROLL_ARROW_RIGHT,
    .secondX = 72,
    .secondY = 72,
    .fullyUpThreshold = 0,
    .fullyDownThreshold = 2,
    .tileTag = 111,
    .palTag = 111,
    .palNum = 0,
};

static const u8 sBlit_SelectButton[] = INCBIN_U8("graphics/item_menu_frlg/select_button.4bpp");

#define tListTaskId        data[0]
#define tListPosition      data[1]
#define tQuantity          data[2]
#define tNeverRead         data[3]
#define tItemCount         data[8]
#define tMsgWindowId       data[10]
#define tPocketSwitchDir   data[11]
#define tPocketSwitchTimer data[12]
#define tPocketSwitchState data[13]

#define tSwitchDir     data[11]
#define tSwitchCounter data[12]
#define tSwitchState   data[13]

void GoToBagMenuFrlg(u8 location, u8 pocket, MainCallback bagCallback)
{
    u8 i;

    NullBagMenuBufferPtrs();
    sBagMenuDisplay = Alloc(sizeof(struct BagMenuAlloc));
    if (sBagMenuDisplay == NULL)
        SetMainCallback2(bagCallback);
    else
    {
        if (location != ITEMMENULOCATION_LAST)
            gBagMenuState.location = location;
        if (bagCallback != NULL)
            gBagMenuState.bagCallback = bagCallback;
        sBagMenuDisplay->exitCB = NULL;
        sBagMenuDisplay->itemOriginalLocation = 0xFF;
        sBagMenuDisplay->itemMenuIcon = 0;
        sBagMenuDisplay->inhibitItemDescriptionPrint = FALSE;
        sBagMenuDisplay->pocketScrollArrowsTask = TASK_NONE;
        sBagMenuDisplay->pocketSwitchArrowsTask = TASK_NONE;
        if (location == ITEMMENULOCATION_ITEMPC)
            sBagMenuDisplay->pocketSwitchMode = 1;
        else if (location == ITEMMENULOCATION_WALLY)
            sBagMenuDisplay->pocketSwitchMode = 2;
        else
            sBagMenuDisplay->pocketSwitchMode = 0;
        for (i = 0; i < 4; i++)
        {
            sBagMenuDisplay->data[i] = 0;
        }
        if (pocket != OPEN_BAG_LAST)
        {
            gBagMenuState.bagTab = pocket;
            gBagMenuState.pocket = sBagTabToPocket[pocket];
        }
        gTextFlags.autoScroll = FALSE;
        gSpecialVar_ItemId = ITEM_NONE;
        SetMainCallback2(CB2_OpenBagMenu);
    }
}

void CB2_BagMenuFromStartMenuFrlg(void)
{
    GoToBagMenuFrlg(ITEMMENULOCATION_FIELD, OPEN_BAG_LAST, CB2_ReturnToFieldWithOpenMenu);
}

static void CB2_BagMenuRun(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void VBlankCB_BagMenuRun(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void CB2_OpenBagMenu(void)
{
    while (1)
    {
        if (MenuHelpers_ShouldWaitForLinkRecv())
            break;
        if (LoadBagMenuGraphics())
            break;
        if (MenuHelpers_IsLinkActive())
            break;
    }
}

static bool8 LoadBagMenuGraphics(void)
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
        gPaletteFade.bufferTransferDisabled = TRUE;
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
        if (!MenuHelpers_IsLinkActive())
        {
            ResetTasks();
        }
        gMain.state++;
        break;
    case 7:
        if (BagMenuInitBgsAndAllocTilemapBuffer())
        {
            sBagMenuDisplay->data[0] = 0;
            gMain.state++;
        }
        else
        {
            FadeOutOfBagMenu();
            return TRUE;
        }
        break;
    case 8:
        if (DoLoadBagGraphics() == TRUE)
            gMain.state++;
        break;
    case 9:
        InitBagWindows();
        gMain.state++;
        break;
    case 10:
        UpdatePocketItemListsFrlg();
        CalculateInitialCursorPosAndItemsAbove();
        InitPocketScrollPositionsFrlg();
        gMain.state++;
        break;
    case 11:
        if (!TryAllocListMenuBuffers())
        {
            FadeOutOfBagMenu();
            return TRUE;
        }
        gMain.state++;
        break;
    case 12:
        Bag_BuildListMenuTemplate(gBagMenuState.pocket, gBagMenuState.bagTab);
        gMain.state++;
        break;
    case 13:
        if (gBagMenuState.location != ITEMMENULOCATION_ITEMPC)
            PrintBagPocketName();
        else
            BagDrawDepositItemTextBox();
        gMain.state++;
        break;
    case 14:
        taskId = CreateBagInputHandlerTask(gBagMenuState.location);
        gTasks[taskId].tListTaskId = ListMenuInit(&gMultiuseListMenuTemplate, gBagMenuState.scrollPosition[gBagMenuState.bagTab], gBagMenuState.cursorPos[gBagMenuState.bagTab]);
        gTasks[taskId].tNeverRead = 0;
        gTasks[taskId].tItemCount = 0;
        gMain.state++;
        break;
    case 15:
        ItemRG_CreateBagSprite(gBagMenuState.bagTab);
        gMain.state++;
        break;
    case 16:
        CreatePocketScrollArrowPair();
        CreatePocketSwitchArrowPair();
        gMain.state++;
        break;
    case 17:
        ItemRG_CreateSwapLine();
        gMain.state++;
        break;
    case 18:
        ShowBagOrBeginWin0OpenTask();
        gMain.state++;
        break;
    case 19:
        gPaletteFade.bufferTransferDisabled = FALSE;
        gMain.state++;
        break;
    default:
        SetVBlankCallback(VBlankCB_BagMenuRun);
        SetMainCallback2(CB2_BagMenuRun);
        return TRUE;
    }
    return FALSE;
}

static void FadeOutOfBagMenu(void)
{
    BeginNormalPaletteFade(PALETTES_ALL, -2, 0, 16, RGB_BLACK);
    CreateTask(Task_WaitFadeOutOfBagMenu, 0);
    SetVBlankCallback(VBlankCB_BagMenuRun);
    SetMainCallback2(CB2_BagMenuRun);
}

static void Task_WaitFadeOutOfBagMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(gBagMenuState.bagCallback);
        DestroyBagMenuResources();
        DestroyTask(taskId);
    }
}

static void NullBagMenuBufferPtrs(void)
{
    sBagMenuDisplay = NULL;
    sBagBgTilemapBuffer = NULL;
    sListMenuItems = NULL;
    sListMenuItemStrings = NULL;
}

static bool8 BagMenuInitBgsAndAllocTilemapBuffer(void)
{
    void **buff;
    ResetAllBgsCoordinatesAndBgCntRegs();
    buff = &sBagBgTilemapBuffer;
    *buff = Alloc(0x800);
    if (*buff == NULL)
        return FALSE;
    memset(*buff, 0, 0x800);
    ResetBgsAndClearDma3BusyFlags(FALSE);
    InitBgsFromTemplates(0, sBgTemplates, NELEMS(sBgTemplates));
    SetBgTilemapBuffer(1, *buff);
    ScheduleBgCopyTilemapToVram(1);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON | DISPCNT_WIN0_ON);
    ShowBg(0);
    ShowBg(1);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    return TRUE;
}

static bool8 DoLoadBagGraphics(void)
{
    switch (sBagMenuDisplay->data[0])
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(1, gBagBg_Gfx, 0, 0, 0);
        sBagMenuDisplay->data[0]++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            if (gBagMenuState.location != ITEMMENULOCATION_ITEMPC)
                DecompressDataWithHeaderWram(gBagBg_Tilemap, sBagBgTilemapBuffer);
            else
                DecompressDataWithHeaderWram(gBagBg_ItemPC_Tilemap, sBagBgTilemapBuffer);
            sBagMenuDisplay->data[0]++;
        }
        break;
    case 2:
        LoadPalette(gBagBgPalette, BG_PLTT_ID(0), 3 * PLTT_SIZE_4BPP);
        if (!BagIsTutorial() && gSaveBlock2Ptr->playerGender != MALE)
            LoadPalette(gBagBgPalette_FemaleOverride, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
        sBagMenuDisplay->data[0]++;
        break;
    case 3:
        if (BagIsTutorial() == TRUE || gSaveBlock2Ptr->playerGender == MALE)
            LoadCompressedSpriteSheet(&gSpriteSheet_BagMale);
        else
            LoadCompressedSpriteSheet(&gSpriteSheet_BagFemale);
        sBagMenuDisplay->data[0]++;
        break;
    case 4:
        LoadSpritePalette(&gSpritePalette_Bag);
        sBagMenuDisplay->data[0]++;
        break;
    case 5:
        LoadCompressedSpriteSheet(&gBagSwapSpriteSheet);
        sBagMenuDisplay->data[0]++;
        break;
    default:
        LoadSpritePalette(&gBagSwapSpritePalette);
        sBagMenuDisplay->data[0] = 0;
        return TRUE;
    }

    return FALSE;
}

static u8 CreateBagInputHandlerTask(u8 location)
{
    switch (location)
    {
    case ITEMMENULOCATION_WALLY:
        return CreateTask(Task_Bag_OldManTutorial, 0);
    default:
        return CreateTask(Task_BagMenu_HandleInput, 0);
    }
}

// The buffer for the bag item list needs to be large enough to hold the maximum
// number of item slots that could fit in a single pocket, + 1 for Cancel.
// This constant picks the max of the existing pocket sizes.
// By default, the largest pocket is BAG_ITEMS_COUNT at 30.
#define MAX_POCKET_ITEMS  ((max(BAG_ITEMS_COUNT,             \
                            max(BAG_KEYITEMS_COUNT,          \
                                BAG_POKEBALLS_COUNT))) + 1)

static bool8 TryAllocListMenuBuffers(void)
{
    // The items pocket has the highest capacity, + 1 for CANCEL
    sListMenuItems = Alloc((MAX_POCKET_ITEMS) * sizeof(struct ListMenuItem));
    if (sListMenuItems == NULL)
        return FALSE;
    sListMenuItemStrings = Alloc((MAX_POCKET_ITEMS) * sizeof(*sListMenuItemStrings));
    if (sListMenuItemStrings == NULL)
        return FALSE;
    return TRUE;
}

static void Bag_BuildListMenuTemplate(u8 pocket, u8 bagTab)
{
    u16 i;
    for (i = 0; i < sBagMenuDisplay->numItemStacks[bagTab]; i++)
    {
        BagListMenuGetItemNameColored(sListMenuItemStrings[i], GetBagItemId(pocket, i));
        sListMenuItems[i].name = sListMenuItemStrings[i];
        sListMenuItems[i].id = i;
    }
    StringCopy(sListMenuItemStrings[i], sListItemTextColor_RegularItem);
    StringAppend(sListMenuItemStrings[i], gText_Cancel);
    sListMenuItems[i].name = sListMenuItemStrings[i];
    sListMenuItems[i].id = i;
    gMultiuseListMenuTemplate.items = sListMenuItems;
    gMultiuseListMenuTemplate.totalItems = sBagMenuDisplay->numItemStacks[bagTab] + 1;
    gMultiuseListMenuTemplate.windowId = FRLG_BAG_WINDOW_ITEM_LIST;
    gMultiuseListMenuTemplate.header_X = 0;
    gMultiuseListMenuTemplate.item_X = 9;
    gMultiuseListMenuTemplate.cursor_X = 1;
    gMultiuseListMenuTemplate.lettersSpacing = 0;
    gMultiuseListMenuTemplate.itemVerticalPadding = 2;
    gMultiuseListMenuTemplate.upText_Y = 2;
    gMultiuseListMenuTemplate.maxShowed = sBagMenuDisplay->maxShowed[bagTab];
    gMultiuseListMenuTemplate.fontId = FONT_SHORT;
    gMultiuseListMenuTemplate.cursorPal = 2;
    gMultiuseListMenuTemplate.fillValue = 0;
    gMultiuseListMenuTemplate.cursorShadowPal = 3;
    gMultiuseListMenuTemplate.moveCursorFunc = BagListMenuMoveCursorFunc;
    gMultiuseListMenuTemplate.itemPrintFunc = BagListMenuItemPrintFunc;
    gMultiuseListMenuTemplate.cursorKind = 0;
    gMultiuseListMenuTemplate.scrollMultiple = 0;
}

static void BagListMenuGetItemNameColored(u8 *dest, u16 itemId)
{
    if (itemId == ITEM_TM_CASE || itemId == ITEM_BERRY_POUCH)
        StringCopy(dest, sListItemTextColor_TmCase_BerryPouch);
    else
        StringCopy(dest, sListItemTextColor_RegularItem);
    StringAppend(dest, GetItemName(itemId));
}

static void BagListMenuMoveCursorFunc(s32 itemIndex, bool8 onInit, struct ListMenu *list)
{
    if (onInit != TRUE)
    {
        PlaySE(SE_RG_BAG_CURSOR);
        ItemRG_ShakeBagSprite();
    }
    if (sBagMenuDisplay->itemOriginalLocation == 0xFF)
    {
        ItemRG_EraseItemIcon(sBagMenuDisplay->itemMenuIcon ^ 1);
        if (sBagMenuDisplay->numItemStacks[gBagMenuState.bagTab] != itemIndex)
            ItemRG_DrawItemIcon(GetBagItemId(gBagMenuState.pocket, itemIndex), sBagMenuDisplay->itemMenuIcon);
        else
            ItemRG_DrawItemIcon(ITEM_LIST_END, sBagMenuDisplay->itemMenuIcon);
        sBagMenuDisplay->itemMenuIcon ^= 1;
        if (!sBagMenuDisplay->inhibitItemDescriptionPrint)
            PrintItemDescriptionOnMessageWindow(itemIndex);
    }
}

static void BagListMenuItemPrintFunc(u8 windowId, u32 itemId, u8 y)
{
    u16 bagItemId;
    u16 bagItemQuantity;
    if (sBagMenuDisplay->itemOriginalLocation != 0xFF)
    {
        if (sBagMenuDisplay->itemOriginalLocation == (u8)itemId)
            bag_menu_print_cursor(y, 2);
        else
            bag_menu_print_cursor(y, 0xFF);
    }
    if (itemId != -2 && sBagMenuDisplay->numItemStacks[gBagMenuState.bagTab] != itemId)
    {
        bagItemId = GetBagItemId(gBagMenuState.pocket, itemId);
        bagItemQuantity = GetBagItemQuantity(gBagMenuState.pocket, itemId);
        if (gBagMenuState.pocket != POCKET_KEY_ITEMS && !GetItemImportance(bagItemId))
        {
            ConvertIntToDecimalStringN(gStringVar1, bagItemQuantity, STR_CONV_MODE_RIGHT_ALIGN, 3);
            StringExpandPlaceholders(gStringVar4, gText_xVar1);
            BagPrintTextOnWindow(windowId, FONT_SMALL, gStringVar4, 0x6e, y, 0, 0, 0xFF, 1);
        }
        else if (gSaveBlock1Ptr->registeredItem != ITEM_NONE && gSaveBlock1Ptr->registeredItem == bagItemId)
        {
            BlitBitmapToWindow(windowId, sBlit_SelectButton, 0x70, y, 0x18, 0x10);
        }
    }
}

static void bag_menu_print_cursor_(u8 taskId, u8 colorIdx)
{
    bag_menu_print_cursor(ListMenuGetYCoordForPrintingArrowCursor(taskId), colorIdx);
}

static void bag_menu_print_cursor(u8 y, u8 colorIdx)
{
    if (colorIdx == 0xFF)
    {
        FillWindowPixelRect(FRLG_BAG_WINDOW_ITEM_LIST, PIXEL_FILL(0), 1, y, GetMenuCursorDimensionByFont(FONT_SHORT, 0), GetMenuCursorDimensionByFont(FONT_SHORT, 1));
    }
    else
    {
        BagPrintTextOnWindow(FRLG_BAG_WINDOW_ITEM_LIST, FONT_SHORT, gText_SelectorArrow2, 1, y, 0, 0, 0, colorIdx);
    }
}

static void PrintBagPocketName(void)
{
    FillWindowPixelBuffer(FRLG_BAG_WINDOW_POCKET_NAME, PIXEL_FILL(0));
    BagPrintTextOnWin1CenteredColor0(gPocketNamesStringsTable[gBagMenuState.pocket]);
}

static void PrintItemDescriptionOnMessageWindow(s32 itemIndex)
{
    const u8 *description;
    if (itemIndex != sBagMenuDisplay->numItemStacks[gBagMenuState.bagTab])
        description = GetItemLongDescription(GetBagItemId(gBagMenuState.pocket, itemIndex));
    else
        description = gText_CloseBag;
    FillWindowPixelBuffer(FRLG_BAG_WINDOW_DESCRIPTION, PIXEL_FILL(0));
    BagPrintTextOnWindow(FRLG_BAG_WINDOW_DESCRIPTION, FONT_SHORT, description, 0, 3, 2, 0, 0, 0);
}

static void CreatePocketScrollArrowPair(void)
{
    if (gBagMenu->pocketScrollArrowsTask == TASK_NONE)
        sBagMenuDisplay->pocketScrollArrowsTask = AddScrollIndicatorArrowPairParameterized(
            SCROLL_ARROW_UP,
            160,
            8,
            104,
            sBagMenuDisplay->numItemStacks[gBagMenuState.bagTab] - sBagMenuDisplay->maxShowed[gBagMenuState.bagTab] + 1,
            110,
            110,
            &gBagMenuState.scrollPosition[gBagMenuState.bagTab]
    );
}

static void CreatePocketSwitchArrowPair(void)
{
    if (sBagMenuDisplay->pocketSwitchMode != 1)
    {
        sBagMenuDisplay->pocketSwitchArrowsTask = AddScrollIndicatorArrowPair(&sPocketSwitchArrowPairTemplate, &gBagMenuState.bagTab);
    }
}

static void CreatePocketScrollArrowPair_SellQuantity(void)
{
    sBagMenuDisplay->contextMenuSelectedItem = 1;
    sBagMenuDisplay->pocketScrollArrowsTask = AddScrollIndicatorArrowPairParameterized(
        SCROLL_ARROW_UP,
        152,
        72,
        104,
        2,
        110,
        110,
        &sBagMenuDisplay->contextMenuSelectedItem
    );
}

static void CreateArrowPair_QuantitySelect(void)
{
    sBagMenuDisplay->contextMenuSelectedItem = 1;
    sBagMenuDisplay->pocketScrollArrowsTask = AddScrollIndicatorArrowPairParameterized(
        SCROLL_ARROW_UP,
        212,
        120,
        152,
        2,
        110,
        110,
        &sBagMenuDisplay->contextMenuSelectedItem
    );
}

static void BagDestroyPocketScrollArrowPair(void)
{
    if (sBagMenuDisplay->pocketScrollArrowsTask != TASK_NONE)
    {
        RemoveScrollIndicatorArrowPair(sBagMenuDisplay->pocketScrollArrowsTask);
        sBagMenuDisplay->pocketScrollArrowsTask = TASK_NONE;
    }
    BagDestroyPocketSwitchArrowPair();
}

static void BagDestroyPocketSwitchArrowPair(void)
{
    if (sBagMenuDisplay->pocketSwitchArrowsTask != TASK_NONE)
    {
        RemoveScrollIndicatorArrowPair(sBagMenuDisplay->pocketSwitchArrowsTask);
        sBagMenuDisplay->pocketSwitchArrowsTask = TASK_NONE;
    }
}

void ResetBagCursorPositions(void)
{
    u8 i;
    gBagMenuState.bagTab = FRLG_POCKET_ITEMS;
    gBagMenuState.pocket = POCKET_ITEMS;
    gBagMenuState.bagOpen = FALSE;
    for (i = 0; i < FRLG_POCKET_COUNT; i++)
    {
        gBagMenuState.cursorPos[i] = 0;
        gBagMenuState.scrollPosition[i] = 0;
    }
}

void UpdatePocketListPositionFrlg(u8 pocketId)
{
    u8 bagTab = sPocketToBagTab[pocketId];

    if (bagTab == FRLG_POCKET_COUNT)
        return;

    SetCursorWithinListBounds(&gBagMenuState.scrollPosition[bagTab], &gBagMenuState.cursorPos[bagTab], sBagMenuDisplay->maxShowed[bagTab], sBagMenuDisplay->numItemStacks[bagTab] + 1);
}

static void CalculateInitialCursorPosAndItemsAbove(void)
{
    u8 i;
    for (i = 0; i < POCKETS_COUNT; i++)
    {
        UpdatePocketListPositionFrlg(i);
    }
}

static void InitPocketScrollPositionsFrlg(void)
{
    u8 i;

    for (i = 0; i < FRLG_POCKET_COUNT; i++)
    {
        SetCursorScrollWithinListBounds(&gBagMenuState.scrollPosition[i], &gBagMenuState.cursorPos[i], sBagMenuDisplay->maxShowed[i], sBagMenuDisplay->numItemStacks[i] + 1, 6);
    }
}

static void DestroyBagMenuResources(void)
{
    FREE_IF_SET(sBagMenuDisplay);
    FREE_IF_SET(sBagBgTilemapBuffer);
    FREE_IF_SET(sListMenuItems);
    FREE_IF_SET(sListMenuItemStrings);
    FreeAllWindowBuffers();
}

void ItemMenu_StartFadeToExitCallback(u8 taskId)
{
    BeginNormalPaletteFade(PALETTES_ALL, -2, 0, 16, RGB_BLACK);
    gTasks[taskId].func = Task_ItemMenu_WaitFadeAndSwitchToExitCallback;
}

static void Task_ItemMenu_WaitFadeAndSwitchToExitCallback(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (!gPaletteFade.active && !FuncIsActiveTask(Task_AnimateWin0v))
    {
        DestroyListMenuTask(tListTaskId, &gBagMenuState.scrollPosition[gBagMenuState.bagTab], &gBagMenuState.cursorPos[gBagMenuState.bagTab]);
        if (sBagMenuDisplay->exitCB != NULL)
            SetMainCallback2(sBagMenuDisplay->exitCB);
        else
            SetMainCallback2(gBagMenuState.bagCallback);
        BagDestroyPocketScrollArrowPair();
        DestroyBagMenuResources();
        DestroyTask(taskId);
    }
}

static void ShowBagOrBeginWin0OpenTask(void)
{
    u16 paldata = RGB_BLACK;
    u8 taskId;

    SetBackdropFromPalette(&paldata);
    SetGpuReg(REG_OFFSET_WININ, 0);
    SetGpuReg(REG_OFFSET_WINOUT, WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN0_CLR);
    BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
    BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
    if (gBagMenuState.bagOpen == TRUE)
    {
        SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(0, DISPLAY_WIDTH));
        SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(0, 0));
    }
    else
    {
        SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(0, DISPLAY_WIDTH));
        SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(0, DISPLAY_HEIGHT));
        taskId = CreateTask(Task_AnimateWin0v, 0);
        gTasks[taskId].data[0] = 192;
        gTasks[taskId].data[1] = -16;
        gBagMenuState.bagOpen = TRUE;
    }
}

void Bag_BeginCloseWin0Animation(void)
{

    u8 taskId = CreateTask(Task_AnimateWin0v, 0);
    gTasks[taskId].data[0] = -16;
    gTasks[taskId].data[1] =  16;
    gBagMenuState.bagOpen = FALSE;
}

static void Task_AnimateWin0v(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    data[0] += data[1];
    if (data[0] > 160)
        SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(0, 160));
    else
        SetGpuReg(REG_OFFSET_WIN0V, data[0]);
    if ((data[1] == 16 && data[0] == 160) || (data[1] == -16 && data[0] == 0))
        DestroyTask(taskId);
}

void UpdatePocketItemListFrlg(u8 pocketId)
{
    u16 bagTab;
    struct BagPocket * pocket = &gBagPockets[pocketId];
    CompactItemsInBagPocket(pocketId);
    bagTab = sPocketToBagTab[pocketId];

    if (bagTab == FRLG_POCKET_COUNT)
        return;

    sBagMenuDisplay->numItemStacks[bagTab] = 0;

    for (u32 i = 0; i < pocket->capacity && BagPocket_GetSlotData(pocket, i).itemId; i++)
        sBagMenuDisplay->numItemStacks[bagTab]++;

    if (sBagMenuDisplay->numItemStacks[bagTab] + 1 > 6)
        sBagMenuDisplay->maxShowed[bagTab] = 6;
    else
        sBagMenuDisplay->maxShowed[bagTab] = sBagMenuDisplay->numItemStacks[bagTab] + 1;
}

static void UpdatePocketItemListsFrlg(void)
{
    u8 i;
    for (i = 0; i < POCKETS_COUNT; i++)
    {
        UpdatePocketItemListFrlg(i);
    }
}

void DisplayItemMessageFrlg(u8 taskId, u8 fontId, const u8 * string, TaskFunc followUpFunc)
{
    s16 *data = gTasks[taskId].data;
    tMsgWindowId = OpenBagWindow(FRLG_BAG_SUBWINDOW_MESSAGE);
    FillWindowPixelBuffer(tMsgWindowId, PIXEL_FILL(1));
    DisplayMessageAndContinueTask(taskId, tMsgWindowId, 0x06D, 0x0D, fontId, GetPlayerTextSpeedDelay(), string, followUpFunc);
    ScheduleBgCopyTilemapToVram(0);
}

void ItemMenu_SetExitCallback(MainCallback cb)
{
    sBagMenuDisplay->exitCB = cb;
}

static void Task_BagMenu_HandleInput(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    u16 scrollPosition;
    u16 cursorPos;
    s32 input;

    if (gPaletteFade.active)
        return;
    if (FuncIsActiveTask(Task_AnimateWin0v) == TRUE)
        return;
    if (MenuHelpers_ShouldWaitForLinkRecv() == TRUE)
        return;
    switch (ProcessPocketSwitchInput(taskId, gBagMenuState.bagTab))
    {
    case 1:
        SwitchPockets(taskId, -1, FALSE);
        return;
    case 2:
        SwitchPockets(taskId,  1, FALSE);
        return;
    default:
        if (JOY_NEW(SELECT_BUTTON) && gBagMenuState.location == ITEMMENULOCATION_FIELD)
        {
            ListMenuGetScrollAndRow(tListTaskId, &scrollPosition, &cursorPos);
            if (scrollPosition + cursorPos != sBagMenuDisplay->numItemStacks[gBagMenuState.bagTab])
            {
                PlaySE(SE_SELECT);
                BeginMovingItemInPocket(taskId, scrollPosition + cursorPos);
                return;
            }
        }
        break;
    }
    input = ListMenu_ProcessInput(tListTaskId);
    ListMenuGetScrollAndRow(tListTaskId, &gBagMenuState.scrollPosition[gBagMenuState.bagTab], &gBagMenuState.cursorPos[gBagMenuState.bagTab]);
    switch (input)
    {
    case LIST_NOTHING_CHOSEN:
        return;
    case LIST_CANCEL:
        PlaySE(SE_SELECT);
        gSpecialVar_ItemId = ITEM_NONE;
        Bag_BeginCloseWin0Animation();
        gTasks[taskId].func = ItemMenu_StartFadeToExitCallback;
        break;
    default:
        PlaySE(SE_SELECT);
        if (input == sBagMenuDisplay->numItemStacks[gBagMenuState.bagTab])
        {
            gSpecialVar_ItemId = ITEM_NONE;
            Bag_BeginCloseWin0Animation();
            gTasks[taskId].func = ItemMenu_StartFadeToExitCallback;
        }
        else
        {
            BagDestroyPocketScrollArrowPair();
            bag_menu_print_cursor_(tListTaskId, 2);
            tListPosition = input;
            tQuantity = GetBagItemQuantity(gBagMenuState.pocket, input);
            gSpecialVar_ItemId = GetBagItemId(gBagMenuState.pocket, input);
            gTasks[taskId].func = Task_ItemContextMenuByLocation;
        }
        break;
    }
}

static void Task_ItemContextMenuByLocation(u8 taskId)
{
    Bag_FillMessageBoxWithPalette(1);
    sItemContextTaskFuncs[gBagMenuState.location](taskId);
}

static void Task_RedrawArrowsAndReturnToBagMenuSelect(u8 taskId)
{
    Bag_FillMessageBoxWithPalette(0);
    CreatePocketScrollArrowPair();
    CreatePocketSwitchArrowPair();
    gTasks[taskId].func = Task_BagMenu_HandleInput;
}

static void Bag_FillMessageBoxWithPalette(u32 a0)
{
    SetBgTilemapPalette(1, 0, 14, 30, 6, a0 + 1);
    ScheduleBgCopyTilemapToVram(1);
}

static u8 ProcessPocketSwitchInput(u8 taskId, u8 bagTab)
{
    u8 lrState;
    if (sBagMenuDisplay->pocketSwitchMode != 0)
        return 0;
    lrState = GetLRKeysPressed();
    if (JOY_NEW(DPAD_LEFT) || lrState == MENU_L_PRESSED)
    {
        if (bagTab == FRLG_POCKET_ITEMS)
            return 0;
        PlaySE(SE_RG_BAG_POCKET);
        return 1;
    }
    if (JOY_NEW(DPAD_RIGHT) || lrState == MENU_R_PRESSED)
    {
        if (bagTab >= FRLG_POCKET_COUNT - 1)
            return 0;
        PlaySE(SE_RG_BAG_POCKET);
        return 2;
    }
    return 0;
}

static void SwitchPockets(u8 taskId, s16 direction, bool16 a2)
{
    s16 *data = gTasks[taskId].data;
    tSwitchState = 0;
    tSwitchCounter = 0;
    tSwitchDir = direction;
    if (!a2)
    {
        ClearWindowTilemap(FRLG_BAG_WINDOW_ITEM_LIST);
        ClearWindowTilemap(FRLG_BAG_WINDOW_DESCRIPTION);
        ClearWindowTilemap(FRLG_BAG_WINDOW_POCKET_NAME);
        DestroyListMenuTask(tListTaskId, &gBagMenuState.scrollPosition[gBagMenuState.bagTab], &gBagMenuState.cursorPos[gBagMenuState.bagTab]);
        ScheduleBgCopyTilemapToVram(0);
        ItemRG_EraseItemIcon(sBagMenuDisplay->itemMenuIcon ^ 1);
        BagDestroyPocketScrollArrowPair();
    }
    FillBgTilemapBufferRect_Palette0(1, 0x02D, 11, 1, 18, 12);
    ScheduleBgCopyTilemapToVram(1);
    ItemRG_SetBagVisualPocketId(gBagMenuState.bagTab + direction);
    SetTaskFuncWithFollowupFunc(taskId, Task_AnimateSwitchPockets, gTasks[taskId].func);
}

static void Task_AnimateSwitchPockets(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (!MenuHelpers_IsLinkActive() && !BagIsTutorial())
    {
        switch (ProcessPocketSwitchInput(taskId, gBagMenuState.bagTab + tSwitchDir))
        {
        case 1:
            gBagMenuState.bagTab += tSwitchDir;
            gBagMenuState.pocket = sBagTabToPocket[gBagMenuState.bagTab];
            SwitchTaskToFollowupFunc(taskId);
            SwitchPockets(taskId, -1, TRUE);
            return;
        case 2:
            gBagMenuState.bagTab += tSwitchDir;
            gBagMenuState.pocket = sBagTabToPocket[gBagMenuState.bagTab];
            SwitchTaskToFollowupFunc(taskId);
            SwitchPockets(taskId,  1, TRUE);
            return;
        }
    }
    switch (tSwitchState)
    {
    case 0:
        // Animate the item list being revealed from the bottom row up
        if (tSwitchCounter != SHRT_MAX)
        {
            tSwitchCounter++;
            DrawItemListRow(tSwitchCounter);
            if (tSwitchCounter == LIST_TILES_HEIGHT)
                tSwitchCounter = SHRT_MAX;
        }
        if (tSwitchCounter == SHRT_MAX)
            tSwitchState++;
        break;
    case 1:
        gBagMenuState.bagTab += tSwitchDir;
        gBagMenuState.pocket = sBagTabToPocket[gBagMenuState.bagTab];
        PrintBagPocketName();
        Bag_BuildListMenuTemplate(gBagMenuState.pocket, gBagMenuState.bagTab);
        tListTaskId = ListMenuInit(&gMultiuseListMenuTemplate, gBagMenuState.scrollPosition[gBagMenuState.bagTab], gBagMenuState.cursorPos[gBagMenuState.bagTab]);
        PutWindowTilemap(FRLG_BAG_WINDOW_DESCRIPTION);
        PutWindowTilemap(FRLG_BAG_WINDOW_POCKET_NAME);
        ScheduleBgCopyTilemapToVram(0);
        CreatePocketScrollArrowPair();
        CreatePocketSwitchArrowPair();
        SwitchTaskToFollowupFunc(taskId);
        break;
    }
}

static void BeginMovingItemInPocket(u8 taskId, s16 itemIndex)
{
    u16 cursorPos;
    u16 scrollPosition;
    s16 *data = gTasks[taskId].data;
    ListMenuGetScrollAndRow(tListTaskId, &scrollPosition, &cursorPos);
    ListMenuSetTemplateField(tListTaskId, LISTFIELD_CURSORKIND, 1);
    tListPosition = itemIndex;
    sBagMenuDisplay->itemOriginalLocation = itemIndex;
    StringCopy(gStringVar1, GetItemName(GetBagItemId(gBagMenuState.pocket, tListPosition)));
    StringExpandPlaceholders(gStringVar4, sText_WhereShouldTheStrVar1BePlaced);
    FillWindowPixelBuffer(FRLG_BAG_WINDOW_DESCRIPTION, PIXEL_FILL(0));
    BagPrintTextOnWindow(FRLG_BAG_WINDOW_DESCRIPTION, FONT_SHORT, gStringVar4, 0, 3, 2, 0, 0, 0);
    ItemRG_UpdateSwapLinePos(0, ListMenuGetYCoordForPrintingArrowCursor(tListTaskId));
    ItemRG_SetSwapLineInvisibility(FALSE);
    BagDestroyPocketSwitchArrowPair();
    bag_menu_print_cursor_(tListTaskId, 2);
    gTasks[taskId].func = Task_MoveItemInPocket_HandleInput;
}

static void Task_MoveItemInPocket_HandleInput(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    s32 input;
    u16 cursorPos;
    u16 scrollPosition;
    if (MenuHelpers_ShouldWaitForLinkRecv() == TRUE)
        return;
    input = ListMenu_ProcessInput(tListTaskId);
    ListMenuGetScrollAndRow(tListTaskId, &gBagMenuState.scrollPosition[gBagMenuState.bagTab], &gBagMenuState.cursorPos[gBagMenuState.bagTab]);
    ItemRG_UpdateSwapLinePos(0, ListMenuGetYCoordForPrintingArrowCursor(tListTaskId));
    if (JOY_NEW(SELECT_BUTTON))
    {
        PlaySE(SE_SELECT);
        sBagMenuDisplay->itemOriginalLocation = 0xFF;
        ListMenuGetScrollAndRow(tListTaskId, &scrollPosition, &cursorPos);
        ExecuteMoveItemInPocket(taskId, scrollPosition + cursorPos);
        return;
    }
    switch (input)
    {
    case LIST_NOTHING_CHOSEN:
        return;
    case LIST_CANCEL:
        PlaySE(SE_SELECT);
        sBagMenuDisplay->itemOriginalLocation = 0xFF;
        ListMenuGetScrollAndRow(tListTaskId, &scrollPosition, &cursorPos);
        AbortMovingItemInPocket(taskId, scrollPosition + cursorPos);
        break;
    default:
        PlaySE(SE_SELECT);
        sBagMenuDisplay->itemOriginalLocation = 0xFF;
        ExecuteMoveItemInPocket(taskId, input);
        break;
    }
}

static void ExecuteMoveItemInPocket(u8 taskId, u32 itemIndex)
{
    s16 *data = gTasks[taskId].data;
    if (tListPosition == itemIndex || tListPosition == itemIndex - 1)
    {
        AbortMovingItemInPocket(taskId, itemIndex);
    }
    else
    {
        MoveItemSlotInPocket(gBagMenuState.pocket, tListPosition, gBagMenuState.scrollPosition[gBagMenuState.bagTab]);
        DestroyListMenuTask(tListTaskId, &gBagMenuState.scrollPosition[gBagMenuState.bagTab], &gBagMenuState.cursorPos[gBagMenuState.bagTab]);
        if (tListPosition < itemIndex)
            gBagMenuState.cursorPos[gBagMenuState.bagTab]--;
        Bag_BuildListMenuTemplate(gBagMenuState.pocket, gBagMenuState.bagTab);
        tListTaskId = ListMenuInit(&gMultiuseListMenuTemplate, gBagMenuState.scrollPosition[gBagMenuState.bagTab], gBagMenuState.cursorPos[gBagMenuState.bagTab]);
        ItemRG_SetSwapLineInvisibility(TRUE);
        CreatePocketSwitchArrowPair();
        gTasks[taskId].func = Task_BagMenu_HandleInput;
    }
}

static void AbortMovingItemInPocket(u8 taskId, u32 itemIndex)
{
    s16 *data = gTasks[taskId].data;
    DestroyListMenuTask(tListTaskId, &gBagMenuState.scrollPosition[gBagMenuState.bagTab], &gBagMenuState.cursorPos[gBagMenuState.bagTab]);
    if (tListPosition < itemIndex)
        gBagMenuState.cursorPos[gBagMenuState.bagTab]--;
    Bag_BuildListMenuTemplate(gBagMenuState.pocket, gBagMenuState.bagTab);
    tListTaskId = ListMenuInit(&gMultiuseListMenuTemplate, gBagMenuState.scrollPosition[gBagMenuState.bagTab], gBagMenuState.cursorPos[gBagMenuState.bagTab]);
    ItemRG_SetSwapLineInvisibility(TRUE);
    CreatePocketSwitchArrowPair();
    gTasks[taskId].func = Task_BagMenu_HandleInput;
}

static void InitQuantityToTossOrDeposit(u16 scrollPosition, const u8 *str)
{
    u8 r4;
    u8 r5 = ShowBagWindow(FRLG_BAG_SUBWINDOW_7, 2);
    CopyItemNameHandlePlural(GetBagItemId(gBagMenuState.pocket, scrollPosition), gStringVar1, 2);
    StringExpandPlaceholders(gStringVar4, str);
    BagPrintTextOnWindow(r5, FONT_SHORT, gStringVar4, 0, 2, 1, 0, 0, 1);
    r4 = ShowBagWindow(FRLG_BAG_SUBWINDOW_1, 0);
    ConvertIntToDecimalStringN(gStringVar1, 1, STR_CONV_MODE_LEADING_ZEROS, 3);
    StringExpandPlaceholders(gStringVar4, gText_xVar1);
    BagPrintTextOnWindow(r4, FONT_SMALL, gStringVar4, 4, 10, 1, 0, 0, 1);
    CreateArrowPair_QuantitySelect();
}

static void UpdateQuantityToTossOrDeposit(s16 value, u8 ndigits)
{
    u8 r6 = GetBagWindow(FRLG_BAG_SUBWINDOW_1);
    FillWindowPixelBuffer(r6, PIXEL_FILL(1));
    ConvertIntToDecimalStringN(gStringVar1, value, STR_CONV_MODE_LEADING_ZEROS, ndigits);
    StringExpandPlaceholders(gStringVar4, gText_xVar1);
    BagPrintTextOnWindow(r6, FONT_SMALL, gStringVar4, 4, 10, 1, 0, 0, 1);
}

// row of 0 is the bottom row in the list, up to LIST_TILES_HEIGHT at the top
static void DrawItemListRow(u8 row)
{
    CopyToBgTilemapBufferRect(1, &sItemListTilemap[(LIST_TILES_HEIGHT - row) * LIST_TILES_WIDTH], 11, 1 + LIST_TILES_HEIGHT - row, LIST_TILES_WIDTH, 1);
    ScheduleBgCopyTilemapToVram(1);
}

static void OpenContextMenu(u8 taskId)
{
    u8 r6;
    u8 r4;
    switch (gBagMenuState.location)
    {
    case ITEMMENULOCATION_BATTLE:
        if (gSpecialVar_ItemId == ITEM_BERRY_POUCH)
        {
            sContextMenuItemsBuffer[0] = ITEMMENUACTION_OPEN_BERRIES;
            sContextMenuItemsBuffer[1] = ITEMMENUACTION_CANCEL;
            sContextMenuItemsPtr = sContextMenuItemsBuffer;
            sContextMenuNumItems = 2;
        }
        else if (GetItemBattleUsage(gSpecialVar_ItemId))
        {
            sContextMenuItemsPtr = sContextMenuItems_BattleUse;
            sContextMenuNumItems = 2;
        }
        else
        {
            sContextMenuItemsPtr = sContextMenuItems_Cancel;
            sContextMenuNumItems = 1;
        }
        break;
    case ITEMMENULOCATION_WALLY:
        sContextMenuItemsPtr = sContextMenuItems_BattleUse;
        sContextMenuNumItems = 2;
        break;
    default:
        if (MenuHelpers_IsLinkActive() == TRUE || InUnionRoom() == TRUE)
        {
            if (gSpecialVar_ItemId == ITEM_TM_CASE || gSpecialVar_ItemId == ITEM_BERRY_POUCH)
            {
                sContextMenuItemsPtr = sContextMenuItems_Open;
                sContextMenuNumItems = 2;
            }
            else
            {
                if (gBagMenuState.pocket == POCKET_KEY_ITEMS)
                    sContextMenuNumItems = 1;
                else
                    sContextMenuNumItems = 2;
                sContextMenuItemsPtr = sContextMenuItems_GiveIfNotKeyItemPocket[gBagMenuState.bagTab];
            }
        }
        else
        {
            switch (gBagMenuState.pocket)
            {
            default:
            case POCKET_ITEMS:
                sContextMenuNumItems = 4;
                if (ItemIsMail(gSpecialVar_ItemId) == TRUE)
                    sContextMenuItemsPtr = sContextMenuItems_CheckGiveTossCancel;
                else
                    sContextMenuItemsPtr = sContextMenuItems_Field[FRLG_POCKET_ITEMS];
                break;
            case POCKET_KEY_ITEMS:
                sContextMenuItemsPtr = sContextMenuItemsBuffer;
                sContextMenuNumItems = 3;
                sContextMenuItemsBuffer[2] = ITEMMENUACTION_CANCEL;
                if (gSaveBlock1Ptr->registeredItem == gSpecialVar_ItemId)
                    sContextMenuItemsBuffer[1] = ITEMMENUACTION_DESELECT;
                else
                    sContextMenuItemsBuffer[1] = ITEMMENUACTION_REGISTER;
                if (gSpecialVar_ItemId == ITEM_TM_CASE || gSpecialVar_ItemId == ITEM_BERRY_POUCH)
                    sContextMenuItemsBuffer[0] = ITEMMENUACTION_OPEN;
                else if (gSpecialVar_ItemId == ITEM_BICYCLE && TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_ACRO_BIKE | PLAYER_AVATAR_FLAG_MACH_BIKE))
                    sContextMenuItemsBuffer[0] = ITEMMENUACTION_WALK;
                else
                    sContextMenuItemsBuffer[0] = ITEMMENUACTION_USE;
                break;
            case POCKET_POKE_BALLS:
                sContextMenuItemsPtr = sContextMenuItems_Field[FRLG_POCKET_POKEBALLS];
                sContextMenuNumItems = 3;
                break;
            }
        }
    }
    r6 = ShowBagWindow(FRLG_BAG_SUBWINDOW_11, sContextMenuNumItems - 1);
    PrintMenuActionTexts(
        r6,
        FONT_SHORT,
        GetMenuCursorDimensionByFont(FONT_SHORT, 0),
        2,
        GetFontAttribute(FONT_SHORT, FONTATTR_LETTER_SPACING),
        GetFontAttribute(FONT_SHORT, FONTATTR_MAX_LETTER_HEIGHT) + 2,
        sContextMenuNumItems,
        sItemMenuContextActions,
        sContextMenuItemsPtr
    );
    InitMenuNormal(r6, FONT_SHORT, 0, 2, GetFontAttribute(FONT_SHORT, FONTATTR_MAX_LETTER_HEIGHT) + 2, sContextMenuNumItems, 0);
    r4 = ShowBagWindow(FRLG_BAG_SUBWINDOW_7, 0);
    CopyItemName(gSpecialVar_ItemId, gStringVar1);
    StringExpandPlaceholders(gStringVar4, gText_Var1IsSelected);
    BagPrintTextOnWindow(r4, FONT_SHORT, gStringVar4, 0, 2, 1, 0, 0, 1);
}

static void Task_ItemContext_FieldOrBattle(u8 taskId)
{
    OpenContextMenu(taskId);
    gTasks[taskId].func = Task_FieldItemContextMenuHandleInput;
}

static void Task_FieldItemContextMenuHandleInput(u8 taskId)
{
    s8 input;
    if (MenuHelpers_ShouldWaitForLinkRecv() != TRUE)
    {
        input = Menu_ProcessInputNoWrap();
        switch (input)
        {
        case MENU_B_PRESSED:
            PlaySE(SE_SELECT);
            sItemMenuContextActions[ITEMMENUACTION_CANCEL].func.void_u8(taskId);
            break;
        case MENU_NOTHING_CHOSEN:
            break;
        default:
            PlaySE(SE_SELECT);
            sItemMenuContextActions[sContextMenuItemsPtr[input]].func.void_u8(taskId);
            break;
        }
    }
}

static void Task_ItemMenuAction_Use(u8 taskId)
{
    if (GetItemFieldFunc(gSpecialVar_ItemId) != NULL)
    {
        HideBagWindow(FRLG_BAG_SUBWINDOW_11);
        HideBagWindow(FRLG_BAG_SUBWINDOW_7);
        PutWindowTilemap(FRLG_BAG_WINDOW_ITEM_LIST);
        PutWindowTilemap(FRLG_BAG_WINDOW_DESCRIPTION);
        ScheduleBgCopyTilemapToVram(0);
        if (CalculatePlayerPartyCount() == 0 && GetItemType(gSpecialVar_ItemId) == ITEM_USE_PARTY_MENU)
            Task_PrintThereIsNoPokemon(taskId);
        else
            GetItemFieldFunc(gSpecialVar_ItemId)(taskId);
    }
}

static void Task_ItemMenuAction_Toss(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    ClearWindowTilemap(GetBagWindow(FRLG_BAG_SUBWINDOW_11));
    ClearWindowTilemap(GetBagWindow(FRLG_BAG_SUBWINDOW_7));
    HideBagWindow(FRLG_BAG_SUBWINDOW_11);
    HideBagWindow(FRLG_BAG_SUBWINDOW_7);
    PutWindowTilemap(FRLG_BAG_WINDOW_ITEM_LIST);
    tItemCount = 1;
    if (tQuantity == 1)
    {
        Task_ConfirmTossItems(taskId);
    }
    else
    {
        InitQuantityToTossOrDeposit(tListPosition, sText_TossOutHowManyStrVar1s);
        gTasks[taskId].func = Task_SelectQuantityToToss;
    }
}

static void Task_ConfirmTossItems(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    ConvertIntToDecimalStringN(gStringVar2, tItemCount, STR_CONV_MODE_LEFT_ALIGN, 3);
    StringExpandPlaceholders(gStringVar4, sText_ThrowAwayStrVar2OfThisItemQM);
    BagPrintTextOnWindow(ShowBagWindow(FRLG_BAG_SUBWINDOW_7, 1), FONT_SHORT, gStringVar4, 0, 2, 1, 0, 0, 1);
    BagCreateYesNoMenuBottomRight(taskId, &sYesNoMenu_Toss);
}

static void Task_TossItem_No(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    HideBagWindow(FRLG_BAG_SUBWINDOW_7);
    PutWindowTilemap(FRLG_BAG_WINDOW_DESCRIPTION);
    ScheduleBgCopyTilemapToVram(0);
    bag_menu_print_cursor_(tListTaskId, 1);
    Task_RedrawArrowsAndReturnToBagMenuSelect(taskId);
}

static void Task_SelectQuantityToToss(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (AdjustQuantityAccordingToDPadInput(&tItemCount, tQuantity) == TRUE)
    {
        UpdateQuantityToTossOrDeposit(tItemCount, 3);
    }
    else if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        ClearWindowTilemap(GetBagWindow(FRLG_BAG_SUBWINDOW_7));
        HideBagWindow(FRLG_BAG_SUBWINDOW_7);
        HideBagWindow(FRLG_BAG_SUBWINDOW_1);
        ScheduleBgCopyTilemapToVram(0);
        BagDestroyPocketScrollArrowPair();
        Task_ConfirmTossItems(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        HideBagWindow(FRLG_BAG_SUBWINDOW_7);
        HideBagWindow(FRLG_BAG_SUBWINDOW_1);
        PutWindowTilemap(FRLG_BAG_WINDOW_ITEM_LIST);
        PutWindowTilemap(FRLG_BAG_WINDOW_DESCRIPTION);
        ScheduleBgCopyTilemapToVram(0);
        bag_menu_print_cursor_(tListTaskId, 1);
        BagDestroyPocketScrollArrowPair();
        Task_RedrawArrowsAndReturnToBagMenuSelect(taskId);
    }
}

static void Task_TossItem_Yes(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    HideBagWindow(FRLG_BAG_SUBWINDOW_7);
    CopyItemName(GetBagItemId(gBagMenuState.pocket, tListPosition), gStringVar1);
    ConvertIntToDecimalStringN(gStringVar2, tItemCount, STR_CONV_MODE_LEFT_ALIGN, 3);
    StringExpandPlaceholders(gStringVar4, gText_ThrewAwayVar2Var1s);
    BagPrintTextOnWindow(ShowBagWindow(FRLG_BAG_SUBWINDOW_7, 3), FONT_SHORT, gStringVar4, 0, 2, 1, 0, 0, 1);
    gTasks[taskId].func = Task_WaitAB_RedrawAndReturnToBag;
}

static void Task_WaitAB_RedrawAndReturnToBag(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        RemoveBagItem(gSpecialVar_ItemId, tItemCount);
        HideBagWindow(FRLG_BAG_SUBWINDOW_7);
        DestroyListMenuTask(tListTaskId, &gBagMenuState.scrollPosition[gBagMenuState.bagTab], &gBagMenuState.cursorPos[gBagMenuState.bagTab]);
        UpdatePocketItemListFrlg(gBagMenuState.pocket);
        UpdatePocketListPositionFrlg(gBagMenuState.pocket);
        Bag_BuildListMenuTemplate(gBagMenuState.pocket, gBagMenuState.bagTab);
        tListTaskId = ListMenuInit(&gMultiuseListMenuTemplate, gBagMenuState.scrollPosition[gBagMenuState.bagTab], gBagMenuState.cursorPos[gBagMenuState.bagTab]);
        PutWindowTilemap(FRLG_BAG_WINDOW_DESCRIPTION);
        ScheduleBgCopyTilemapToVram(0);
        bag_menu_print_cursor_(tListTaskId, 1);
        Task_RedrawArrowsAndReturnToBagMenuSelect(taskId);
    }
}

static void Task_ItemMenuAction_ToggleSelect(u8 taskId)
{
    u16 itemId;
    s16 *data = gTasks[taskId].data;
    itemId = GetBagItemId(gBagMenuState.pocket, tListPosition);
    if (gSaveBlock1Ptr->registeredItem == itemId)
        gSaveBlock1Ptr->registeredItem = ITEM_NONE;
    else
        gSaveBlock1Ptr->registeredItem = itemId;

    DestroyListMenuTask(tListTaskId, &gBagMenuState.scrollPosition[gBagMenuState.bagTab], &gBagMenuState.cursorPos[gBagMenuState.bagTab]);
    Bag_BuildListMenuTemplate(gBagMenuState.pocket, gBagMenuState.bagTab);
    tListTaskId = ListMenuInit(&gMultiuseListMenuTemplate, gBagMenuState.scrollPosition[gBagMenuState.bagTab], gBagMenuState.cursorPos[gBagMenuState.bagTab]);
    CopyWindowToVram(FRLG_BAG_WINDOW_ITEM_LIST, COPYWIN_MAP);
    Task_ItemMenuAction_Cancel(taskId);
}

static void Task_ItemMenuAction_Give(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    u16 itemId = GetBagItemId(gBagMenuState.pocket, tListPosition);
    HideBagWindow(FRLG_BAG_SUBWINDOW_11);
    HideBagWindow(FRLG_BAG_SUBWINDOW_7);
    PutWindowTilemap(FRLG_BAG_WINDOW_ITEM_LIST);
    PutWindowTilemap(FRLG_BAG_WINDOW_DESCRIPTION);
    CopyWindowToVram(FRLG_BAG_WINDOW_ITEM_LIST, COPYWIN_MAP);
    if (!IsWritingMailAllowed(itemId))
        DisplayItemMessageFrlg(taskId, FONT_SHORT, gText_CantWriteMail, Task_WaitAButtonAndCloseContextMenu);
    else if (!GetItemImportance(itemId))
    {
        if (!CalculatePlayerPartyCount())
        {
            Task_PrintThereIsNoPokemon(taskId);
        }
        else
        {
            sBagMenuDisplay->exitCB = CB2_ChooseMonToGiveItem;
            gTasks[taskId].func = ItemMenu_StartFadeToExitCallback;
        }
    }
    else
        Task_PrintItemCantBeHeld(taskId);
}

static void Task_PrintThereIsNoPokemon(u8 taskId)
{
    DisplayItemMessageFrlg(taskId, FONT_SHORT, gText_NoPokemon, Task_WaitAButtonAndCloseContextMenu);
}

static void Task_PrintItemCantBeHeld(u8 taskId)
{
    CopyItemName(gSpecialVar_ItemId, gStringVar1);
    StringExpandPlaceholders(gStringVar4, gText_Var1CantBeHeld);
    DisplayItemMessageFrlg(taskId, FONT_SHORT, gStringVar4, Task_WaitAButtonAndCloseContextMenu);
}

static void Task_WaitAButtonAndCloseContextMenu(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        Task_ReturnToBagFromContextMenu(taskId);
    }
}

void Task_ReturnToBagFromContextMenu(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    CloseBagWindow(FRLG_BAG_SUBWINDOW_MESSAGE);
    DestroyListMenuTask(tListTaskId, &gBagMenuState.scrollPosition[gBagMenuState.bagTab], &gBagMenuState.cursorPos[gBagMenuState.bagTab]);
    UpdatePocketItemListFrlg(gBagMenuState.pocket);
    UpdatePocketListPositionFrlg(gBagMenuState.pocket);
    Bag_BuildListMenuTemplate(gBagMenuState.pocket, gBagMenuState.bagTab);
    tListTaskId = ListMenuInit(&gMultiuseListMenuTemplate, gBagMenuState.scrollPosition[gBagMenuState.bagTab], gBagMenuState.cursorPos[gBagMenuState.bagTab]);
    ScheduleBgCopyTilemapToVram(0);
    bag_menu_print_cursor_(tListTaskId, 1);
    Task_RedrawArrowsAndReturnToBagMenuSelect(taskId);
}

static void Task_ItemMenuAction_Cancel(u8 taskId)
{
    HideBagWindow(FRLG_BAG_SUBWINDOW_11);
    HideBagWindow(FRLG_BAG_SUBWINDOW_7);
    PutWindowTilemap(FRLG_BAG_WINDOW_ITEM_LIST);
    PutWindowTilemap(FRLG_BAG_WINDOW_DESCRIPTION);
    ScheduleBgCopyTilemapToVram(0);
    bag_menu_print_cursor_(gTasks[taskId].tListTaskId, 1);
    Task_RedrawArrowsAndReturnToBagMenuSelect(taskId);
}

static void InitBerryPouchFromBattle(void)
{
    InitBerryPouch(BERRYPOUCH_BATTLE, CB2_BagMenuFromBattle, FALSE);
}

static void Task_ItemMenuAction_BattleUse(u8 taskId)
{
    if (gSpecialVar_ItemId == ITEM_BERRY_POUCH)
    {
        HideBagWindow(FRLG_BAG_SUBWINDOW_11);
        HideBagWindow(FRLG_BAG_SUBWINDOW_7);
        PutWindowTilemap(FRLG_BAG_WINDOW_ITEM_LIST);
        PutWindowTilemap(FRLG_BAG_WINDOW_DESCRIPTION);
        CopyWindowToVram(FRLG_BAG_WINDOW_ITEM_LIST, COPYWIN_MAP);
        ItemMenu_SetExitCallback(InitBerryPouchFromBattle);
        ItemMenu_StartFadeToExitCallback(taskId);
    }

    // Safety check
    enum ItemType type = GetItemType(gSpecialVar_ItemId);
    if (!GetItemBattleUsage(gSpecialVar_ItemId))
        return;

    HideBagWindow(FRLG_BAG_SUBWINDOW_11);
    HideBagWindow(FRLG_BAG_SUBWINDOW_7);
    PutWindowTilemap(FRLG_BAG_WINDOW_ITEM_LIST);
    PutWindowTilemap(FRLG_BAG_WINDOW_DESCRIPTION);
    CopyWindowToVram(FRLG_BAG_WINDOW_ITEM_LIST, COPYWIN_MAP);
    if (type == ITEM_USE_BAG_MENU)
        ItemUseInBattle_BagMenu(taskId);
    else if (type == ITEM_USE_PARTY_MENU)
        ItemUseInBattle_PartyMenu(taskId);
    else if (type == ITEM_USE_PARTY_MENU_MOVES)
        ItemUseInBattle_PartyMenuChooseMove(taskId);
}

static void Task_ItemContext_FieldGive(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    u16 itemId = GetBagItemId(gBagMenuState.pocket, tListPosition);
    if (!IsWritingMailAllowed(itemId))
    {
        DisplayItemMessageFrlg(taskId, FONT_SHORT, gText_CantWriteMail, Task_WaitAButtonAndCloseContextMenu);
    }
    else if (itemId == ITEM_TM_CASE)
    {
        ItemMenu_SetExitCallback(GoToTMCase_Give);
        ItemMenu_StartFadeToExitCallback(taskId);
    }
    else if (itemId == ITEM_BERRY_POUCH)
    {
        ItemMenu_SetExitCallback(GoToBerryPouch_Give);
        ItemMenu_StartFadeToExitCallback(taskId);
    }
    else if (gBagMenuState.pocket != POCKET_KEY_ITEMS && !GetItemImportance(itemId))
    {
        Bag_BeginCloseWin0Animation();
        gTasks[taskId].func = ItemMenu_StartFadeToExitCallback;
    }
    else
    {
        Task_PrintItemCantBeHeld(taskId);
    }
}

static void GoToTMCase_Give(void)
{
    InitTMCase(TMCASE_GIVE_PARTY, ReturnToBagMenuFromSubmenu_Give, FALSE);
}

static void GoToBerryPouch_Give(void)
{
    InitBerryPouch(BERRYPOUCH_GIVE_PARTY, ReturnToBagMenuFromSubmenu_Give, FALSE);
}

static void ReturnToBagMenuFromSubmenu_Give(void)
{
    CB2_SelectBagItemToGive();
}

static void Task_ItemContext_PcBoxGive(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    u16 itemId = GetBagItemId(gBagMenuState.pocket, tListPosition);
    if (ItemIsMail(itemId))
    {
        DisplayItemMessageFrlg(taskId, FONT_SHORT, gText_CantWriteMail, Task_WaitAButtonAndCloseContextMenu);
    }
    else if (itemId == ITEM_TM_CASE)
    {
        ItemMenu_SetExitCallback(GoToTMCase_PCBox);
        ItemMenu_StartFadeToExitCallback(taskId);
    }
    else if (itemId == ITEM_BERRY_POUCH)
    {
        ItemMenu_SetExitCallback(GoToBerryPouch_PCBox);
        ItemMenu_StartFadeToExitCallback(taskId);
    }
    else if (gBagMenuState.pocket != POCKET_KEY_ITEMS && !GetItemImportance(itemId))
    {
        Bag_BeginCloseWin0Animation();
        gTasks[taskId].func = ItemMenu_StartFadeToExitCallback;
    }
    else
    {
        Task_PrintItemCantBeHeld(taskId);
    }
}

static void GoToTMCase_PCBox(void)
{
    InitTMCase(TMCASE_GIVE_PC, ReturnToBagMenuFromSubmenu_PCBox, FALSE);
}

static void GoToBerryPouch_PCBox(void)
{
    InitBerryPouch(BERRYPOUCH_GIVE_PC, ReturnToBagMenuFromSubmenu_PCBox, FALSE);
}

static void ReturnToBagMenuFromSubmenu_PCBox(void)
{
    GoToBagMenuFrlg(ITEMMENULOCATION_PCBOX, OPEN_BAG_LAST, CB2_ReturnToPokeStorage);
}

static void Task_ItemContext_Sell(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (gSpecialVar_ItemId == ITEM_TM_CASE)
    {
        ItemMenu_SetExitCallback(GoToTMCase_Sell);
        ItemMenu_StartFadeToExitCallback(taskId);
    }
    else if (gSpecialVar_ItemId == ITEM_BERRY_POUCH)
    {
        ItemMenu_SetExitCallback(GoToBerryPouch_Sell);
        ItemMenu_StartFadeToExitCallback(taskId);
    }
    else if (GetItemPrice(gSpecialVar_ItemId) == 0)
    {
        CopyItemName(gSpecialVar_ItemId, gStringVar1);
        StringExpandPlaceholders(gStringVar4, gText_CantBuyKeyItem);
        DisplayItemMessageFrlg(taskId, FONT_SHORT, gStringVar4, Task_ReturnToBagFromContextMenu);
    }
    else
    {
        tItemCount = 1;
        if (tQuantity == 1)
        {
            BagPrintMoneyAmount();
            Task_PrintSaleConfirmationText(taskId);
        }
        else
        {
            if (tQuantity > MAX_BAG_ITEM_CAPACITY)
                tQuantity = MAX_BAG_ITEM_CAPACITY;
            CopyItemName(gSpecialVar_ItemId, gStringVar1);
            StringExpandPlaceholders(gStringVar4, gText_HowManyToSell);
            DisplayItemMessageFrlg(taskId, FONT_SHORT, gStringVar4, Task_InitSaleQuantitySelectInterface);
        }
    }
}

static void GoToTMCase_Sell(void)
{
    InitTMCase(TMCASE_SELL, ReturnToBagMenuFromSubmenu_Sell, FALSE);
}

static void GoToBerryPouch_Sell(void)
{
    InitBerryPouch(BERRYPOUCH_SELL, ReturnToBagMenuFromSubmenu_Sell, FALSE);
}

static void ReturnToBagMenuFromSubmenu_Sell(void)
{
    GoToBagMenuFrlg(ITEMMENULOCATION_SHOP, OPEN_BAG_LAST, CB2_ReturnToField);
}

static void Task_PrintSaleConfirmationText(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    ConvertIntToDecimalStringN(gStringVar3, GetItemSellPrice(GetBagItemId(gBagMenuState.pocket, tListPosition)) * tItemCount, STR_CONV_MODE_LEFT_ALIGN, 6);
    StringExpandPlaceholders(gStringVar4, gText_ICanPayVar1);
    DisplayItemMessageFrlg(taskId, FONT_SHORT, gStringVar4, Task_ShowSellYesNoMenu);
}

static void Task_ShowSellYesNoMenu(u8 taskId)
{
    BagCreateYesNoMenuTopRight(taskId, &sYesNoMenu_Sell);
}

static void Task_SellItem_No(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    HideBagWindow(FRLG_BAG_SUBWINDOW_MONEY);
    CloseBagWindow(FRLG_BAG_SUBWINDOW_MESSAGE);
    PutWindowTilemap(FRLG_BAG_WINDOW_POCKET_NAME);
    PutWindowTilemap(FRLG_BAG_WINDOW_ITEM_LIST);
    PutWindowTilemap(FRLG_BAG_WINDOW_DESCRIPTION);
    ScheduleBgCopyTilemapToVram(0);
    bag_menu_print_cursor_(tListTaskId, 1);
    Task_RedrawArrowsAndReturnToBagMenuSelect(taskId);
}

static void Task_InitSaleQuantitySelectInterface(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    u8 r4 = ShowBagWindow(FRLG_BAG_SUBWINDOW_1, 1);
    ConvertIntToDecimalStringN(gStringVar1, 1, STR_CONV_MODE_LEADING_ZEROS, MAX_ITEM_DIGITS);
    StringExpandPlaceholders(gStringVar4, gText_xVar1);
    BagPrintTextOnWindow(r4, FONT_SMALL, gStringVar4, 4, 10, 1, 0, TEXT_SKIP_DRAW, 1);
    UpdateSalePriceDisplay(GetItemSellPrice(GetBagItemId(gBagMenuState.pocket, tListPosition)) * tItemCount);
    BagPrintMoneyAmount();
    CreatePocketScrollArrowPair_SellQuantity();
    gTasks[taskId].func = Task_SelectQuantityToSell;
}

static void UpdateSalePriceDisplay(s32 amount)
{
    PrintMoneyAmount(GetBagWindow(FRLG_BAG_SUBWINDOW_1), 56, 10, amount, 0);
}

static void Task_SelectQuantityToSell(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (AdjustQuantityAccordingToDPadInput(&tItemCount, tQuantity) == TRUE)
    {
        UpdateQuantityToTossOrDeposit(tItemCount, 2);
        UpdateSalePriceDisplay(GetItemSellPrice(GetBagItemId(gBagMenuState.pocket, tListPosition)) * tItemCount);
    }
    else if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        HideBagWindow(FRLG_BAG_SUBWINDOW_1);
        PutWindowTilemap(0);
        ScheduleBgCopyTilemapToVram(0);
        BagDestroyPocketScrollArrowPair();
        Task_PrintSaleConfirmationText(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        HideBagWindow(FRLG_BAG_SUBWINDOW_1);
        HideBagWindow(FRLG_BAG_SUBWINDOW_MONEY);
        CloseBagWindow(FRLG_BAG_SUBWINDOW_MESSAGE);
        PutWindowTilemap(FRLG_BAG_WINDOW_POCKET_NAME);
        PutWindowTilemap(FRLG_BAG_WINDOW_ITEM_LIST);
        PutWindowTilemap(FRLG_BAG_WINDOW_DESCRIPTION);
        ScheduleBgCopyTilemapToVram(0);
        BagDestroyPocketScrollArrowPair();
        bag_menu_print_cursor_(tListTaskId, 1);
        Task_RedrawArrowsAndReturnToBagMenuSelect(taskId);
    }
}

static void Task_SellItem_Yes(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    PutWindowTilemap(FRLG_BAG_WINDOW_ITEM_LIST);
    ScheduleBgCopyTilemapToVram(0);
    CopyItemName(gSpecialVar_ItemId, gStringVar2);
    ConvertIntToDecimalStringN(gStringVar1, GetItemSellPrice(GetBagItemId(gBagMenuState.pocket, tListPosition)) * tItemCount, STR_CONV_MODE_LEFT_ALIGN, 6);
    StringExpandPlaceholders(gStringVar4, gText_TurnedOverVar1ForVar2);
    DisplayItemMessageFrlg(taskId, FONT_SHORT, gStringVar4, Task_FinalizeSaleToShop);
}

static void Task_FinalizeSaleToShop(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    PlaySE(SE_SHOP);
    RemoveBagItem(gSpecialVar_ItemId, tItemCount);
    AddMoney(&gSaveBlock1Ptr->money, GetItemSellPrice(gSpecialVar_ItemId) * tItemCount);
    DestroyListMenuTask(tListTaskId, &gBagMenuState.scrollPosition[gBagMenuState.bagTab], &gBagMenuState.cursorPos[gBagMenuState.bagTab]);
    UpdatePocketItemListFrlg(gBagMenuState.pocket);
    UpdatePocketListPositionFrlg(gBagMenuState.pocket);
    sBagMenuDisplay->inhibitItemDescriptionPrint = TRUE;
    Bag_BuildListMenuTemplate(gBagMenuState.pocket, gBagMenuState.bagTab);
    tListTaskId = ListMenuInit(&gMultiuseListMenuTemplate, gBagMenuState.scrollPosition[gBagMenuState.bagTab], gBagMenuState.cursorPos[gBagMenuState.bagTab]);
    bag_menu_print_cursor_(tListTaskId, 2);
    BagDrawTextBoxOnWindow(GetBagWindow(FRLG_BAG_SUBWINDOW_MONEY));
    PrintMoneyAmountInMoneyBox(GetBagWindow(FRLG_BAG_SUBWINDOW_MONEY), GetMoney(&gSaveBlock1Ptr->money), 0);
    gTasks[taskId].func = Task_WaitPressAB_AfterSell;
}

static void Task_WaitPressAB_AfterSell(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        HideBagWindow(FRLG_BAG_SUBWINDOW_MONEY);
        PutWindowTilemap(FRLG_BAG_WINDOW_POCKET_NAME);
        sBagMenuDisplay->inhibitItemDescriptionPrint = FALSE;
        Task_ReturnToBagFromContextMenu(taskId);
    }
}

static void Task_ItemContext_Deposit(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    tItemCount = 1;
    if (tQuantity == 1)
    {
        Task_TryDoItemDeposit(taskId);
    }
    else
    {
        InitQuantityToTossOrDeposit(tListPosition, sText_DepositHowManyVar1);
        gTasks[taskId].func = Task_SelectQuantityToDeposit;
    }
}

static void Task_SelectQuantityToDeposit(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (AdjustQuantityAccordingToDPadInput(&tItemCount, tQuantity) == TRUE)
    {
        UpdateQuantityToTossOrDeposit(tItemCount, 3);
    }
    else if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        ClearWindowTilemap(GetBagWindow(FRLG_BAG_SUBWINDOW_7));
        HideBagWindow(FRLG_BAG_SUBWINDOW_7);
        HideBagWindow(FRLG_BAG_SUBWINDOW_1);
        ScheduleBgCopyTilemapToVram(0);
        BagDestroyPocketScrollArrowPair();
        Task_TryDoItemDeposit(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        HideBagWindow(FRLG_BAG_SUBWINDOW_7);
        HideBagWindow(FRLG_BAG_SUBWINDOW_1);
        PutWindowTilemap(FRLG_BAG_WINDOW_DESCRIPTION);
        ScheduleBgCopyTilemapToVram(0);
        bag_menu_print_cursor_(tListTaskId, 1);
        BagDestroyPocketScrollArrowPair();
        Task_RedrawArrowsAndReturnToBagMenuSelect(taskId);
    }
}

static void Task_TryDoItemDeposit(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (AddPCItem(gSpecialVar_ItemId, tItemCount) == TRUE)
    {
        CopyItemNameHandlePlural(gSpecialVar_ItemId, gStringVar1, tItemCount);
        ConvertIntToDecimalStringN(gStringVar2, tItemCount, STR_CONV_MODE_LEFT_ALIGN, 3);
        StringExpandPlaceholders(gStringVar4, sText_DepositedVar2Var1s);
        BagPrintTextOnWindow(ShowBagWindow(FRLG_BAG_SUBWINDOW_7, 3), FONT_SHORT, gStringVar4, 0, 2, 1, 0, 0, 1);
        gTasks[taskId].func = Task_WaitAB_RedrawAndReturnToBag;
    }
    else
    {
        DisplayItemMessageFrlg(taskId, FONT_SHORT, sText_NoRoomForItems, Task_WaitAButtonAndCloseContextMenu);
    }
}

static bool8 BagIsTutorial(void)
{
    if (gBagMenuState.location == ITEMMENULOCATION_WALLY)
        return TRUE;
    return FALSE;
}

static void BackUpPlayerBag(void)
{
    u32 i;
    sBackupPlayerBag = AllocZeroed(sizeof(struct BagSlots));
    memcpy(sBackupPlayerBag->bagPocket_Items, gSaveBlock1Ptr->bag.items, BAG_ITEMS_COUNT * sizeof(struct ItemSlot));
    memcpy(sBackupPlayerBag->bagPocket_KeyItems, gSaveBlock1Ptr->bag.keyItems, BAG_KEYITEMS_COUNT * sizeof(struct ItemSlot));
    memcpy(sBackupPlayerBag->bagPocket_PokeBalls, gSaveBlock1Ptr->bag.pokeBalls, BAG_POKEBALLS_COUNT * sizeof(struct ItemSlot));
    sBackupPlayerBag->registeredItem = gSaveBlock1Ptr->registeredItem;
    sBackupPlayerBag->pocket = gBagMenuState.bagTab;
    for (i = 0; i < FRLG_POCKET_COUNT; i++)
    {
        sBackupPlayerBag->cursorPos[i] = gBagMenuState.cursorPos[i];
        sBackupPlayerBag->scrollPosition[i] = gBagMenuState.scrollPosition[i];
    }
    memset(gSaveBlock1Ptr->bag.items, 0, sizeof(gSaveBlock1Ptr->bag.items));
    memset(gSaveBlock1Ptr->bag.keyItems, 0, sizeof(gSaveBlock1Ptr->bag.keyItems));
    memset(gSaveBlock1Ptr->bag.pokeBalls, 0, sizeof(gSaveBlock1Ptr->bag.pokeBalls));
    gSaveBlock1Ptr->registeredItem = ITEM_NONE;
    ResetBagCursorPositions();
}

static void RestorePlayerBag(void)
{
    u32 i;
    memcpy(gSaveBlock1Ptr->bag.items, sBackupPlayerBag->bagPocket_Items, BAG_ITEMS_COUNT * sizeof(struct ItemSlot));
    memcpy(gSaveBlock1Ptr->bag.keyItems, sBackupPlayerBag->bagPocket_KeyItems, BAG_KEYITEMS_COUNT * sizeof(struct ItemSlot));
    memcpy(gSaveBlock1Ptr->bag.pokeBalls, sBackupPlayerBag->bagPocket_PokeBalls, BAG_POKEBALLS_COUNT * sizeof(struct ItemSlot));
    gSaveBlock1Ptr->registeredItem = sBackupPlayerBag->registeredItem;
    gBagMenuState.bagTab = sBackupPlayerBag->pocket;
    gBagMenuState.pocket = sBagTabToPocket[gBagMenuState.bagTab];
    for (i = 0; i < FRLG_POCKET_COUNT; i++)
    {
        gBagMenuState.cursorPos[i] = sBackupPlayerBag->cursorPos[i];
        gBagMenuState.scrollPosition[i] = sBackupPlayerBag->scrollPosition[i];
    }
    Free(sBackupPlayerBag);
}

void InitOldManBagFrlg(void)
{
    BackUpPlayerBag();
    AddBagItem(ITEM_POTION, 1);
    AddBagItem(ITEM_POKE_BALL, 1);
    GoToBagMenuFrlg(ITEMMENULOCATION_WALLY, FRLG_POCKET_ITEMS, CB2_SetUpReshowBattleScreenAfterMenu);
}

static void Task_Bag_OldManTutorial(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (!gPaletteFade.active)
    {
        switch (data[8])
        {
        case 102:
        case 204:
            PlaySE(SE_RG_BAG_POCKET);
            SwitchPockets(taskId, 1, FALSE);
            break;
        case 306:
            PlaySE(SE_SELECT);
            bag_menu_print_cursor_(tListTaskId, 2);
            Bag_FillMessageBoxWithPalette(1);
            gSpecialVar_ItemId = ITEM_POKE_BALL;
            OpenContextMenu(taskId);
            break;
        case 408:
            PlaySE(SE_SELECT);
            HideBagWindow(FRLG_BAG_SUBWINDOW_11);
            HideBagWindow(FRLG_BAG_SUBWINDOW_7);
            PutWindowTilemap(0);
            PutWindowTilemap(1);
            CopyWindowToVram(0, COPYWIN_MAP);
            DestroyListMenuTask(tListTaskId, NULL, NULL);
            RestorePlayerBag();
            Bag_BeginCloseWin0Animation();
            gTasks[taskId].func = Task_Pokedude_FadeFromBag;
            return;
        }
        data[8]++;
    }
}

static void Task_Pokedude_FadeFromBag(u8 taskId)
{
    BeginNormalPaletteFade(PALETTES_ALL, -2, 0, 16, RGB_BLACK);
    gTasks[taskId].func = Task_Pokedude_WaitFadeAndExitBag;
}

static void Task_Pokedude_WaitFadeAndExitBag(u8 taskId)
{
    if (!gPaletteFade.active && FuncIsActiveTask(Task_AnimateWin0v) != TRUE)
    {
        if (sBagMenuDisplay->exitCB != NULL)
            SetMainCallback2(sBagMenuDisplay->exitCB);
        else
            SetMainCallback2(gBagMenuState.bagCallback);
        BagDestroyPocketScrollArrowPair();
        DestroyBagMenuResources();
        DestroyTask(taskId);
    }
}

static void InitBagWindows(void)
{
    u8 i;

    if (gBagMenuState.location != ITEMMENULOCATION_ITEMPC)
        InitWindows(sDefaultBagWindowsStd);
    else
        InitWindows(sDefaultBagWindowsDeposit);
    DeactivateAllTextPrinters();
    LoadUserWindowBorderGfx(FRLG_BAG_WINDOW_ITEM_LIST, 0x64, BG_PLTT_ID(14));
    LoadMessageBoxGfx(FRLG_BAG_WINDOW_ITEM_LIST, 0x6D, BG_PLTT_ID(13));
    LoadPalette(sBagWindowPalF, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
    for (i = 0; i < 3; i++)
    {
        FillWindowPixelBuffer(i, 0x00);
        PutWindowTilemap(i);
    }
    ScheduleBgCopyTilemapToVram(0);
    for (i = 0; i < 11; i++)
    {
        sOpenWindows[i] = WINDOW_NONE;
    }
}

static void BagPrintTextOnWindow(u8 windowId, u8 fontId, const u8 * str, u8 x, u8 y, u8 letterSpacing, u8 lineSpacing, u8 speed, u8 colorIdx)
{
    AddTextPrinterParameterized4(windowId, fontId, x, y, letterSpacing, lineSpacing, sTextColors[colorIdx], speed, str);
}

static void BagPrintTextOnWin1CenteredColor0(const u8 * str)
{
    u32 x = 0x48 - GetStringWidth(FONT_SHORT, str, 0);
    AddTextPrinterParameterized3(FRLG_BAG_WINDOW_POCKET_NAME, FONT_SHORT, x / 2, 1, sTextColors[0], 0, str);
}

static void BagDrawDepositItemTextBox(void)
{
    u32 x;
    DrawStdFrameWithCustomTileAndPalette(FRLG_BAG_WINDOW_POCKET_NAME, FALSE, 0x081, 14);
    x = 0x40 - GetStringWidth(FONT_SMALL, sText_DepositItem, 0);
    AddTextPrinterParameterized(FRLG_BAG_WINDOW_POCKET_NAME, FONT_SMALL, sText_DepositItem, x / 2, 1, 0, NULL);
}

static u8 ShowBagWindow(u8 whichWindow, u8 numItemStacks)
{
    if (sOpenWindows[whichWindow] == WINDOW_NONE)
    {
        sOpenWindows[whichWindow] = AddWindow(&sWindowTemplates[whichWindow + numItemStacks]);
        DrawStdFrameWithCustomTileAndPalette(sOpenWindows[whichWindow], FALSE, 0x064, 14);
        ScheduleBgCopyTilemapToVram(0);
    }
    return sOpenWindows[whichWindow];
}

static void HideBagWindow(u8 whichWindow)
{
    ClearStdWindowAndFrameToTransparent(sOpenWindows[whichWindow], FALSE);
    ClearWindowTilemap(sOpenWindows[whichWindow]);
    RemoveWindow(sOpenWindows[whichWindow]);
    ScheduleBgCopyTilemapToVram(0);
    sOpenWindows[whichWindow] = WINDOW_NONE;
}

static u8 OpenBagWindow(u8 whichWindow)
{
    if (sOpenWindows[whichWindow] == WINDOW_NONE)
    {
        sOpenWindows[whichWindow] = AddWindow(&sWindowTemplates[whichWindow]);
    }
    return sOpenWindows[whichWindow];
}

static void CloseBagWindow(u8 whichWindow)
{
    if (sOpenWindows[whichWindow] != WINDOW_NONE)
    {
        ClearDialogWindowAndFrameToTransparent(sOpenWindows[whichWindow], FALSE);
        ClearWindowTilemap(sOpenWindows[whichWindow]);
        RemoveWindow(sOpenWindows[whichWindow]);
        PutWindowTilemap(1);
        ScheduleBgCopyTilemapToVram(0);
        sOpenWindows[whichWindow] = WINDOW_NONE;
    }
}

static u8 GetBagWindow(u8 whichWindow)
{
    return sOpenWindows[whichWindow];
}

static void BagCreateYesNoMenuBottomRight(u8 taskId, const struct YesNoFuncTable * ptrs)
{
    CreateYesNoMenuWithCallbacks(taskId, &sWindowTemplates[FRLG_BAG_SUBWINDOW_TOSS_YESNO], FONT_NORMAL, 0, 2, 0x064, 14, ptrs);
}

static void BagCreateYesNoMenuTopRight(u8 taskId, const struct YesNoFuncTable * ptrs)
{
    CreateYesNoMenuWithCallbacks(taskId, &sWindowTemplates[FRLG_BAG_SUBWINDOW_SELL_YESNO], FONT_NORMAL, 0, 2, 0x064, 14, ptrs);
}

static void BagPrintMoneyAmount(void)
{
    PrintMoneyAmountInMoneyBoxWithBorder(ShowBagWindow(FRLG_BAG_SUBWINDOW_MONEY, 0), 0x081, 0x0C, GetMoney(&gSaveBlock1Ptr->money));
    AddMoneyLabelObject(19, 11);
}

static void BagDrawTextBoxOnWindow(u8 windowId)
{
    DrawTextBorderOuter(windowId, 0x064, 14);
}

#undef tListTaskId
#undef tListPosition
#undef tQuantity
#undef tNeverRead
#undef tItemCount
#undef tMsgWindowId
#undef tPocketSwitchDir
#undef tPocketSwitchTimer
#undef tPocketSwitchState
