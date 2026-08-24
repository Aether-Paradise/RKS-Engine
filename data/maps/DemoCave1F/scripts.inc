DemoCave1F_MapScripts::
	.byte 0

DemoCave1F_EventScript_GalarianYamask_Sign::
	lockall
	msgbox DemoCave1F_Text_GalarianYamask_Sign
	@showhelpwindow HELP_TRY_SPECIAL_EVO
	@waitstate
	releaseall
	end

DemoCave1F_GalarianYamask_Trigger::
	lockall
	tryspecialevo EVO_TRIGGER_TABLET_CURSE, canStopEvo=TRUE, tryMultiple=FALSE
	switch VAR_RESULT
	case EVO_EVENT_IMPOSSIBLE, DemoCave1F_GalarianYamask_Trigger_CouldntEvolve
	case EVO_EVENT_INTERRUPTED, DemoCave1F_GalarianYamask_Trigger_EvoInterrupted
	case EVO_EVENT_SUCCESSFUL, DemoCave1F_GalarianYamask_Trigger_EvoSuccess
	@ Unreachable
	releaseall
	end

DemoCave1F_GalarianYamask_Trigger_CouldntEvolve::
	checkspecies SPECIES_YAMASK_GALAR
	goto_if_eq VAR_RESULT, TRUE, DemoCave1F_GalarianYamask_Trigger_NoEvoContidion
	msgbox DemoCave1F_Text_GalarianYamask_NoYamask
	releaseall
	end

DemoCave1F_GalarianYamask_Trigger_NoEvoContidion::
	msgbox DemoCave1F_Text_GalarianYamask_NoEvoCondition
	releaseall
	end

DemoCave1F_GalarianYamask_Trigger_EvoInterrupted::
	msgbox DemoCave1F_Text_GalarianYamask_CancelledEvolution
	releaseall
	end

DemoCave1F_GalarianYamask_Trigger_EvoSuccess::
	msgbox DemoCave1F_Text_GalarianYamask_SuccessfulEvolution
	releaseall
	end

DemoCave1F_EventScript_Archie::
	lockall
	faceplayer
	msgbox DemoCave1F_Text_Archie
	releaseall
	end
