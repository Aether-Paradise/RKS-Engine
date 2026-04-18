DemoTown_MapScripts::
	.byte 0

DemoTown_EventScript_Trader::
	lock
	faceplayer
	goto_if_set FLAG_DEMO_INGAME_TRADE, DemoTown_EventScript_TradeCompleted
	ingame_trade INGAME_TRADE_DEMO, DemoTown_Text_LookingForTrade, DemoTown_EventScript_DeclineTrade, DemoTown_EventScript_NotRequestedMon, DemoTown_Text_ThankYouForTheTrade
	setflag FLAG_DEMO_INGAME_TRADE
	release
	end

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
