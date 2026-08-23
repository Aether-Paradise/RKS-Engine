#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetItemHoldEffect(ITEM_CORNERSTONE_MASK) == HOLD_EFFECT_OGERPON_MASK);
    ASSUME(GetItemHoldEffect(ITEM_WELLSPRING_MASK) == HOLD_EFFECT_OGERPON_MASK);
    ASSUME(GetItemHoldEffect(ITEM_HEARTHFLAME_MASK) == HOLD_EFFECT_OGERPON_MASK);
    ASSUME(GetItemHoldEffectParam(ITEM_CORNERSTONE_MASK) == 20);
    ASSUME(GetItemHoldEffectParam(ITEM_WELLSPRING_MASK) == 20);
    ASSUME(GetItemHoldEffectParam(ITEM_HEARTHFLAME_MASK) == 20);
}

SINGLE_BATTLE_TEST("Ogerpon Masks increase the base power of moves by 20%", s16 damage)
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_OGERPON_TEAL; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_OGERPON_WELLSPRING; item = ITEM_CORNERSTONE_MASK; }
    PARAMETRIZE { species = SPECIES_OGERPON_HEARTHFLAME; item = ITEM_WELLSPRING_MASK; }
    PARAMETRIZE { species = SPECIES_OGERPON_CORNERSTONE; item = ITEM_HEARTHFLAME_MASK; }

    GIVEN {
        ASSUME(GetMovePower(MOVE_SCRATCH) > 0);
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.2), results[1].damage);
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.2), results[2].damage);
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.2), results[3].damage);
    }
}
