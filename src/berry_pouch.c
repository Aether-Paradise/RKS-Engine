#include "global.h"
#include "main.h"
#include "battle.h"
#include "battle_controllers.h"
#include "berry.h"
#include "berry_pouch.h"
#include "berry_tag_screen.h"
#include "bg.h"
#include "decompress.h"
#include "gpu_regs.h"
#include "graphics.h"
#include "item_menu.h"
#include "item_menu_icons_rg.h"
#include "item_icon.h"
#include "menu.h"
#include "menu_helpers.h"
#include "money.h"
#include "list_menu.h"
#include "item.h"
#include "item_use.h"
#include "malloc.h"
#include "palette.h"
#include "party_menu.h"
#include "pokemon_storage_system.h"
#include "scanline_effect.h"
#include "shop.h"
#include "sound.h"
#include "strings.h"
#include "string_util.h"
#include "text.h"
#include "text_window.h"
#include "link.h"
#include "constants/items.h"
#include "constants/rgb.h"
#include "constants/songs.h"

struct BerryPouchStruct
{
    void (*exitCallback)(void);
    u16 indicatorOffset;
    u8 indicatorTaskId;
    u8 listMenuNumItems;
    u8 listMenuMaxShowed;
    u8 itemMenuIconId;
    u8 ALIGNED(4) bg1TilemapBuffer[BG_SCREEN_SIZE];
    s16 data[4];
};

struct BerryPouchStaticStruct
{
    void (*savedCallback)(void);
    u8 type;
    u8 allowSelect;
    u8 unused_06;
    u16 listMenuSelectedRow;
    u16 listMenuScrollOffset;
};

enum
{
    BP_ACTION_USE = 0,
    BP_ACTION_TOSS,
    BP_ACTION_GIVE,
    BP_ACTION_EXIT,
    BP_ACTION_NAME,
    BP_ACTION_AMOUNT,
    BP_ACTION_INDEX,
    BP_ACTION_CHECK_TAG,
    BP_ACTION_CONFIRM,
};

enum
{
    BP_WINDOW_BERRY_LIST,
    BP_WINDOW_DESCRIPTION,
    BP_WINDOW_POUCH_HEADER,
    BP_WINDOW_COUNT,
};

enum
{
    BP_VAR_WINDOW_TOSS_AMOUNT,
    BP_VAR_WINDOW_SELL_AMOUNT,
    BP_VAR_WINDOW_MONEY,
    BP_VAR_WINDOW_TOSS_YESNO,
    BP_VAR_WINDOW_SELL_YESNO,
    BP_VAR_WINDOW_MESSAGE,
    BP_VAR_WINDOW_BERRY_SELECTED,
    BP_VAR_WINDOW_TOSS_ASK,
    BP_VAR_WINDOW_TOSS_SELECT,
    BP_VAR_WINDOW_THREW_AWAY,
    BP_VAR_WINDOW_OPTIONS_1,
    BP_VAR_WINDOW_OPTIONS_2,
    BP_VAR_WINDOW_OPTIONS_3,
    BP_VAR_WINDOW_OPTIONS_4,
    BP_VAR_WINDOW_OPTIONS_5,
    BP_VAR_WINDOW_COUNT,
};

enum {
    BP_COLORID_WHITE,
    BP_COLORID_DARK_GARY,
    BP_COLORID_LIGHT_GRAY,
    BP_COLORID_TEXT,
    BP_COLOR_CURSOR_ERASE = 0xFF
};

static EWRAM_DATA struct BerryPouchStruct *sBerryPouchDynamicResources = NULL;
static EWRAM_DATA struct BerryPouchStaticStruct sBerryPouchStaticResources = {};
static EWRAM_DATA struct ListMenuItem *sListMenuItems = NULL;
static EWRAM_DATA u8 * sListMenuStrbuf = NULL;
static EWRAM_DATA const u8 * sContextMenuOptions = NULL;
static EWRAM_DATA u8 sContextMenuNumOptions = 0;
static ALIGNED(4) EWRAM_DATA u8 sVariableWindowIds[BP_VAR_WINDOW_COUNT] = {};
static ALIGNED(4) EWRAM_DATA u8 sBerryPouchSpriteId = 0;
static EWRAM_DATA u8 sIsInBerryPouch = FALSE;

static void CB2_InitBerryPouch(void);
static bool8 RunBerryPouchInit(void);
static void AbortBerryPouchLoading(void);
static void Task_AbortBerryPouchLoading_WaitFade(u8 taskId);
static void BerryPouchInitBgs(void);
static bool8 BerryPouchLoadGfx(void);
static bool8 AllocateListMenuBuffers(void);
static void SetUpListMenuTemplate(void);
static void GetBerryNameAndIndexForMenu(u8 * dest, u16 itemId);
static void BerryPouchMoveCursorFunc(s32 itemIndex, bool8 onInit, struct ListMenu *list);
static void BerryPouchItemPrintFunc(u8 windowId, u32 itemId, u8 y);
static void BerryPouchSetArrowCursorAt(u8 y, u8 colorIdx);
static void PrintSelectedBerryDescription(s32 itemIndex);
static void CreateScrollIndicatorArrows_BerryPouchList(void);
static void PrintBerryPouchHeaderCentered(void);
static void SanitizeListMenuSelectionParams(void);
static void UpdateListMenuScrollOffset(void);
static void BerryPouch_DestroyResources(void);
static void Task_BerryPouchFadeToExitCallback(u8 taskId);
static void SortAndCountBerries(void);
static void Task_BerryPouchMain(u8 taskId);
static void Task_NormalContextMenu(u8 taskId);
static void Task_SortContextMenu(u8 taskId);
static void Task_NormalContextMenu_HandleInput(u8 taskId);
static void Task_BerryPouch_Use(u8 taskId);
static void Task_BerryPouch_Toss(u8 taskId);
static void Task_AskTossMultiple(u8 taskId);
static void Task_TossNo(u8 taskId);
static void Task_Toss_SelectMultiple(u8 taskId);
static void Task_TossYes(u8 taskId);
static void Task_WaitButtonThenTossBerries(u8 taskId);
static void Task_BerryPouch_Give(u8 taskId);
static void Task_Give_PrintThereIsNoPokemon(u8 taskId);
static void Task_WaitButtonBeforeDialogueWindowDestruction(u8 taskId);
static void Task_BerryPouch_Exit(u8 taskId);
static void Task_BerryPouch_Name(u8 taskId);
static void Task_BerryPouch_Amount(u8 taskId);
static void Task_BerryPouch_Index(u8 taskId);
static void Task_BerryPouchSortItems(u8 taskId);
static void Task_BerryPouch_CheckTag(u8 taskId);
static void Task_BerryPouch_Confirm(u8 taskId);
static void Task_ContextMenu_FromPartyGiveMenu(u8 taskId);
static void Task_ContextMenu_FromPokemonPC(u8 taskId);
static void Task_ContextMenu_Sell(u8 taskId);
static void Task_AskSellMultiple(u8 taskId);
static void Task_SellMultiple_CreateYesNoMenu(u8 taskId);
static void Task_SellNo(u8 taskId);
static void Task_Sell_PrintSelectMultipleUI(u8 taskId);
static void SellMultiple_UpdateSellPriceDisplay(s32 price);
static void Task_Sell_SelectMultiple(u8 taskId);
static void Task_SellYes(u8 taskId);
static void Task_SellBerries_PlaySfxAndRemoveBerries(u8 taskId);
static void Task_SellBerries_WaitButton(u8 taskId);
static void BerryPouchInitWindows(void);
static void BerryPouchPrint(u8 windowId, u8 fontId, const u8 * str, u8 x, u8 y, u8 letterSpacing, u8 lineSpacing, u8 speed, u8 colorIdx);
static u8 GetOrCreateVariableWindow(u8 winIdx);
static void DestroyVariableWindow(u8 winIdx);
static void TryDestroyVariableWindow(u8 winIdx);
static u8 GetVariableWindowId(u8 winIdx);
static void CreateTossYesNoMenu(u8 taskId, const struct YesNoFuncTable *ptrs);
static void CreateSellYesNoMenu(u8 taskId, const struct YesNoFuncTable *ptrs);
static void PrintMoneyInWin2(void);
static void CreateBerryPouchSprite(void);
static void StartBerryPouchSpriteWobbleAnim(void);
static void SpriteCB_BerryPouchWaitWobbleAnim(struct Sprite *sprite);

static const struct BgTemplate sBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0x000
    },
    {
        .bg = 1,
        .charBaseIndex = 3,
        .mapBaseIndex = 30,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 2,
        .baseTile = 0x000
    },
    {
        .bg = 2,
        .charBaseIndex = 0,
        .mapBaseIndex = 29,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0x000
    }
};

static const TaskFunc sBerryPouchContextMenuTasks[] =
{
    [BERRYPOUCH_FIELD] = Task_NormalContextMenu,
    [BERRYPOUCH_GIVE_PARTY] = Task_ContextMenu_FromPartyGiveMenu,
    [BERRYPOUCH_SELL] = Task_ContextMenu_Sell,
    [BERRYPOUCH_GIVE_PC] = Task_ContextMenu_FromPokemonPC,
    [BERRYPOUCH_BATTLE] = Task_NormalContextMenu,
    [BERRYPOUCH_BERRY_BLENDER_CRUSH] = Task_NormalContextMenu,
};

static const struct YesNoFuncTable sYesNoFuncs_Toss =
{
    .yesFunc = Task_TossYes,
    .noFunc  = Task_TossNo
};

static const struct YesNoFuncTable sYesNoFuncs_Sell =
{
    .yesFunc = Task_SellYes,
    .noFunc  = Task_SellNo
};

static const struct MenuAction sContextMenuActions[] =
{
    [BP_ACTION_USE]         = { COMPOUND_STRING("Use"),       {Task_BerryPouch_Use} },
    [BP_ACTION_TOSS]        = { COMPOUND_STRING("Toss"),      {Task_BerryPouch_Toss} },
    [BP_ACTION_GIVE]        = { COMPOUND_STRING("Give"),      {Task_BerryPouch_Give} },
    [BP_ACTION_EXIT]        = { COMPOUND_STRING("Exit"),      {Task_BerryPouch_Exit} },
    [BP_ACTION_NAME]        = { COMPOUND_STRING("Name"),      {Task_BerryPouch_Name} },
    [BP_ACTION_AMOUNT]      = { COMPOUND_STRING("Amount"),    {Task_BerryPouch_Amount} },
    [BP_ACTION_INDEX]       = { COMPOUND_STRING("Index"),     {Task_BerryPouch_Index} },
    [BP_ACTION_CHECK_TAG]   = { COMPOUND_STRING("Check"),     {Task_BerryPouch_CheckTag} },
    [BP_ACTION_CONFIRM]     = { COMPOUND_STRING("Confirm"),   {Task_BerryPouch_Confirm} },
};

static const u8 *const sSortTypeStrings[] =
{
    [SORT_ALPHABETICALLY] = COMPOUND_STRING("name"),
    [SORT_BY_TYPE] = COMPOUND_STRING("type"),
    [SORT_BY_AMOUNT] = COMPOUND_STRING("amount"),
    [SORT_BY_INDEX] = COMPOUND_STRING("index")
};

static const u8 sOptions_UseGiveTossExit[] =
{
    BP_ACTION_USE,
    BP_ACTION_CHECK_TAG,
    BP_ACTION_GIVE,
    BP_ACTION_TOSS,
    BP_ACTION_EXIT
};

static const u8 sOptions_GiveExit[] =
{
    BP_ACTION_GIVE,
    BP_ACTION_EXIT,
};

static const u8 sOptions_UseExit[] =
{
    BP_ACTION_USE,
    BP_ACTION_EXIT,
};

static const u8 sOptions_Exit[] =
{
    BP_ACTION_EXIT,
};

static const u8 sOptions_NameAmountIndexExit[] =
{
    BP_ACTION_NAME,
    BP_ACTION_AMOUNT,
    BP_ACTION_INDEX,
    BP_ACTION_EXIT
};

static const u8 sOptions_ConfirmCheckExit[] =
{
    BP_ACTION_CONFIRM,
    BP_ACTION_CHECK_TAG,
    BP_ACTION_EXIT
};

static const u8 sText_Space[] = _(" ");
static const u8 sText_Close[] = _("Close");
static const u8 sText_FontSmall[] = _("{FONT_SMALL}");
static const u8 sText_FontNormal[] = _("{FONT_SHORT}");
static const u8 sText_BerryPouch[] = _("BERRY POUCH");
static const u8 sText_TheBerryPouchWillBePutAway[] = _("The Berry Pouch will be\nput away.");
static const u8 sText_TossOutHowManyStrVar1s[] = _("Toss out how many\n{STR_VAR_1}?");
static const u8 sText_ThrowAwayStrVar2OfThisItemQM[] = _("Throw away {STR_VAR_2} of\nthis item?");
static const u8 sText_SortItemsHow[] = _("Sort items how?");
static const u8 sText_ItemsSorted[] = _("Items sorted by {STR_VAR_1}!");
static const u8 sText_NothingToSort[] = _("There's nothing to sort!");

static const u32 sBerryPouchSpriteTiles[] = INCBIN_U32("graphics/berry_pouch/berry_pouch.4bpp.smol");
static const u8 sBerryPouchBgGfx[] = INCBIN_U8("graphics/berry_pouch/background.4bpp.smol");
static const u16 sBerryPouchBgPals[] = INCBIN_U16("graphics/berry_pouch/background.gbapal");
static const u16 sBerryPouchBgPal0FemaleOverride[] = INCBIN_U16("graphics/berry_pouch/background_female.gbapal");
static const u16 sBerryPouchSpritePalette[] = INCBIN_U16("graphics/berry_pouch/berry_pouch.gbapal");
static const u32 sBerryPouchBg1Tilemap[] = INCBIN_U32("graphics/berry_pouch/background.bin.smolTM");


static const struct WindowTemplate sWindowTemplates_Main[] =
{
    [BP_WINDOW_BERRY_LIST] =
    {
        .bg = 0,
        .tilemapLeft = 11,
        .tilemapTop = 1,
        .width = 18,
        .height = 14,
        .paletteNum = 15,
        .baseBlock = 0x027
    },
    [BP_WINDOW_DESCRIPTION] =
    {
        .bg = 0,
        .tilemapLeft = 5,
        .tilemapTop = 16,
        .width = 25,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x123
    },
    [BP_WINDOW_POUCH_HEADER] =
    {
        .bg = 2,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 9,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 0x187
    },
    DUMMY_WIN_TEMPLATE
};

static const struct WindowTemplate sWindowTemplates_Variable[] =
{
    [BP_VAR_WINDOW_TOSS_AMOUNT] =
    {
        .bg = 2,
        .tilemapLeft = 24,
        .tilemapTop = 15,
        .width = 5,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x1d1
    },
    [BP_VAR_WINDOW_SELL_AMOUNT] =
    {
        .bg = 2,
        .tilemapLeft = 17,
        .tilemapTop = 9,
        .width = 12,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x1d1
    },
    [BP_VAR_WINDOW_MONEY] =
    {
        .bg = 2,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 10,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 0x201
    },
    [BP_VAR_WINDOW_TOSS_YESNO] =
    {
        .bg = 2,
        .tilemapLeft = 23,
        .tilemapTop = 15,
        .width = 6,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x219
    },
    [BP_VAR_WINDOW_SELL_YESNO] =
    {
        .bg = 2,
        .tilemapLeft = 21,
        .tilemapTop = 9,
        .width = 6,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x219
    },
    [BP_VAR_WINDOW_MESSAGE] =
    {
        .bg = 2,
        .tilemapLeft = 2,
        .tilemapTop = 15,
        .width = 26,
        .height = 4,
        .paletteNum = 13,
        .baseBlock = 0x231
    },
    [BP_VAR_WINDOW_BERRY_SELECTED] =
    {
        .bg = 2,
        .tilemapLeft = 6,
        .tilemapTop = 15,
        .width = 14,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x231
    },
    [BP_VAR_WINDOW_TOSS_ASK] =
    {
        .bg = 2,
        .tilemapLeft = 6,
        .tilemapTop = 15,
        .width = 15,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x269
    },
    [BP_VAR_WINDOW_TOSS_SELECT] =
    {
        .bg = 2,
        .tilemapLeft = 6,
        .tilemapTop = 15,
        .width = 16,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x2a5
    },
    [BP_VAR_WINDOW_THREW_AWAY] =
    {
        .bg = 2,
        .tilemapLeft = 6,
        .tilemapTop = 15,
        .width = 23,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x2e5
    },
    [BP_VAR_WINDOW_OPTIONS_1] =
    {
        .bg = 2,
        .tilemapLeft = 22,
        .tilemapTop = 17,
        .width = 7,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 0x199
    },
    [BP_VAR_WINDOW_OPTIONS_2] =
    {
        .bg = 2,
        .tilemapLeft = 22,
        .tilemapTop = 15,
        .width = 7,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x199
    },
    [BP_VAR_WINDOW_OPTIONS_3] =
    {
        .bg = 2,
        .tilemapLeft = 22,
        .tilemapTop = 13,
        .width = 7,
        .height = 6,
        .paletteNum = 15,
        .baseBlock = 0x199
    },
    [BP_VAR_WINDOW_OPTIONS_4] =
    {
        .bg = 2,
        .tilemapLeft = 22,
        .tilemapTop = 11,
        .width = 7,
        .height = 8,
        .paletteNum = 15,
        .baseBlock = 0x199
    },
    [BP_VAR_WINDOW_OPTIONS_5] =
    {
        .bg = 2,
        .tilemapLeft = 22,
        .tilemapTop = 9,
        .width = 7,
        .height = 10,
        .paletteNum = 15,
        .baseBlock = 0x199
    }
};

#define TEXT_COLOR_BP_TRANSPARENT TEXT_COLOR_TRANSPARENT
#define TEXT_COLOR_BP_DARK_GRAY TEXT_COLOR_DARK_GRAY
#define TEXT_COLOR_BP_WHITE TEXT_COLOR_WHITE
#define TEXT_COLOR_BP_LIGHT_GRAY TEXT_COLOR_LIGHT_GRAY
#define TEXT_COLOR_BP_MESSAGE_NORMAL TEXT_COLOR_DARK_GRAY
#define TEXT_COLOR_BP_MESSAGE_SHADOW TEXT_COLOR_LIGHT_GRAY

static const u8 sTextColors[][3] =
{
    [BP_COLORID_WHITE]      = { TEXT_COLOR_BP_TRANSPARENT, TEXT_COLOR_BP_WHITE, TEXT_COLOR_BP_DARK_GRAY },
    [BP_COLORID_DARK_GARY]  = { TEXT_COLOR_BP_TRANSPARENT, TEXT_COLOR_BP_DARK_GRAY, TEXT_COLOR_BP_LIGHT_GRAY },
    [BP_COLORID_LIGHT_GRAY] = { TEXT_COLOR_BP_TRANSPARENT, TEXT_COLOR_BP_LIGHT_GRAY, TEXT_COLOR_BP_DARK_GRAY },
    [BP_COLORID_TEXT]       = { TEXT_COLOR_BP_TRANSPARENT, TEXT_COLOR_BP_MESSAGE_NORMAL, TEXT_COLOR_BP_MESSAGE_SHADOW },
};

static const struct OamData sOamData = {
    .affineMode = ST_OAM_AFFINE_NORMAL,
    .shape = ST_OAM_SQUARE,
    .size = ST_OAM_SIZE_3,
    .priority = 1
};

static const union AnimCmd sSpriteAnim_Dummy[] = {
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable[] = {
    sSpriteAnim_Dummy
};

static const union AffineAnimCmd sSpriteAffineAnim_Static[] = {
    AFFINEANIMCMD_FRAME(0x100, 0x100, 0, 0),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sSpriteAffineAnim_Wobble[] = {
    AFFINEANIMCMD_FRAME(0, 0, -2, 2),
    AFFINEANIMCMD_FRAME(0, 0,  2, 4),
    AFFINEANIMCMD_FRAME(0, 0, -2, 4),
    AFFINEANIMCMD_FRAME(0, 0,  2, 2),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd *const sSpriteAffineAnimTable[] = {
    sSpriteAffineAnim_Static,
    sSpriteAffineAnim_Wobble
};

static const struct CompressedSpriteSheet sBerryPouchSpriteSheet = {
    sBerryPouchSpriteTiles, 0x800, 100
};

static const struct SpritePalette sBerryPouchSpritePal = {
    sBerryPouchSpritePalette, 100
};

static const struct SpriteTemplate sSpriteTemplate_BerryPouch = {
    100, 100, &sOamData, sSpriteAnimTable, NULL, sSpriteAffineAnimTable, SpriteCallbackDummy
};

#define tListTaskId     data[0]
#define tListPosition   data[1]
#define tQuantity       data[2]
#define tItemCount      data[8]

void InitBerryPouch(u8 type, void (*savedCallback)(void), u8 allowSelect)
{
    u8 i;

    sBerryPouchDynamicResources = Alloc(sizeof(struct BerryPouchStruct));
    if (sBerryPouchDynamicResources == NULL)
    {
        SetMainCallback2(savedCallback);
    }
    else
    {
        sIsInBerryPouch = TRUE;
        if (type != BERRYPOUCH_REOPENING)
            sBerryPouchStaticResources.type = type;
        if (allowSelect != BERRYPOUCH_KEEP_PREV)
            sBerryPouchStaticResources.allowSelect = allowSelect;
        if (savedCallback != NULL)
            sBerryPouchStaticResources.savedCallback = savedCallback;
        sBerryPouchDynamicResources->exitCallback = NULL;
        sBerryPouchDynamicResources->itemMenuIconId = 0;
        sBerryPouchStaticResources.listMenuScrollOffset = gBagPosition.scrollPosition[POCKET_BERRIES];
        sBerryPouchStaticResources.listMenuSelectedRow = gBagPosition.cursorPosition[POCKET_BERRIES];
        sBerryPouchDynamicResources->indicatorTaskId = TASK_NONE;
        for (i = 0; i < 4; i++)
            sBerryPouchDynamicResources->data[i] = 0;
        gTextFlags.autoScroll = FALSE;
        gSpecialVar_ItemId = ITEM_NONE;
        SetMainCallback2(CB2_InitBerryPouch);
    }
}

static void CB2_BerryPouchIdle(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void VBlankCB_BerryPouchIdle(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void CB2_InitBerryPouch(void)
{
    while (TRUE)
    {
        if (MenuHelpers_ShouldWaitForLinkRecv())
            break;
        if (RunBerryPouchInit())
            break;
        if (MenuHelpers_IsLinkActive())
            break;
    }
}

static bool8 RunBerryPouchInit(void)
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
        if (!MenuHelpers_IsLinkActive())
            ResetTasks();
        gMain.state++;
        break;
    case 7:
        BerryPouchInitBgs();
        sBerryPouchDynamicResources->data[0] = 0;
        gMain.state++;
        break;
    case 8:
        if (BerryPouchLoadGfx())
            gMain.state++;
        break;
    case 9:
        BerryPouchInitWindows();
        gMain.state++;
        break;
    case 10:
        SortAndCountBerries();
        SanitizeListMenuSelectionParams();
        UpdateListMenuScrollOffset();
        gMain.state++;
        break;
    case 11:
        if (!AllocateListMenuBuffers())
        {
            AbortBerryPouchLoading();
            return TRUE;
        }
        gMain.state++;
        break;
    case 12:
        SetUpListMenuTemplate();
        gMain.state++;
        break;
    case 13:
        PrintBerryPouchHeaderCentered();
        gMain.state++;
        break;
    case 14:
        taskId = CreateTask(Task_BerryPouchMain, 0);
        gTasks[taskId].tListTaskId = ListMenuInit(&gMultiuseListMenuTemplate, sBerryPouchStaticResources.listMenuScrollOffset, sBerryPouchStaticResources.listMenuSelectedRow);
        gTasks[taskId].tItemCount = 0;
        gMain.state++;
        break;
    case 15:
        CreateBerryPouchSprite();
        gMain.state++;
        break;
    case 16:
        CreateScrollIndicatorArrows_BerryPouchList();
        gMain.state++;
        break;
    case 17:
        BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
        gMain.state++;
        break;
    case 18:
        BeginNormalPaletteFade(PALETTES_ALL, -2, 16, 0, RGB_BLACK);
        gMain.state++;
        break;
    default:
        SetVBlankCallback(VBlankCB_BerryPouchIdle);
        SetMainCallback2(CB2_BerryPouchIdle);
        return TRUE;
    }

    return FALSE;
}

static void AbortBerryPouchLoading(void)
{
    BeginNormalPaletteFade(PALETTES_ALL, -2, 0, 16, RGB_BLACK);
    CreateTask(Task_AbortBerryPouchLoading_WaitFade, 0);
    SetVBlankCallback(VBlankCB_BerryPouchIdle);
    SetMainCallback2(CB2_BerryPouchIdle);
}

static void Task_AbortBerryPouchLoading_WaitFade(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sBerryPouchStaticResources.savedCallback);
        BerryPouch_DestroyResources();
        DestroyTask(taskId);
    }
}

static void BerryPouchInitBgs(void)
{
    ResetAllBgsCoordinatesAndBgCntRegs();
    memset(sBerryPouchDynamicResources->bg1TilemapBuffer, 0, BG_SCREEN_SIZE);
    ResetBgsAndClearDma3BusyFlags(FALSE);
    InitBgsFromTemplates(0, sBgTemplates, NELEMS(sBgTemplates));
    SetBgTilemapBuffer(1, sBerryPouchDynamicResources->bg1TilemapBuffer);
    ScheduleBgCopyTilemapToVram(1);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON);
    ShowBg(0);
    ShowBg(1);
    ShowBg(2);
}

static bool8 BerryPouchLoadGfx(void)
{
    switch (sBerryPouchDynamicResources->data[0])
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(1, sBerryPouchBgGfx, 0, 0, 0);
        sBerryPouchDynamicResources->data[0]++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            DecompressDataWithHeaderWram(sBerryPouchBg1Tilemap, sBerryPouchDynamicResources->bg1TilemapBuffer);
            sBerryPouchDynamicResources->data[0]++;
        }
        break;
    case 2:
        LoadPalette(sBerryPouchBgPals, BG_PLTT_ID(0), 3 * PLTT_SIZE_4BPP);
        if (gSaveBlock2Ptr->playerGender != MALE)
            LoadPalette(sBerryPouchBgPal0FemaleOverride, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
        sBerryPouchDynamicResources->data[0]++;
        break;
    case 3:
        LoadCompressedSpriteSheet(&sBerryPouchSpriteSheet);
        sBerryPouchDynamicResources->data[0]++;
        break;
    default:
        LoadSpritePalette(&sBerryPouchSpritePal);
        sBerryPouchDynamicResources->data[0] = 0;
        return TRUE;
    }

    return FALSE;
}

static bool8 AllocateListMenuBuffers(void)
{
    sListMenuItems = Alloc((BAG_BERRIES_COUNT + 1) * sizeof(struct ListMenuItem));
    if (sListMenuItems == NULL)
        return FALSE;
    sListMenuStrbuf = Alloc(sBerryPouchDynamicResources->listMenuNumItems * 28);
    if (sListMenuStrbuf == NULL)
        return FALSE;
    return TRUE;
}

static inline u16 GetBerryPouchItemIdByPosition(u32 index)
{
    return GetBagItemId(POCKET_BERRIES, index);
}

static void SetUpListMenuTemplate(void)
{
    u16 i;
    for (i = 0; i < sBerryPouchDynamicResources->listMenuNumItems; i++)
    {
        GetBerryNameAndIndexForMenu(&sListMenuStrbuf[i * 28], GetBerryPouchItemIdByPosition(i));
        sListMenuItems[i].name = &sListMenuStrbuf[i * 28];
        sListMenuItems[i].id = i;
    }
    sListMenuItems[i].name = sText_Close;
    sListMenuItems[i].id = i;
    gMultiuseListMenuTemplate.items = sListMenuItems;
    if (sBerryPouchStaticResources.type != BERRYPOUCH_BERRY_BLENDER_CRUSH)
        gMultiuseListMenuTemplate.totalItems = sBerryPouchDynamicResources->listMenuNumItems + 1;
    else
        gMultiuseListMenuTemplate.totalItems = sBerryPouchDynamicResources->listMenuNumItems;
    gMultiuseListMenuTemplate.windowId = BP_WINDOW_BERRY_LIST;
    gMultiuseListMenuTemplate.header_X = 0;
    gMultiuseListMenuTemplate.item_X = 9;
    gMultiuseListMenuTemplate.cursor_X = 1;
    gMultiuseListMenuTemplate.lettersSpacing = 0;
    gMultiuseListMenuTemplate.itemVerticalPadding = 2;
    gMultiuseListMenuTemplate.upText_Y = 2;
    gMultiuseListMenuTemplate.maxShowed = sBerryPouchDynamicResources->listMenuMaxShowed;
    gMultiuseListMenuTemplate.fontId = FONT_SHORT;
    gMultiuseListMenuTemplate.cursorPal = TEXT_COLOR_BP_DARK_GRAY;
    gMultiuseListMenuTemplate.fillValue = TEXT_COLOR_BP_TRANSPARENT;
    gMultiuseListMenuTemplate.cursorShadowPal = TEXT_COLOR_BP_LIGHT_GRAY;
    gMultiuseListMenuTemplate.moveCursorFunc = BerryPouchMoveCursorFunc;
    gMultiuseListMenuTemplate.itemPrintFunc = BerryPouchItemPrintFunc;
    gMultiuseListMenuTemplate.cursorKind = 0;
    gMultiuseListMenuTemplate.scrollMultiple = 0;
}

static void GetBerryNameAndIndexForMenu(u8 * dest, u16 itemId)
{
    StringCopy(gStringVar4, sText_FontSmall);
    StringAppend(gStringVar4, gText_NumberClear01);
    ConvertIntToDecimalStringN(gStringVar1, itemId - FIRST_BERRY_INDEX + 1, STR_CONV_MODE_LEADING_ZEROS, 2);
    StringAppend(gStringVar4, gStringVar1);
    CopyItemName(itemId, gStringVar1);
    StringAppend(gStringVar4, sText_Space);
    StringAppend(gStringVar4, sText_FontNormal);
    StringAppend(gStringVar4, gStringVar1);
    StringCopy(dest, gStringVar4);
}

static void CopySelectedListMenuItemName(s16 itemIdx, u8 * dest)
{
    StringCopy(dest, &sListMenuStrbuf[itemIdx * 28]);
}

static void BerryPouchMoveCursorFunc(s32 itemIndex, bool8 onInit, struct ListMenu *list)
{
    if (onInit != TRUE)
    {
        PlaySE(SE_RG_BAG_CURSOR);
        StartBerryPouchSpriteWobbleAnim();
    }
    ItemRG_EraseItemIcon(sBerryPouchDynamicResources->itemMenuIconId ^ 1);
    if (sBerryPouchDynamicResources->listMenuNumItems != itemIndex)
        CreateBerryPouchItemIcon(GetBerryPouchItemIdByPosition(itemIndex), sBerryPouchDynamicResources->itemMenuIconId);
    else
        CreateBerryPouchItemIcon(ITEM_LIST_END, sBerryPouchDynamicResources->itemMenuIconId);
    sBerryPouchDynamicResources->itemMenuIconId ^= 1;
    PrintSelectedBerryDescription(itemIndex);
}

static void BerryPouchItemPrintFunc(u8 windowId, u32 itemId, u8 y)
{
    u16 itemQuantity;
    if (itemId != LIST_CANCEL && sBerryPouchDynamicResources->listMenuNumItems != itemId)
    {
        itemQuantity = GetBagItemQuantity(POCKET_BERRIES, itemId);
        ConvertIntToDecimalStringN(gStringVar1, itemQuantity, STR_CONV_MODE_RIGHT_ALIGN, MAX_ITEM_DIGITS);
        StringExpandPlaceholders(gStringVar4, gText_xVar1);
        BerryPouchPrint(windowId, FONT_SMALL, gStringVar4, 110, y, 0, 0, TEXT_SKIP_DRAW, BP_COLORID_DARK_GARY);
    }
}

static void BerryPouchSetArrowCursorFromListMenu(u8 taskId, u8 colorIdx)
{
    BerryPouchSetArrowCursorAt(ListMenuGetYCoordForPrintingArrowCursor(taskId), colorIdx);
}

static void BerryPouchSetArrowCursorAt(u8 y, u8 colorIdx)
{
    u8 width;
    u8 height;
    if (colorIdx == BP_COLOR_CURSOR_ERASE)
    {
        width = GetMenuCursorDimensionByFont(FONT_SHORT, 0);
        height = GetMenuCursorDimensionByFont(FONT_SHORT, 1);
        FillWindowPixelRect(BP_WINDOW_BERRY_LIST, 0, 1, y, width, height);
        CopyWindowToVram(BP_WINDOW_BERRY_LIST, COPYWIN_GFX);
    }
    else
    {
        BerryPouchPrint(BP_WINDOW_BERRY_LIST, FONT_SHORT, gText_SelectorArrow2, 1, y, 0, 0, 0, colorIdx);
    }
}

static void PrintSelectedBerryDescription(s32 itemIdx)
{
    const u8 * str;
    if (itemIdx != sBerryPouchDynamicResources->listMenuNumItems)
    {
        str = GetItemLongDescription(GetBerryPouchItemIdByPosition(itemIdx));
    }
    else
    {
        str = sText_TheBerryPouchWillBePutAway;
    }
    FillWindowPixelBuffer(BP_WINDOW_DESCRIPTION, PIXEL_FILL(0));
    BerryPouchPrint(BP_WINDOW_DESCRIPTION, FONT_SHORT, str, 0, 2, 2, 0, 0, BP_COLORID_WHITE);
}

static void SetDescriptionWindowBorderPalette(s32 pal)
{
    SetBgTilemapPalette(1, 0, 16, 30, 4, pal + 1);
    ScheduleBgCopyTilemapToVram(1);
}

static void CreateScrollIndicatorArrows_BerryPouchList(void)
{
    if (sBerryPouchStaticResources.type != BERRYPOUCH_BERRY_BLENDER_CRUSH)
        sBerryPouchDynamicResources->indicatorTaskId = AddScrollIndicatorArrowPairParameterized(2, 160, 8, 120, sBerryPouchDynamicResources->listMenuNumItems - sBerryPouchDynamicResources->listMenuMaxShowed + 1, 110, 110, &sBerryPouchStaticResources.listMenuScrollOffset);
    else
        sBerryPouchDynamicResources->indicatorTaskId = AddScrollIndicatorArrowPairParameterized(2, 160, 8, 120, sBerryPouchDynamicResources->listMenuNumItems - sBerryPouchDynamicResources->listMenuMaxShowed, 110, 110, &sBerryPouchStaticResources.listMenuScrollOffset);
}

static void CreateScrollIndicatorArrows_TossQuantity(void)
{
    sBerryPouchDynamicResources->indicatorOffset = 1;
    sBerryPouchDynamicResources->indicatorTaskId = AddScrollIndicatorArrowPairParameterized(2, 212, 120, 152, 2, 110, 110, &sBerryPouchDynamicResources->indicatorOffset);
}

static void CreateScrollIndicatorArrows_SellQuantity(void)
{
    sBerryPouchDynamicResources->indicatorOffset = 1;
    sBerryPouchDynamicResources->indicatorTaskId = AddScrollIndicatorArrowPairParameterized(2, 152, 72, 104, 2, 110, 110, &sBerryPouchDynamicResources->indicatorOffset);
}

static void DestroyScrollIndicatorArrows(void)
{
    if (sBerryPouchDynamicResources->indicatorTaskId != TASK_NONE)
    {
        RemoveScrollIndicatorArrowPair(sBerryPouchDynamicResources->indicatorTaskId);
        sBerryPouchDynamicResources->indicatorTaskId = TASK_NONE;
    }
}

static void PrintBerryPouchHeaderCentered(void)
{
    u32 slack = 72 - GetStringWidth(FONT_SHORT, sText_BerryPouch, 0);
    BerryPouchPrint(BP_WINDOW_POUCH_HEADER, FONT_SHORT, sText_BerryPouch, slack / 2, 1, 0, 0, 0, BP_COLORID_WHITE);
}

void BerryPouch_CursorResetToTop(void)
{
    sBerryPouchStaticResources.listMenuSelectedRow = 0;
    sBerryPouchStaticResources.listMenuScrollOffset = 0;
}

static void SanitizeListMenuSelectionParams(void)
{
    s32 itemCount;
    if (sBerryPouchStaticResources.type != BERRYPOUCH_BERRY_BLENDER_CRUSH)
        itemCount = sBerryPouchDynamicResources->listMenuNumItems + 1;
    else
        itemCount = sBerryPouchDynamicResources->listMenuNumItems;
    if (sBerryPouchStaticResources.listMenuScrollOffset != 0 && sBerryPouchStaticResources.listMenuScrollOffset + sBerryPouchDynamicResources->listMenuMaxShowed > itemCount)
        sBerryPouchStaticResources.listMenuScrollOffset = itemCount - sBerryPouchDynamicResources->listMenuMaxShowed;
    if (sBerryPouchStaticResources.listMenuScrollOffset + sBerryPouchStaticResources.listMenuSelectedRow >= itemCount)
    {
        if (itemCount == 0 || itemCount == 1)
            sBerryPouchStaticResources.listMenuSelectedRow = 0;
        else
            sBerryPouchStaticResources.listMenuSelectedRow = itemCount - 1;
    }
}

static void UpdateListMenuScrollOffset(void)
{
    u8 lim;
    u8 i;
    if (sBerryPouchStaticResources.type != BERRYPOUCH_BERRY_BLENDER_CRUSH)
        lim = sBerryPouchDynamicResources->listMenuNumItems + 1;
    else
        lim = sBerryPouchDynamicResources->listMenuNumItems;
    if (sBerryPouchStaticResources.listMenuSelectedRow > 4)
    {
        for (i = 0; i <= sBerryPouchStaticResources.listMenuSelectedRow - 4; sBerryPouchStaticResources.listMenuSelectedRow--, sBerryPouchStaticResources.listMenuScrollOffset++, i++)
        {
            if (sBerryPouchStaticResources.listMenuScrollOffset + sBerryPouchDynamicResources->listMenuMaxShowed == lim)
                break;
        }
    }
}

static void BerryPouch_DestroyResources(void)
{
    if (sBerryPouchDynamicResources != NULL)
        Free(sBerryPouchDynamicResources);
    if (sListMenuItems != NULL)
        Free(sListMenuItems);
    if (sListMenuStrbuf != NULL)
        Free(sListMenuStrbuf);
    FreeAllWindowBuffers();
}

void BerryPouch_StartFadeToExitCallback(u8 taskId)
{
    BeginNormalPaletteFade(PALETTES_ALL, -2, 0, 16, RGB_BLACK);
    gBagPosition.scrollPosition[POCKET_BERRIES] = sBerryPouchStaticResources.listMenuScrollOffset;
    gBagPosition.cursorPosition[POCKET_BERRIES] = sBerryPouchStaticResources.listMenuSelectedRow;
    gTasks[taskId].func = Task_BerryPouchFadeToExitCallback;
}

static void Task_BerryPouchFadeToExitCallback(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    if (!gPaletteFade.active)
    {
        DestroyListMenuTask(tListTaskId, &sBerryPouchStaticResources.listMenuScrollOffset, &sBerryPouchStaticResources.listMenuSelectedRow);
        if (sBerryPouchDynamicResources->exitCallback != NULL)
            SetMainCallback2(sBerryPouchDynamicResources->exitCallback);
        else
            SetMainCallback2(sBerryPouchStaticResources.savedCallback);
        DestroyScrollIndicatorArrows();
        BerryPouch_DestroyResources();
        DestroyTask(taskId);
    }
}

static void SortAndCountBerries(void)
{
    u32 itemCount;
    struct BagPocket *pocket = &gBagPockets[POCKET_BERRIES];
    CompactItemsInBagPocket(POCKET_BERRIES);

    sBerryPouchDynamicResources->listMenuNumItems = 0;

    for (u32 i = 0; i < pocket->capacity && GetBerryPouchItemIdByPosition(i); i++)
        sBerryPouchDynamicResources->listMenuNumItems++;

    if (sBerryPouchStaticResources.type != BERRYPOUCH_BERRY_BLENDER_CRUSH)
        itemCount = sBerryPouchDynamicResources->listMenuNumItems + 1;
    else
        itemCount = sBerryPouchDynamicResources->listMenuNumItems;

    if (itemCount > 7)
        sBerryPouchDynamicResources->listMenuMaxShowed = 7;
    else
        sBerryPouchDynamicResources->listMenuMaxShowed = itemCount;
}

void BerryPouch_SetExitCallback(void (*callback)(void))
{
    sBerryPouchDynamicResources->exitCallback = callback;
}

void InitTossQuantitySelectUI(u8 taskId, const u8 * str)
{
    s16 * data = gTasks[taskId].data;
    u8 windowId = GetOrCreateVariableWindow(BP_VAR_WINDOW_TOSS_SELECT);
    u8 windowId2;
    CopyItemNameHandlePlural(GetBerryPouchItemIdByPosition(tListPosition), gStringVar1, 2);
    StringExpandPlaceholders(gStringVar4, str);
    BerryPouchPrint(windowId, FONT_SHORT, gStringVar4, 0, 2, 1, 2, 0, BP_COLORID_TEXT);
    windowId2 = GetOrCreateVariableWindow(BP_VAR_WINDOW_TOSS_AMOUNT);
    ConvertIntToDecimalStringN(gStringVar1, 1, STR_CONV_MODE_LEADING_ZEROS, MAX_ITEM_DIGITS);
    StringExpandPlaceholders(gStringVar4, gText_xVar1);
    BerryPouchPrint(windowId2, FONT_SMALL, gStringVar4, 4, 10, 1, 0, 0, BP_COLORID_TEXT);
}

static void PrintxQuantityOnWindow(u8 whichWindow, s16 quantity, u8 ndigits)
{
    u8 windowId = GetVariableWindowId(whichWindow);
    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
    ConvertIntToDecimalStringN(gStringVar1, quantity, STR_CONV_MODE_LEADING_ZEROS, ndigits);
    StringExpandPlaceholders(gStringVar4, gText_xVar1);
    BerryPouchPrint(windowId, FONT_SMALL, gStringVar4, 4, 10, 1, 0, 0, BP_COLORID_TEXT);
}

static inline bool32 CanMoveItemsBerryPouch(void)
{
    // Swaps can only be done from the field or in battle (as opposed to while selling items, for example)
    if (sBerryPouchStaticResources.type == BERRYPOUCH_FIELD
     || sBerryPouchStaticResources.type == BERRYPOUCH_BATTLE)
    {
        return TRUE;
    }
    return FALSE;
}

static void Task_BerryPouchMain(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    s32 menuInput;
    if (!gPaletteFade.active && MenuHelpers_ShouldWaitForLinkRecv() != TRUE)
    {
        if (CanMoveItemsBerryPouch())
        {
            if (JOY_NEW(START_BUTTON))
            {
                if ((sBerryPouchDynamicResources->listMenuNumItems - 1) <= 1) //can't sort with 0 or 1 item in bag
                {
                    PlaySE(SE_FAILURE);
                    DisplayItemMessageInBerryPouch(taskId, FONT_SHORT, sText_NothingToSort, Task_WaitButtonBeforeDialogueWindowDestruction);
                }
                else
                {
                    menuInput = ListMenu_ProcessInput(tListTaskId);
                    tListPosition = menuInput;
                    if (menuInput != sBerryPouchDynamicResources->listMenuNumItems)
                    {
                        gSpecialVar_ItemId = GetBerryPouchItemIdByPosition(menuInput);
                        tQuantity = GetBagItemQuantity(POCKET_BERRIES, menuInput);
                    }

                    PlaySE(SE_SELECT);
                    DestroyScrollIndicatorArrows();
                    SetDescriptionWindowBorderPalette(1);
                    BerryPouchSetArrowCursorFromListMenu(tListTaskId, BP_COLORID_LIGHT_GRAY);
                    ListMenuGetScrollAndRow(tListTaskId, &sBerryPouchStaticResources.listMenuScrollOffset, &sBerryPouchStaticResources.listMenuSelectedRow);
                    gTasks[taskId].func = Task_SortContextMenu;
                    return;
                }
            }
        }
        menuInput = ListMenu_ProcessInput(tListTaskId);
        ListMenuGetScrollAndRow(tListTaskId, &sBerryPouchStaticResources.listMenuScrollOffset, &sBerryPouchStaticResources.listMenuSelectedRow);
        if (JOY_NEW(SELECT_BUTTON) && sBerryPouchStaticResources.allowSelect == TRUE)
        {
            PlaySE(SE_SELECT);
            gSpecialVar_ItemId = 0;
            sIsInBerryPouch = FALSE;
            BerryPouch_StartFadeToExitCallback(taskId);
        }
        else
        {
            switch (menuInput)
            {
            case LIST_NOTHING_CHOSEN:
                return;
            case LIST_CANCEL:
                if (sBerryPouchStaticResources.type != BERRYPOUCH_BERRY_BLENDER_CRUSH)
                {
                    PlaySE(SE_SELECT);
                    gSpecialVar_ItemId = 0;
                    sIsInBerryPouch = FALSE;
                    BerryPouch_StartFadeToExitCallback(taskId);
                }
                break;
            default:
                PlaySE(SE_SELECT);
                if (sBerryPouchStaticResources.type == BERRYPOUCH_BERRY_TREE)
                {
                    gSpecialVar_ItemId = GetBerryPouchItemIdByPosition(menuInput);
                    sIsInBerryPouch = FALSE;
                    BerryPouch_StartFadeToExitCallback(taskId);
                }
                else if (menuInput == sBerryPouchDynamicResources->listMenuNumItems)
                {
                    gSpecialVar_ItemId = 0;
                    sIsInBerryPouch = FALSE;
                    BerryPouch_StartFadeToExitCallback(taskId);
                }
                else
                {
                    DestroyScrollIndicatorArrows();
                    SetDescriptionWindowBorderPalette(1);
                    BerryPouchSetArrowCursorFromListMenu(tListTaskId, BP_COLORID_LIGHT_GRAY);
                    tListPosition = menuInput;
                    tQuantity = GetBagItemQuantity(POCKET_BERRIES, menuInput);
                    gSpecialVar_ItemId = GetBerryPouchItemIdByPosition(menuInput);
                    gTasks[taskId].func = sBerryPouchContextMenuTasks[sBerryPouchStaticResources.type];
                }
                break;
            }
        }
    }
}

static void Task_CleanUpAndReturnToMain(u8 taskId)
{
    SetDescriptionWindowBorderPalette(0);
    CreateScrollIndicatorArrows_BerryPouchList();
    gTasks[taskId].func = Task_BerryPouchMain;
}

static void CreateNormalContextMenu(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    u8 windowId;
    u8 windowId2;

    if (sBerryPouchStaticResources.type == BERRYPOUCH_BATTLE)
    {
        if (GetItemBattleUsage(gSpecialVar_ItemId))
        {
            sContextMenuOptions = sOptions_UseExit;
            sContextMenuNumOptions = ARRAY_COUNT(sOptions_UseExit);
        }
        else
        {
            sContextMenuOptions = sOptions_Exit;
            sContextMenuNumOptions = ARRAY_COUNT(sOptions_Exit);
        }
    }
    else if (sBerryPouchStaticResources.type == BERRYPOUCH_BERRY_BLENDER_CRUSH)
    {
        sContextMenuOptions = sOptions_ConfirmCheckExit;
        sContextMenuNumOptions = ARRAY_COUNT(sOptions_ConfirmCheckExit);
    }
    else if (MenuHelpers_IsLinkActive() == TRUE || InUnionRoom() == TRUE)
    {
        sContextMenuOptions = sOptions_GiveExit;
        sContextMenuNumOptions = ARRAY_COUNT(sOptions_GiveExit);
    }
    else
    {
        sContextMenuOptions = sOptions_UseGiveTossExit;
        sContextMenuNumOptions = ARRAY_COUNT(sOptions_UseGiveTossExit);
    }
    windowId = GetOrCreateVariableWindow(sContextMenuNumOptions + BP_VAR_WINDOW_THREW_AWAY);
    PrintMenuActionTexts(windowId, FONT_SHORT, GetMenuCursorDimensionByFont(FONT_SHORT, 0), 2, GetFontAttribute(FONT_SHORT, FONTATTR_LETTER_SPACING), GetFontAttribute(FONT_SHORT, FONTATTR_MAX_LETTER_HEIGHT) + 2, sContextMenuNumOptions, sContextMenuActions, sContextMenuOptions);
    InitMenuNormal(windowId, FONT_SHORT, 0, 2, GetFontAttribute(FONT_SHORT, FONTATTR_MAX_LETTER_HEIGHT) + 2, sContextMenuNumOptions, 0);
    windowId2 = GetOrCreateVariableWindow(BP_VAR_WINDOW_BERRY_SELECTED);
    CopySelectedListMenuItemName(tListPosition, gStringVar1);
    StringExpandPlaceholders(gStringVar4, gText_Var1IsSelected);
    BerryPouchPrint(windowId2, FONT_SHORT, gStringVar4, 0, 2, 1, 2, 0, BP_COLORID_TEXT);
}

static void CreateSortContextMenu(u8 taskId)
{
    u8 windowId;
    u8 windowId2;

    sContextMenuOptions = sOptions_NameAmountIndexExit;
    sContextMenuNumOptions = ARRAY_COUNT(sOptions_NameAmountIndexExit);

    windowId = GetOrCreateVariableWindow(sContextMenuNumOptions + BP_VAR_WINDOW_THREW_AWAY);
    PrintMenuActionTexts(windowId, FONT_SHORT, GetMenuCursorDimensionByFont(FONT_SHORT, 0), 2, GetFontAttribute(FONT_SHORT, FONTATTR_LETTER_SPACING), GetFontAttribute(FONT_SHORT, FONTATTR_MAX_LETTER_HEIGHT) + 2, sContextMenuNumOptions, sContextMenuActions, sContextMenuOptions);
    InitMenuNormal(windowId, FONT_SHORT, 0, 2, GetFontAttribute(FONT_SHORT, FONTATTR_MAX_LETTER_HEIGHT) + 2, sContextMenuNumOptions, 0);
    windowId2 = GetOrCreateVariableWindow(BP_VAR_WINDOW_BERRY_SELECTED);
    BerryPouchPrint(windowId2, FONT_SHORT, sText_SortItemsHow, 0, 2, 1, 2, 0, BP_COLORID_TEXT);
}

static void Task_NormalContextMenu(u8 taskId)
{
    CreateNormalContextMenu(taskId);
    gTasks[taskId].func = Task_NormalContextMenu_HandleInput;
}

static void Task_SortContextMenu(u8 taskId)
{
    CreateSortContextMenu(taskId);
    gTasks[taskId].func = Task_NormalContextMenu_HandleInput;
}

static void Task_NormalContextMenu_HandleInput(u8 taskId)
{
    s32 input;
    if (MenuHelpers_ShouldWaitForLinkRecv() != TRUE)
    {
        input = Menu_ProcessInputNoWrap();
        switch (input)
        {
        case MENU_NOTHING_CHOSEN:
            break;
        case MENU_B_PRESSED:
            PlaySE(SE_SELECT);
            sContextMenuActions[BP_ACTION_EXIT].func.void_u8(taskId);
            break;
        default:
            PlaySE(SE_SELECT);
            sContextMenuActions[sContextMenuOptions[input]].func.void_u8(taskId);
            break;
        }
    }
}

static void BerryPouch_RemoveUsedItem(void)
{
    RemoveBagItem(gSpecialVar_ItemId, 1);
    CopyItemName(gSpecialVar_ItemId, gStringVar2);
    StringExpandPlaceholders(gStringVar4, gText_PlayerUsedVar2);
    UpdatePocketItemList(GetItemPocket(gSpecialVar_ItemId));
    UpdatePocketListPosition(GetItemPocket(gSpecialVar_ItemId));
}

static void BerryPouch_BattleUse_BagMenu(u8 taskId)
{
    if (CannotUseItemsInBattle(gSpecialVar_ItemId, NULL))
    {
        DisplayItemMessageInBerryPouch(taskId, FONT_SHORT, gStringVar4, Task_BerryPouch_DestroyDialogueWindowAndRefreshListMenu);
    }
    else
    {
        PlaySE(SE_SELECT);
        if (!GetItemImportance(gSpecialVar_ItemId) && !(B_TRY_CATCH_TRAINER_BALL >= GEN_4 && (GetItemBattleUsage(gSpecialVar_ItemId) == EFFECT_ITEM_THROW_BALL) && (gBattleTypeFlags & BATTLE_TYPE_TRAINER)))
            BerryPouch_RemoveUsedItem();
        sIsInBerryPouch = FALSE;
        sBerryPouchDynamicResources->exitCallback = CB2_SetUpReshowBattleScreenAfterMenu2;
        gTasks[taskId].func = BerryPouch_StartFadeToExitCallback;
    }
}

static void BerryPouch_BattleUse_ShowPartyMenu(u8 taskId)
{
    sIsInBerryPouch = FALSE;
    sBerryPouchDynamicResources->exitCallback = ChooseMonForInBattleItem_BerryPouch;
    gTasks[taskId].func = BerryPouch_StartFadeToExitCallback;
}

static void BerryPouch_BattleUse_PartyMenu(u8 taskId)
{
    gItemUseCB = ItemUseCB_BattleScript;
    BerryPouch_BattleUse_ShowPartyMenu(taskId);
}

static void BerryPouch_BattleUse_PartyMenuChooseMove(u8 taskId)
{
    gItemUseCB = ItemUseCB_BattleChooseMove;
    BerryPouch_BattleUse_ShowPartyMenu(taskId);
}

static void Task_BerryPouch_Use(u8 taskId)
{
    DestroyVariableWindow(sContextMenuNumOptions + BP_VAR_WINDOW_THREW_AWAY);
    DestroyVariableWindow(BP_VAR_WINDOW_BERRY_SELECTED);
    PutWindowTilemap(BP_WINDOW_BERRY_LIST);
    PutWindowTilemap(BP_WINDOW_DESCRIPTION);
    ScheduleBgCopyTilemapToVram(0);
    ScheduleBgCopyTilemapToVram(2);
    if (sBerryPouchStaticResources.type == BERRYPOUCH_BATTLE)
    {
        // Safety check
        u16 type = GetItemType(gSpecialVar_ItemId);
        if (!GetItemBattleUsage(gSpecialVar_ItemId))
            return;

        if (type == ITEM_USE_BAG_MENU)
            BerryPouch_BattleUse_BagMenu(taskId);
        else if (type == ITEM_USE_PARTY_MENU)
            BerryPouch_BattleUse_PartyMenu(taskId);
        else if (type == ITEM_USE_PARTY_MENU_MOVES)
            BerryPouch_BattleUse_PartyMenuChooseMove(taskId);
    }
    else if (CalculatePlayerPartyCount() == 0 && GetItemType(gSpecialVar_ItemId) == ITEM_USE_PARTY_MENU)
    {
        Task_Give_PrintThereIsNoPokemon(taskId);
    }
    else
    {
        GetItemFieldFunc(gSpecialVar_ItemId)(taskId);
    }
}

static void Task_BerryPouch_Toss(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    ClearWindowTilemap(GetVariableWindowId(sContextMenuNumOptions + BP_VAR_WINDOW_THREW_AWAY));
    ClearWindowTilemap(GetVariableWindowId(BP_VAR_WINDOW_BERRY_SELECTED));
    DestroyVariableWindow(sContextMenuNumOptions + BP_VAR_WINDOW_THREW_AWAY);
    DestroyVariableWindow(BP_VAR_WINDOW_BERRY_SELECTED);
    PutWindowTilemap(BP_WINDOW_BERRY_LIST);
    tItemCount = 1;
    if (tQuantity == 1)
    {
        Task_AskTossMultiple(taskId);
    }
    else
    {
        InitTossQuantitySelectUI(taskId, sText_TossOutHowManyStrVar1s);
        CreateScrollIndicatorArrows_TossQuantity();
        gTasks[taskId].func = Task_Toss_SelectMultiple;
    }
}

static void Task_AskTossMultiple(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    ConvertIntToDecimalStringN(gStringVar2, tItemCount, STR_CONV_MODE_LEFT_ALIGN, MAX_ITEM_DIGITS);
    StringExpandPlaceholders(gStringVar4, sText_ThrowAwayStrVar2OfThisItemQM);
    BerryPouchPrint(GetOrCreateVariableWindow(BP_VAR_WINDOW_TOSS_ASK), FONT_SHORT, gStringVar4, 0, 2, 1, 2, 0, BP_COLORID_TEXT);
    CreateTossYesNoMenu(taskId, &sYesNoFuncs_Toss);
}

static void Task_TossNo(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    DestroyVariableWindow(BP_VAR_WINDOW_TOSS_ASK);
    PutWindowTilemap(BP_WINDOW_DESCRIPTION);
    PutWindowTilemap(BP_WINDOW_BERRY_LIST);
    ScheduleBgCopyTilemapToVram(0);
    ScheduleBgCopyTilemapToVram(2);
    BerryPouchSetArrowCursorFromListMenu(tListTaskId, BP_COLORID_DARK_GARY);
    Task_CleanUpAndReturnToMain(taskId);
}

static void Task_Toss_SelectMultiple(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    if (AdjustQuantityAccordingToDPadInput(&tItemCount, tQuantity) == TRUE)
    {
        PrintxQuantityOnWindow(BP_VAR_WINDOW_TOSS_AMOUNT, tItemCount, MAX_ITEM_DIGITS);
    }
    else if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        ClearWindowTilemap(GetVariableWindowId(BP_VAR_WINDOW_TOSS_SELECT));
        DestroyVariableWindow(BP_VAR_WINDOW_TOSS_SELECT);
        DestroyVariableWindow(BP_VAR_WINDOW_TOSS_AMOUNT);
        ScheduleBgCopyTilemapToVram(0);
        ScheduleBgCopyTilemapToVram(2);
        DestroyScrollIndicatorArrows();
        Task_AskTossMultiple(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        DestroyVariableWindow(BP_VAR_WINDOW_TOSS_SELECT);
        DestroyVariableWindow(BP_VAR_WINDOW_TOSS_AMOUNT);
        PutWindowTilemap(BP_WINDOW_BERRY_LIST);
        PutWindowTilemap(BP_WINDOW_DESCRIPTION);
        ScheduleBgCopyTilemapToVram(0);
        ScheduleBgCopyTilemapToVram(2);
        BerryPouchSetArrowCursorFromListMenu(tListTaskId, BP_COLORID_DARK_GARY);
        DestroyScrollIndicatorArrows();
        Task_CleanUpAndReturnToMain(taskId);
    }
}

static void Task_TossYes(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    DestroyVariableWindow(BP_VAR_WINDOW_TOSS_ASK);
    CopyItemNameHandlePlural(GetBerryPouchItemIdByPosition(tListPosition), gStringVar1, tItemCount);
    ConvertIntToDecimalStringN(gStringVar2, tItemCount, STR_CONV_MODE_LEFT_ALIGN, MAX_ITEM_DIGITS);
    StringExpandPlaceholders(gStringVar4, gText_ThrewAwayVar2Var1s);
    BerryPouchPrint(GetOrCreateVariableWindow(BP_VAR_WINDOW_THREW_AWAY), FONT_SHORT, gStringVar4, 0, 2, 1, 2, 0, BP_COLORID_TEXT);
    gTasks[taskId].func = Task_WaitButtonThenTossBerries;
}

static void Task_WaitButtonThenTossBerries(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        RemoveBagItem(gSpecialVar_ItemId, tItemCount);
        DestroyVariableWindow(BP_VAR_WINDOW_THREW_AWAY);
        DestroyListMenuTask(tListTaskId, &sBerryPouchStaticResources.listMenuScrollOffset, &sBerryPouchStaticResources.listMenuSelectedRow);
        SortAndCountBerries();
        SanitizeListMenuSelectionParams();
        SetUpListMenuTemplate();
        tListTaskId = ListMenuInit(&gMultiuseListMenuTemplate, sBerryPouchStaticResources.listMenuScrollOffset, sBerryPouchStaticResources.listMenuSelectedRow);
        PutWindowTilemap(BP_WINDOW_DESCRIPTION);
        ScheduleBgCopyTilemapToVram(0);
        BerryPouchSetArrowCursorFromListMenu(tListTaskId, BP_COLORID_DARK_GARY);
        Task_CleanUpAndReturnToMain(taskId);
    }
}

static void Task_BerryPouch_Give(u8 taskId)
{
    DestroyVariableWindow(sContextMenuNumOptions + BP_VAR_WINDOW_THREW_AWAY);
    DestroyVariableWindow(BP_VAR_WINDOW_BERRY_SELECTED);
    PutWindowTilemap(BP_WINDOW_BERRY_LIST);
    PutWindowTilemap(BP_WINDOW_DESCRIPTION);
    ScheduleBgCopyTilemapToVram(0);
    ScheduleBgCopyTilemapToVram(2);
    if (CalculatePlayerPartyCount() == 0)
    {
        Task_Give_PrintThereIsNoPokemon(taskId);
    }
    else
    {
        sBerryPouchDynamicResources->exitCallback = CB2_ChooseMonToGiveItem;
        gTasks[taskId].func = BerryPouch_StartFadeToExitCallback;
    }
}

static void Task_Give_PrintThereIsNoPokemon(u8 taskId)
{
    DisplayItemMessageInBerryPouch(taskId, FONT_SHORT, gText_NoPokemon, Task_WaitButtonBeforeDialogueWindowDestruction);
}

static void Task_WaitButtonBeforeDialogueWindowDestruction(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        Task_BerryPouch_DestroyDialogueWindowAndRefreshListMenu(taskId);
    }
}

void Task_BerryPouch_DestroyDialogueWindowAndRefreshListMenu(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    TryDestroyVariableWindow(BP_VAR_WINDOW_MESSAGE);
    DestroyListMenuTask(tListTaskId, &sBerryPouchStaticResources.listMenuScrollOffset, &sBerryPouchStaticResources.listMenuSelectedRow);
    SortAndCountBerries();
    SanitizeListMenuSelectionParams();
    SetUpListMenuTemplate();
    tListTaskId = ListMenuInit(&gMultiuseListMenuTemplate, sBerryPouchStaticResources.listMenuScrollOffset, sBerryPouchStaticResources.listMenuSelectedRow);
    ScheduleBgCopyTilemapToVram(0);
    BerryPouchSetArrowCursorFromListMenu(tListTaskId, BP_COLORID_DARK_GARY);
    Task_CleanUpAndReturnToMain(taskId);
}

static void Task_BerryPouch_Exit(u8 taskId)
{
    DestroyVariableWindow(sContextMenuNumOptions + BP_VAR_WINDOW_THREW_AWAY);
    DestroyVariableWindow(BP_VAR_WINDOW_BERRY_SELECTED);
    PutWindowTilemap(BP_WINDOW_BERRY_LIST);
    PutWindowTilemap(BP_WINDOW_DESCRIPTION);
    ScheduleBgCopyTilemapToVram(0);
    ScheduleBgCopyTilemapToVram(2);
    BerryPouchSetArrowCursorFromListMenu(gTasks[taskId].tListTaskId, BP_COLORID_DARK_GARY);
    Task_CleanUpAndReturnToMain(taskId);
}

#define tSortType data[3]
static void Task_BerryPouch_Name(u8 taskId)
{
    gTasks[taskId].tSortType = SORT_ALPHABETICALLY;
    gTasks[taskId].func = Task_BerryPouchSortItems;
}

static void Task_BerryPouch_Amount(u8 taskId)
{
    gTasks[taskId].tSortType = SORT_BY_AMOUNT;
    gTasks[taskId].func = Task_BerryPouchSortItems;
}

static void Task_BerryPouch_Index(u8 taskId)
{
    gTasks[taskId].tSortType = SORT_BY_INDEX;
    gTasks[taskId].func = Task_BerryPouchSortItems;
}

static void Task_BerryPouchSortItems(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    DestroyVariableWindow(sContextMenuNumOptions + BP_VAR_WINDOW_THREW_AWAY);
    DestroyVariableWindow(BP_VAR_WINDOW_BERRY_SELECTED);
    PutWindowTilemap(BP_WINDOW_BERRY_LIST);
    PutWindowTilemap(BP_WINDOW_DESCRIPTION);
    ScheduleBgCopyTilemapToVram(2);

    SortItemsInBag(&gBagPockets[POCKET_BERRIES], tSortType);
    DestroyListMenuTask(tListTaskId, &sBerryPouchStaticResources.listMenuScrollOffset, &sBerryPouchStaticResources.listMenuSelectedRow);
    SortAndCountBerries();
    SanitizeListMenuSelectionParams();
    SetUpListMenuTemplate();
    tListTaskId = ListMenuInit(&gMultiuseListMenuTemplate, sBerryPouchStaticResources.listMenuScrollOffset, sBerryPouchStaticResources.listMenuSelectedRow);
    ScheduleBgCopyTilemapToVram(0);

    StringCopy(gStringVar1, sSortTypeStrings[tSortType]);
    StringExpandPlaceholders(gStringVar4, sText_ItemsSorted);
    DisplayItemMessageInBerryPouch(taskId, FONT_SHORT, gStringVar4, Task_WaitButtonBeforeDialogueWindowDestruction);
}

#undef tSortType

static void Task_BerryPouch_CheckTag(u8 taskId)
{
    DestroyVariableWindow(sContextMenuNumOptions + BP_VAR_WINDOW_THREW_AWAY);
    DestroyVariableWindow(BP_VAR_WINDOW_BERRY_SELECTED);
    PutWindowTilemap(BP_WINDOW_BERRY_LIST);
    PutWindowTilemap(BP_WINDOW_DESCRIPTION);
    ScheduleBgCopyTilemapToVram(0);
    ScheduleBgCopyTilemapToVram(2);
    sBerryPouchDynamicResources->exitCallback = DoBerryTagScreenFromPouch;
    gTasks[taskId].func = BerryPouch_StartFadeToExitCallback;
}

static void Task_BerryPouch_Confirm(u8 taskId)
{
    DestroyVariableWindow(sContextMenuNumOptions + BP_VAR_WINDOW_THREW_AWAY);
    DestroyVariableWindow(BP_VAR_WINDOW_BERRY_SELECTED);
    PutWindowTilemap(BP_WINDOW_BERRY_LIST);
    PutWindowTilemap(BP_WINDOW_DESCRIPTION);
    ScheduleBgCopyTilemapToVram(0);
    ScheduleBgCopyTilemapToVram(2);
    sIsInBerryPouch = FALSE;
    gTasks[taskId].func = BerryPouch_StartFadeToExitCallback;
}

static void Task_ContextMenu_FromPartyGiveMenu(u8 taskId)
{
    sIsInBerryPouch = FALSE;
    sBerryPouchDynamicResources->exitCallback = CB2_GiveHoldItem;
    gTasks[taskId].func = BerryPouch_StartFadeToExitCallback;
}

static void Task_ContextMenu_FromPokemonPC(u8 taskId)
{
    sIsInBerryPouch = FALSE;
    sBerryPouchDynamicResources->exitCallback = CB2_ReturnToPokeStorage;
    gTasks[taskId].func = BerryPouch_StartFadeToExitCallback;
}

static void Task_ContextMenu_Sell(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    if (GetItemPrice(gSpecialVar_ItemId) == 0 || GetItemImportance(gSpecialVar_ItemId))
    {
        CopyItemName(gSpecialVar_ItemId, gStringVar2);
        StringExpandPlaceholders(gStringVar4, gText_CantBuyKeyItem);
        DisplayItemMessageInBerryPouch(taskId, FONT_SHORT, gStringVar4, Task_BerryPouch_DestroyDialogueWindowAndRefreshListMenu);
    }
    else
    {
        tItemCount = 1;
        if (tQuantity == 1)
        {
            PrintMoneyInWin2();
            Task_AskSellMultiple(taskId);
        }
        else
        {
            u32 maxQuantity = MAX_MONEY / GetItemSellPrice(gSpecialVar_ItemId);

            if (tQuantity > maxQuantity)
                tQuantity = maxQuantity;

            CopyItemName(gSpecialVar_ItemId, gStringVar2);
            StringExpandPlaceholders(gStringVar4, gText_HowManyToSell);
            DisplayItemMessageInBerryPouch(taskId, FONT_SHORT, gStringVar4, Task_Sell_PrintSelectMultipleUI);
        }
    }
}

static void Task_AskSellMultiple(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    ConvertIntToDecimalStringN(gStringVar1, GetItemSellPrice(GetBerryPouchItemIdByPosition(tListPosition)) * tItemCount, STR_CONV_MODE_LEFT_ALIGN, MAX_MONEY_DIGITS);
    StringExpandPlaceholders(gStringVar4, gText_ICanPayVar1);
    DisplayItemMessageInBerryPouch(taskId, FONT_SHORT, gStringVar4, Task_SellMultiple_CreateYesNoMenu);
}

static void Task_SellMultiple_CreateYesNoMenu(u8 taskId)
{
    CreateSellYesNoMenu(taskId, &sYesNoFuncs_Sell);
}

static void Task_SellNo(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    DestroyVariableWindow(BP_VAR_WINDOW_MONEY);
    TryDestroyVariableWindow(BP_VAR_WINDOW_MESSAGE);
    PutWindowTilemap(BP_WINDOW_POUCH_HEADER);
    PutWindowTilemap(BP_WINDOW_BERRY_LIST);
    PutWindowTilemap(BP_WINDOW_DESCRIPTION);
    RemoveMoneyLabelObject();
    ScheduleBgCopyTilemapToVram(0);
    BerryPouchSetArrowCursorFromListMenu(tListTaskId, BP_COLORID_DARK_GARY);
    Task_CleanUpAndReturnToMain(taskId);
}

static void Task_Sell_PrintSelectMultipleUI(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    u8 windowId = GetOrCreateVariableWindow(BP_VAR_WINDOW_SELL_AMOUNT);
    ConvertIntToDecimalStringN(gStringVar1, 1, STR_CONV_MODE_LEADING_ZEROS, MAX_ITEM_DIGITS);
    StringExpandPlaceholders(gStringVar4, gText_xVar1);
    BerryPouchPrint(windowId, FONT_SMALL, gStringVar4, 4, 10, 1, 0, TEXT_SKIP_DRAW, BP_COLORID_TEXT);
    SellMultiple_UpdateSellPriceDisplay(GetItemSellPrice(GetBerryPouchItemIdByPosition(tListPosition)) * tItemCount);
    PrintMoneyInWin2();
    CreateScrollIndicatorArrows_SellQuantity();
    gTasks[taskId].func = Task_Sell_SelectMultiple;
}

static void SellMultiple_UpdateSellPriceDisplay(s32 price)
{
    PrintMoneyAmount(GetVariableWindowId(BP_VAR_WINDOW_SELL_AMOUNT), 40, 10, price, 0);
}

static void Task_Sell_SelectMultiple(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    if (AdjustQuantityAccordingToDPadInput(&tItemCount, tQuantity) == TRUE)
    {
        PrintxQuantityOnWindow(BP_VAR_WINDOW_SELL_AMOUNT, tItemCount, MAX_ITEM_DIGITS);
        SellMultiple_UpdateSellPriceDisplay(GetItemSellPrice(GetBerryPouchItemIdByPosition(tListPosition)) * tItemCount);
    }
    else if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        DestroyVariableWindow(BP_VAR_WINDOW_SELL_AMOUNT);
        PutWindowTilemap(BP_WINDOW_BERRY_LIST);
        ScheduleBgCopyTilemapToVram(0);
        DestroyScrollIndicatorArrows();
        Task_AskSellMultiple(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        DestroyVariableWindow(BP_VAR_WINDOW_SELL_AMOUNT);
        DestroyVariableWindow(BP_VAR_WINDOW_MONEY);
        TryDestroyVariableWindow(BP_VAR_WINDOW_MESSAGE);
        PutWindowTilemap(BP_WINDOW_POUCH_HEADER);
        PutWindowTilemap(BP_WINDOW_BERRY_LIST);
        PutWindowTilemap(BP_WINDOW_DESCRIPTION);
        RemoveMoneyLabelObject();
        ScheduleBgCopyTilemapToVram(0);
        DestroyScrollIndicatorArrows();
        BerryPouchSetArrowCursorFromListMenu(tListTaskId, BP_COLORID_DARK_GARY);
        Task_CleanUpAndReturnToMain(taskId);
    }
}

static void Task_SellYes(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    PutWindowTilemap(BP_WINDOW_BERRY_LIST);
    ScheduleBgCopyTilemapToVram(0);
    CopyItemName(gSpecialVar_ItemId, gStringVar2);
    ConvertIntToDecimalStringN(gStringVar1, GetItemSellPrice(GetBerryPouchItemIdByPosition(tListPosition)) * tItemCount, STR_CONV_MODE_LEFT_ALIGN, MAX_MONEY_DIGITS);
    StringExpandPlaceholders(gStringVar4, gText_TurnedOverVar1ForVar2);
    DisplayItemMessageInBerryPouch(taskId, FONT_SHORT, gStringVar4, Task_SellBerries_PlaySfxAndRemoveBerries);
}

static void Task_SellBerries_PlaySfxAndRemoveBerries(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    PlaySE(SE_RG_SHOP);
    RemoveBagItem(gSpecialVar_ItemId, tItemCount);
    AddMoney(&gSaveBlock1Ptr->money, GetItemSellPrice(gSpecialVar_ItemId) * tItemCount);
    DestroyListMenuTask(tListTaskId, &sBerryPouchStaticResources.listMenuScrollOffset, &sBerryPouchStaticResources.listMenuSelectedRow);
    SortAndCountBerries();
    SanitizeListMenuSelectionParams();
    SetUpListMenuTemplate();
    tListTaskId = ListMenuInit(&gMultiuseListMenuTemplate, sBerryPouchStaticResources.listMenuScrollOffset, sBerryPouchStaticResources.listMenuSelectedRow);
    BerryPouchSetArrowCursorFromListMenu(tListTaskId, BP_COLORID_LIGHT_GRAY);
    PrintMoneyAmountInMoneyBox(GetVariableWindowId(BP_VAR_WINDOW_MONEY), GetMoney(&gSaveBlock1Ptr->money), 0);
    gTasks[taskId].func = Task_SellBerries_WaitButton;
}

static void Task_SellBerries_WaitButton(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        DestroyVariableWindow(BP_VAR_WINDOW_MONEY);
        RemoveMoneyLabelObject();
        PutWindowTilemap(BP_WINDOW_POUCH_HEADER);
        Task_BerryPouch_DestroyDialogueWindowAndRefreshListMenu(taskId);
    }
}

static void BerryPouchInitWindows(void)
{
    u8 i;
    InitWindows(sWindowTemplates_Main);
    DeactivateAllTextPrinters();
    LoadUserWindowBorderGfx(BP_WINDOW_BERRY_LIST, 0x001, BG_PLTT_ID(14));
    LoadMessageBoxGfx(BP_WINDOW_BERRY_LIST, 0x013, BG_PLTT_ID(13));
    LoadPalette(gStandardMenuPalette, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
    for (i = BP_WINDOW_BERRY_LIST; i < BP_WINDOW_COUNT; i++)
        FillWindowPixelBuffer(i, PIXEL_FILL(0));
    PutWindowTilemap(BP_WINDOW_BERRY_LIST);
    PutWindowTilemap(BP_WINDOW_DESCRIPTION);
    PutWindowTilemap(BP_WINDOW_POUCH_HEADER);
    ScheduleBgCopyTilemapToVram(0);
    ScheduleBgCopyTilemapToVram(2);
    for (i = BP_VAR_WINDOW_TOSS_AMOUNT; i < BP_VAR_WINDOW_COUNT; i++)
        sVariableWindowIds[i] = WINDOW_NONE;
}

static void BerryPouchPrint(u8 windowId, u8 fontId, const u8 * str, u8 x, u8 y, u8 letterSpacing, u8 lineSpacing, u8 speed, u8 colorIdx)
{
    AddTextPrinterParameterized4(windowId, fontId, x, y, letterSpacing, lineSpacing, sTextColors[colorIdx], speed, str);
}

static u8 GetOrCreateVariableWindow(u8 winIdx)
{
    u8 retval = sVariableWindowIds[winIdx];
    if (retval == WINDOW_NONE)
    {
        sVariableWindowIds[winIdx] = AddWindow(&sWindowTemplates_Variable[winIdx]);
        DrawStdFrameWithCustomTileAndPalette(sVariableWindowIds[winIdx], FALSE, 0x001, 14);
        ScheduleBgCopyTilemapToVram(2);
        retval = sVariableWindowIds[winIdx];
    }
    return retval;
}

static void DestroyVariableWindow(u8 winIdx)
{
    ClearStdWindowAndFrameToTransparent(sVariableWindowIds[winIdx], FALSE);
    ClearWindowTilemap(sVariableWindowIds[winIdx]);
    RemoveWindow(sVariableWindowIds[winIdx]);
    ScheduleBgCopyTilemapToVram(2);
    sVariableWindowIds[winIdx] = WINDOW_NONE;
}

static void TryDestroyVariableWindow(u8 winIdx)
{
    if (sVariableWindowIds[winIdx] != WINDOW_NONE)
    {
        ClearDialogWindowAndFrameToTransparent(sVariableWindowIds[winIdx], FALSE);
        ClearWindowTilemap(sVariableWindowIds[winIdx]);
        RemoveWindow(sVariableWindowIds[winIdx]);
        PutWindowTilemap(BP_WINDOW_DESCRIPTION);
        ScheduleBgCopyTilemapToVram(0);
        ScheduleBgCopyTilemapToVram(2);
        sVariableWindowIds[winIdx] = WINDOW_NONE;
    }
}

static u8 GetVariableWindowId(u8 winIdx)
{
    return sVariableWindowIds[winIdx];
}

void DisplayItemMessageInBerryPouch(u8 taskId, u8 fontId, const u8 * str, TaskFunc followUpFunc)
{
    if (sVariableWindowIds[BP_VAR_WINDOW_MESSAGE] == WINDOW_NONE)
        sVariableWindowIds[BP_VAR_WINDOW_MESSAGE] = AddWindow(&sWindowTemplates_Variable[BP_VAR_WINDOW_MESSAGE]);
    DisplayMessageAndContinueTask(taskId, sVariableWindowIds[BP_VAR_WINDOW_MESSAGE], 0x013, 13, fontId, GetPlayerTextSpeedDelay(), str, followUpFunc);
    ScheduleBgCopyTilemapToVram(2);
}

static void CreateTossYesNoMenu(u8 taskId, const struct YesNoFuncTable *ptrs)
{
    CreateYesNoMenuWithCallbacks(taskId, &sWindowTemplates_Variable[BP_VAR_WINDOW_TOSS_YESNO], FONT_SHORT, 0, 2, 0x001, 14, ptrs);
}

static void CreateSellYesNoMenu(u8 taskId, const struct YesNoFuncTable *ptrs)
{
    CreateYesNoMenuWithCallbacks(taskId, &sWindowTemplates_Variable[BP_VAR_WINDOW_SELL_YESNO], FONT_SHORT, 0, 2, 0x001, 14, ptrs);
}

static void PrintMoneyInWin2(void)
{
    PrintMoneyAmountInMoneyBoxWithBorder(GetOrCreateVariableWindow(BP_VAR_WINDOW_MONEY), 0x001, 14, GetMoney(&gSaveBlock1Ptr->money));
    AddMoneyLabelObject(19, 11);
}

static void CreateBerryPouchSprite(void)
{
    sBerryPouchSpriteId = CreateSprite(&sSpriteTemplate_BerryPouch, 40, 76, 0);
}

static void StartBerryPouchSpriteWobbleAnim(void)
{
    struct Sprite *sprite = &gSprites[sBerryPouchSpriteId];
    if (sprite->affineAnimEnded)
    {
        StartSpriteAffineAnim(sprite, 1);
        sprite->callback = SpriteCB_BerryPouchWaitWobbleAnim;
    }
}

static void SpriteCB_BerryPouchWaitWobbleAnim(struct Sprite *sprite)
{
    if (sprite->affineAnimEnded)
    {
        StartSpriteAffineAnim(sprite, 0);
        sprite->callback = SpriteCallbackDummy;
    }
}

bool32 CheckIfInBerryPouch(void)
{
    return sIsInBerryPouch;
}

#undef tListTaskId
#undef tPosition
#undef tQuantity
#undef tItemCount
