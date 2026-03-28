DemoCave1F_MapScripts::
	.byte 0

DemoCave1F_EventScript_GalarianYamask_Sign::
	lockall
	msgbox DemoCave1F_Text_GalarianYamask_Sign
	releaseall
	end

DemoCave1F_GalarianYamask_Trigger::
	lockall
	tryspecialevo 0
	waitstate
	checkspecies SPECIES_YAMASK_GALAR
	goto_if_eq VAR_RESULT, TRUE, DemoCave1F_GalarianYamask_Trigger_StillYamask
	releaseall
	end

DemoCave1F_GalarianYamask_Trigger_StillYamask::
	msgbox DemoCave1F_Text_GalarianYamask_StillYamask
	release
	end

DemoCave1F_EventScript_Archie::
	lockall
	faceplayer
	msgbox DemoCave1F_Text_Archie
	releaseall
	end
