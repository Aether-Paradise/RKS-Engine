DemoDeoxysEvent_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, DemoDeoxysEvent_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, DemoDeoxysEvent_OnResume
	map_script MAP_SCRIPT_ON_RETURN_TO_FIELD, DemoDeoxysEvent_OnReturnToField
	.byte 0

DemoDeoxysEvent_EventScript_DeoxysRockSign::
	lockall
	msgbox DemoDeoxysEvent_Text_DeoxysRockSign
	releaseall
	end

DemoDeoxysEvent_OnReturnToField:
	special SetDeoxysRockPalette
	end

DemoDeoxysEvent_OnTransition:
	setflag FLAG_MAP_SCRIPT_CHECKED_DEOXYS
	setvar VAR_OBJ_GFX_ID_0, OBJ_EVENT_GFX_RIVAL_BRENDAN_NORMAL
	setvar VAR_DEOXYS_ROCK_STEP_COUNT, 0
	setvar VAR_DEOXYS_ROCK_LEVEL, 0
	call_if_set FLAG_BATTLED_DEOXYS, DemoDeoxysEvent_EventScript_HideDeoxysAndPuzzle
	call_if_unset FLAG_BATTLED_DEOXYS, DemoDeoxysEvent_EventScript_TryShowDeoxysPuzzle
	end

DemoDeoxysEvent_EventScript_HideDeoxysAndPuzzle::
	setflag FLAG_DEMO_HIDE_DEOXYS
	setflag FLAG_DEMO_HIDE_DEOXYS_TRIANGLE
	return

DemoDeoxysEvent_EventScript_TryShowDeoxysPuzzle::
	goto_if_set FLAG_DEMO_HIDE_DEOXYS, Common_EventScript_NopReturn
	clearflag FLAG_DEMO_HIDE_DEOXYS_TRIANGLE
	clearflag FLAG_DEOXYS_ROCK_COMPLETE
	return

DemoDeoxysEvent_OnResume:
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, DemoDeoxysEvent_EventScript_TryRemoveDeoxys
	end

DemoDeoxysEvent_EventScript_TryRemoveDeoxys::
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_NopReturn
	removeobject LOCALID_DEMO_DEOXYS_EVENT_DEOXYS
	return

DemoDeoxysEvent_EventScript_Triangle::
	lock
	faceplayer
	special DoDeoxysRockInteraction
	switch VAR_RESULT
@ The actual rock triangle movement is handled by DoDeoxysRockInteraction.
@ Unless the player has solved the puzzle and needs to encounter Deoxys,
@ there's nothing else the script needs to do.
	case DEOXYS_ROCK_SOLVED,     DemoDeoxysEvent_EventScript_Deoxys
	release
	end

DemoDeoxysEvent_EventScript_Deoxys::
	waitse
	setfieldeffectargument 0, LOCALID_DEMO_DEOXYS_EVENT_ROCK
	setfieldeffectargument 1, MAP_NUM(MAP_DEMO_DEOXYS_EVENT)
	setfieldeffectargument 2, MAP_GROUP(MAP_DEMO_DEOXYS_EVENT)
	dofieldeffect FLDEFF_DESTROY_DEOXYS_ROCK
	playbgm MUS_RG_ENCOUNTER_DEOXYS, FALSE
	waitfieldeffect FLDEFF_DESTROY_DEOXYS_ROCK
	addobject LOCALID_DEMO_DEOXYS_EVENT_DEOXYS
	applymovement LOCALID_DEMO_DEOXYS_EVENT_DEOXYS, DemoDeoxysEvent_Movement_DeoxysApproach
	waitmovement 0
	waitse
	playmoncry SPECIES_DEOXYS_NORMAL, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_LAST_TALKED, LOCALID_DEMO_DEOXYS_EVENT_DEOXYS
	seteventmon SPECIES_DEOXYS_NORMAL, 30
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, DemoDeoxysEvent_EventScript_DefeatedDeoxys
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, DemoDeoxysEvent_EventScript_RanFromDeoxys
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, DemoDeoxysEvent_EventScript_RanFromDeoxys
	setflag FLAG_BATTLED_DEOXYS
	release
	end

DemoDeoxysEvent_EventScript_DefeatedDeoxys::
	setflag FLAG_DEMO_HIDE_DEOXYS
	setvar VAR_0x8004, SPECIES_DEOXYS_NORMAL
	goto Common_EventScript_LegendaryFlewAway
	end

DemoDeoxysEvent_EventScript_RanFromDeoxys::
	setvar VAR_0x8004, SPECIES_DEOXYS_NORMAL
	goto Common_EventScript_LegendaryFlewAway
	end

DemoDeoxysEvent_Movement_DeoxysApproach:
	walk_slow_down
	walk_slow_down
	walk_slow_down
	walk_slow_down
	walk_slow_down
	walk_slow_down
	walk_slow_down
	step_end
