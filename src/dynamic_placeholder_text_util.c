#include "global.h"
#include "text.h"
#include "dynamic_placeholder_text_util.h"
#include "string_util.h"

static EWRAM_DATA const u8 *sStringPointers[8] = {};

static const u8 sTextColorTable[] =
{
    [OBJ_EVENT_GFX_RED_NORMAL]              = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_RED_BIKE]                = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_RED_SURF]                = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_RED_FIELD_MOVE]          = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_RED_FISH]                = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_RED_VS_SEEKER]           = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_RED_VS_SEEKER_BIKE]      = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_UNUSED_250]              = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_GREEN_NORMAL]            = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_GREEN_BIKE]              = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_GREEN_SURF]              = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_GREEN_FIELD_MOVE]        = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_GREEN_FISH]              = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_GREEN_VS_SEEKER]         = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_GREEN_VS_SEEKER_BIKE]    = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_BOY]                     = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_CRUSH_GIRL]              = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_MAN]                     = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_ROCKER]                  = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_BALDING_MAN]             = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_OLD_MAN_1]               = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_OLD_MAN_2]               = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_OLD_MAN_LYING_DOWN]      = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_TUBER_M_WATER]           = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_TUBER_M_LAND]            = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_COOLTRAINER_M]           = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_COOLTRAINER_F]           = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_SWIMMER_M_WATER]         = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_SWIMMER_F_WATER]         = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_SWIMMER_M_LAND]          = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_SWIMMER_F_LAND]          = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_WORKER_M]                = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_WORKER_F]                = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_ROCKET_M]                = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_ROCKET_F]                = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_GBA_KID]                 = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_POKE_MANIAC_FRLG]        = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_BIKER]                   = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_BLACK_BELT_FRLG]         = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_SCIENTIST]               = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_FISHER]                  = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_CHANNELER]               = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_CHEF]                    = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_POLICEMAN]               = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_CAPTAIN]                 = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_CABLE_CLUB_RECEPTIONIST] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_UNION_ROOM_RECEPTIONIST] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_CLERK]                   = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_MG_DELIVERYMAN]          = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_TRAINER_TOWER_DUDE]      = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_PROF_OAK]                = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_BLUE]                    = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_BILL]                    = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_LANCE]                   = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_AGATHA]                  = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_DAISY]                   = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_LORELEI]                 = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_MR_FUJI]                 = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_BRUNO]                   = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_BROCK]                   = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_MISTY]                   = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_LT_SURGE]                = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_ERIKA]                   = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_KOGA]                    = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_SABRINA]                 = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_BLAINE]                  = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_GIOVANNI]                = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_CELIO]                   = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_TEACHY_TV_HOST]          = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_GYM_GUY]                 = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_LITTLE_BOY_FRLG]         = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_LITTLE_GIRL_FRLG]        = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_YOUNGSTER_FRLG]          = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_BUG_CATCHER_FRLG]        = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_LASS_FRLG]               = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_WOMAN_1_FRLG]            = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_FAT_MAN_FRLG]            = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_WOMAN_2_FRLG]            = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_BEAUTY_FRLG]             = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_WOMAN_3_FRLG]            = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_OLD_WOMAN_FRLG]          = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_CAMPER_FRLG]             = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_PICNICKER_FRLG]          = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_MOM_FRLG]                = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_TUBER_F_FRLG]            = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_HIKER_FRLG]              = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_GENTLEMAN_FRLG]          = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_SAILOR_FRLG]             = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_NURSE_FRLG]              = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_LINK_RS_BRENDAN]         = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_LINK_RS_MAY]             = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_GAMEBOY_KID]             = NPC_TEXT_COLOR_MALE,
};

void DynamicPlaceholderTextUtil_Reset(void)
{
    const u8 **ptr;
    u8 *fillval;
    const u8 **ptr2;

    ptr = sStringPointers;
    fillval = NULL;
    ptr2 = ptr + (ARRAY_COUNT(sStringPointers) - 1);
    do
    {
        *ptr2-- = fillval;
    } while ((int)ptr2 >= (int)ptr);
}

void DynamicPlaceholderTextUtil_SetPlaceholderPtr(u8 idx, const u8 *ptr)
{
    if (idx < ARRAY_COUNT(sStringPointers))
    {
        sStringPointers[idx] = ptr;
    }
}

u8 *DynamicPlaceholderTextUtil_ExpandPlaceholders(u8 *dest, const u8 *src)
{
    while (*src != EOS)
    {
        if (*src != CHAR_DYNAMIC)
        {
            *dest++ = *src++;
        }
        else
        {
            src++;
            if (sStringPointers[*src] != NULL)
            {
                dest = StringCopy(dest, sStringPointers[*src]);
            }
            src++;
        }
    }
    *dest = EOS;
    return dest;
}

const u8 *DynamicPlaceholderTextUtil_GetPlaceholderPtr(u8 idx)
{
    return sStringPointers[idx];
}

u8 GetColorFromTextColorTable(u16 graphicId)
{
    if (graphicId >= ARRAY_COUNT(sTextColorTable))
        return NPC_TEXT_COLOR_NEUTRAL;
    else
        return sTextColorTable[graphicId];
}
