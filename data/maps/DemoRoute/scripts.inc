.equ PLAYER_STARTER_NUM,      VAR_TEMP_1
.equ PLAYER_STARTER_SPECIES,  VAR_TEMP_2

DemoRoute_MapScripts::
	.byte 0

DemoRoute_EventScript_Rowlet::
	lock
	faceplayer
	setvar PLAYER_STARTER_NUM, 0
	setvar PLAYER_STARTER_SPECIES, SPECIES_ROWLET
	goto DemoRoute_EventScript_ConfirmStarter

DemoRoute_EventScript_Cyndaquil::
	lock
	faceplayer
	setvar PLAYER_STARTER_NUM, 1
	setvar PLAYER_STARTER_SPECIES, SPECIES_CYNDAQUIL
	goto DemoRoute_EventScript_ConfirmStarter

DemoRoute_EventScript_Oshawott::
	lock
	faceplayer
	setvar PLAYER_STARTER_NUM, 2
	setvar PLAYER_STARTER_SPECIES, SPECIES_OSHAWOTT
	goto DemoRoute_EventScript_ConfirmStarter

DemoRoute_EventScript_ConfirmStarter::
	showmonpic PLAYER_STARTER_SPECIES, 10, 3
	textcolor NPC_TEXT_COLOR_MALE
	goto_if_set FLAG_DEMO_CHOSE_STARTER, DemoRoute_EventScript_AlreadyChoseStarter
	goto_if_eq PLAYER_STARTER_NUM, 0, DemoRoute_EventScript_ConfirmRowlet
	goto_if_eq PLAYER_STARTER_NUM, 1, DemoRoute_EventScript_ConfirmCyndaquil
	goto_if_eq PLAYER_STARTER_NUM, 2, DemoRoute_EventScript_ConfirmOshawott
	end

DemoRoute_EventScript_AlreadyChoseStarter::
	specialvar VAR_RESULT, GetStarterPokemon
	bufferspeciesname STR_VAR_1, VAR_RESULT
	bufferspeciesname STR_VAR_2, PLAYER_STARTER_SPECIES
	msgbox DemoRoute_Text_AlreadyChoseStarter
	hidemonpic
	release
	end

DemoRoute_EventScript_ConfirmRowlet::
	msgbox DemoRoute_Text_ChoosingRowlet, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, DemoRoute_EventScript_ChoseStarter
	goto_if_eq VAR_RESULT, NO, DemoRoute_EventScript_DeclinedStarter
	end

DemoRoute_EventScript_ConfirmOshawott::
	msgbox DemoRoute_Text_ChoosingOshawott, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, DemoRoute_EventScript_ChoseStarter
	goto_if_eq VAR_RESULT, NO, DemoRoute_EventScript_DeclinedStarter
	end

DemoRoute_EventScript_ConfirmCyndaquil::
	msgbox DemoRoute_Text_ChoosingCyndaquil, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, DemoRoute_EventScript_ChoseStarter
	goto_if_eq VAR_RESULT, NO, DemoRoute_EventScript_DeclinedStarter
	end

DemoRoute_EventScript_DeclinedStarter::
	hidemonpic
	release
	end

DemoRoute_EventScript_ChoseStarter::
	hidemonpic
	removeobject VAR_LAST_TALKED
	call EventScript_RestorePrevTextColor
	setflag FLAG_SYS_POKEMON_GET
	setflag FLAG_DEMO_CHOSE_STARTER
	givemon PLAYER_STARTER_SPECIES, 5
	copyvar VAR_STARTER_MON, PLAYER_STARTER_NUM
	bufferspeciesname STR_VAR_1, PLAYER_STARTER_SPECIES
	message DemoRoute_Text_ReceivedMon
	waitmessage
	playfanfare MUS_RG_OBTAIN_KEY_ITEM
	waitfanfare
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, DemoRoute_EventScript_GiveNicknameToStarter
	release
	end

DemoRoute_EventScript_GiveNicknameToStarter::
	setvar VAR_0x8004, 0
	call Common_EventScript_NameReceivedPartyMon
	release
	end
