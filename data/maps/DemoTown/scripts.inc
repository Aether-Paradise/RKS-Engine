DemoTown_MapScripts::
	.byte 0

DemoTown_EventScript_Trader::
	lock
	faceplayer
	goto_if_set FLAG_DEMO_INGAME_TRADE, DemoTown_EventScript_TradeCompleted
	ingame_trade INGAME_TRADE_DEMO, DemoTown_Text_LookingForTrade, DemoTown_EventScript_DoYouWantARalts, DemoTown_EventScript_NotRequestedMon, DemoTown_Text_ThankYouForTheTrade
	setflag FLAG_DEMO_INGAME_TRADE
	release
	end

DemoTown_EventScript_DoYouWantARalts::
	msgbox DemoTown_Text_DoYouWantARalts, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, DemoTown_EventScript_GiveRalts
	goto_if_eq VAR_RESULT, NO, DemoTown_EventScript_DeclineTrade
	end @unreachable

DemoTown_EventScript_GiveRalts::
	givemon SPECIES_RALTS, 5
	goto DemoTown_EventScript_Trader

DemoTown_EventScript_DeclineTrade::
	msgbox DemoTown_Text_CancelledTrade, MSGBOX_DEFAULT
	release
	end

DemoTown_EventScript_NotRequestedMon::
	bufferspeciesname STR_VAR_1, VAR_0x8009
	msgbox DemoTown_Text_NotRequestedMon, MSGBOX_DEFAULT
	release
	end

DemoTown_EventScript_TradeCompleted::
	msgbox DemoTown_Text_ReallyWantedToGetBagon, MSGBOX_DEFAULT
	release
	end

DemoTown_EventScript_EventScript_MirageIslandWatcher::
	lock
	faceplayer
	specialvar VAR_RESULT, IsMirageIslandPresent
	goto_if_eq VAR_RESULT, TRUE, DemoTown_EventScript_EventScript_MirageIslandPresent
	msgbox DemoTown_EventScript_Text_CantSeeMirageIslandToday, MSGBOX_DEFAULT
	release
	end

DemoTown_EventScript_EventScript_MirageIslandPresent::
	msgbox DemoTown_EventScript_Text_CanSeeMirageIslandToday, MSGBOX_DEFAULT
	release
	end

DemoTown_EventScript_FriendshipRater::
	lock
	faceplayer
	msgbox DemoTown_Text_SeeHowMuchPokemonLikesYou, MSGBOX_DEFAULT
	specialvar VAR_RESULT, GetLeadMonFriendshipScore
	switch VAR_RESULT
	case FRIENDSHIP_NONE, DemoTown_EventScript_DetestsYou
	case FRIENDSHIP_1_TO_49, DemoTown_EventScript_VeryWary
	case FRIENDSHIP_50_TO_99, DemoTown_EventScript_NotUsedToYou
	case FRIENDSHIP_100_TO_149, DemoTown_EventScript_GettingUsedToYou
	case FRIENDSHIP_150_TO_199, DemoTown_EventScript_LikesYouQuiteALot
	case FRIENDSHIP_200_TO_254, DemoTown_EventScript_VeryHappy
	case FRIENDSHIP_MAX, DemoTown_EventScript_AdoresYou
	release
	end

DemoTown_EventScript_DetestsYou::
	msgbox DemoTown_Text_DetestsYou, MSGBOX_DEFAULT
	release
	end

DemoTown_EventScript_VeryWary::
	msgbox DemoTown_Text_VeryWary, MSGBOX_DEFAULT
	release
	end

DemoTown_EventScript_NotUsedToYou::
	msgbox DemoTown_Text_NotUsedToYou, MSGBOX_DEFAULT
	release
	end

DemoTown_EventScript_GettingUsedToYou::
	msgbox DemoTown_Text_GettingUsedToYou, MSGBOX_DEFAULT
	release
	end

DemoTown_EventScript_LikesYouQuiteALot::
	msgbox DemoTown_Text_LikesYouQuiteALot, MSGBOX_DEFAULT
	release
	end

DemoTown_EventScript_VeryHappy::
	msgbox DemoTown_Text_VeryHappy, MSGBOX_DEFAULT
	release
	end

DemoTown_EventScript_AdoresYou::
	msgbox DemoTown_Text_AdoresYou, MSGBOX_DEFAULT
	release
	end
