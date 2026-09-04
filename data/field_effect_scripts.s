#include "config/item.h"
	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section script_data, "aw"

	.align 2
gFieldEffectScriptPointers::
	ptrvalue gFieldEffectScript_ExclamationMarkIcon1      @ FLDEFF_EXCLAMATION_MARK_ICON
	ptrvalue gFieldEffectScript_UseCutOnTallGrass         @ FLDEFF_USE_CUT_ON_GRASS
	ptrvalue gFieldEffectScript_UseCutOnTree              @ FLDEFF_USE_CUT_ON_TREE
	ptrvalue gFieldEffectScript_Shadow                    @ FLDEFF_SHADOW
	ptrvalue gFieldEffectScript_TallGrass                 @ FLDEFF_TALL_GRASS
	ptrvalue gFieldEffectScript_Ripple                    @ FLDEFF_RIPPLE
	ptrvalue gFieldEffectScript_FieldMoveShowMon          @ FLDEFF_FIELD_MOVE_SHOW_MON
	ptrvalue gFieldEffectScript_Ash                       @ FLDEFF_ASH
	ptrvalue gFieldEffectScript_SurfBlob                  @ FLDEFF_SURF_BLOB
	ptrvalue gFieldEffectScript_UseSurf                   @ FLDEFF_USE_SURF
	ptrvalue gFieldEffectScript_GroundImpactDust          @ FLDEFF_DUST
	ptrvalue gFieldEffectScript_UseSecretPowerCave        @ FLDEFF_USE_SECRET_POWER_CAVE
	ptrvalue gFieldEffectScript_JumpTallGrass             @ FLDEFF_JUMP_TALL_GRASS
	ptrvalue gFieldEffectScript_SandFootprints            @ FLDEFF_SAND_FOOTPRINTS
	ptrvalue gFieldEffectScript_JumpBigSplash             @ FLDEFF_JUMP_BIG_SPLASH
	ptrvalue gFieldEffectScript_Splash                    @ FLDEFF_SPLASH
	ptrvalue gFieldEffectScript_JumpSmallSplash           @ FLDEFF_JUMP_SMALL_SPLASH
	ptrvalue gFieldEffectScript_LongGrass                 @ FLDEFF_LONG_GRASS
	ptrvalue gFieldEffectScript_JumpLongGrass             @ FLDEFF_JUMP_LONG_GRASS
	ptrvalue gFieldEffectScript_ShakingGrass              @ FLDEFF_SHAKING_GRASS
	ptrvalue gFieldEffectScript_ShakingGrass2             @ FLDEFF_SHAKING_LONG_GRASS
	ptrvalue gFieldEffectScript_UnusedSand                @ FLDEFF_SAND_HOLE
	ptrvalue gFieldEffectScript_WaterSurfacing            @ FLDEFF_WATER_SURFACING
	ptrvalue gFieldEffectScript_BerryTreeGrowthSparkle    @ FLDEFF_BERRY_TREE_GROWTH_SPARKLE
	ptrvalue gFieldEffectScript_DeepSandFootprints        @ FLDEFF_DEEP_SAND_FOOTPRINTS
	ptrvalue gFieldEffectScript_PokeCenterHeal            @ FLDEFF_POKECENTER_HEAL
	ptrvalue gFieldEffectScript_UseSecretPowerTree        @ FLDEFF_USE_SECRET_POWER_TREE
	ptrvalue gFieldEffectScript_UseSecretPowerShrub       @ FLDEFF_USE_SECRET_POWER_SHRUB
	ptrvalue gFieldEffectScript_TreeDisguise              @ FLDEFF_TREE_DISGUISE
	ptrvalue gFieldEffectScript_MountainDisguise          @ FLDEFF_MOUNTAIN_DISGUISE
	ptrvalue gFieldEffectScript_NPCUseFly                 @ FLDEFF_NPCFLY_OUT
	ptrvalue gFieldEffectScript_UseFly                    @ FLDEFF_USE_FLY
	ptrvalue gFieldEffectScript_FlyIn                     @ FLDEFF_FLY_IN
	ptrvalue gFieldEffectScript_QuestionMarkIcon          @ FLDEFF_QUESTION_MARK_ICON
	ptrvalue gFieldEffectScript_FeetInFlowingWater        @ FLDEFF_FEET_IN_FLOWING_WATER
	ptrvalue gFieldEffectScript_BikeTireTracks            @ FLDEFF_BIKE_TIRE_TRACKS
	ptrvalue gFieldEffectScript_SandDisguisePlaceholder   @ FLDEFF_SAND_DISGUISE
	ptrvalue gFieldEffectScript_UseRockSmash              @ FLDEFF_USE_ROCK_SMASH
	ptrvalue gFieldEffectScript_UseDig                    @ FLDEFF_USE_DIG
	ptrvalue gFieldEffectScript_SandPile                  @ FLDEFF_SAND_PILE
	ptrvalue gFieldEffectScript_UseStrength               @ FLDEFF_USE_STRENGTH
	ptrvalue gFieldEffectScript_ShortGrass                @ FLDEFF_SHORT_GRASS
	ptrvalue gFieldEffectScript_HotSpringsWater           @ FLDEFF_HOT_SPRINGS_WATER
	ptrvalue gFieldEffectScript_UseWaterfall              @ FLDEFF_USE_WATERFALL
	ptrvalue gFieldEffectScript_UseDive                   @ FLDEFF_USE_DIVE
	ptrvalue gFieldEffectScript_PokeballTrail             @ FLDEFF_POKEBALL_TRAIL
	ptrvalue gFieldEffectScript_HeartIcon                 @ FLDEFF_HEART_ICON
	ptrvalue gFieldEffectScript_Nop47                     @ FLDEFF_NOP_47
	ptrvalue gFieldEffectScript_Nop48                     @ FLDEFF_NOP_48
	ptrvalue gFieldEffectScript_AshPuff                   @ FLDEFF_ASH_PUFF
	ptrvalue gFieldEffectScript_AshLaunch                 @ FLDEFF_ASH_LAUNCH
	ptrvalue gFieldEffectScript_SweetScent                @ FLDEFF_SWEET_SCENT
	ptrvalue gFieldEffectScript_SandPillar                @ FLDEFF_SAND_PILLAR
	ptrvalue gFieldEffectScript_Bubbles                   @ FLDEFF_BUBBLES
	ptrvalue gFieldEffectScript_Sparkle                   @ FLDEFF_SPARKLE
	ptrvalue gFieldEffectScript_ShowSecretPowerCave       @ FLDEFF_SECRET_POWER_CAVE
	ptrvalue gFieldEffectScript_ShowSecretPowerTree       @ FLDEFF_SECRET_POWER_TREE
	ptrvalue gFieldEffectScript_ShowSecretPowerShrub      @ FLDEFF_SECRET_POWER_SHRUB
	ptrvalue gFieldEffectScript_ShowCutGrass              @ FLDEFF_CUT_GRASS
	ptrvalue gFieldEffectScript_FieldMoveShowMonInit      @ FLDEFF_FIELD_MOVE_SHOW_MON_INIT
	ptrvalue gFieldEffectScript_UsePuzzleEffect           @ FLDEFF_USE_TOMB_PUZZLE_EFFECT
	ptrvalue gFieldEffectScript_SecretBaseBootPC          @ FLDEFF_PCTURN_ON
	ptrvalue gFieldEffectScript_HallOfFameRecord          @ FLDEFF_HALL_OF_FAME_RECORD
	ptrvalue gFieldEffectScript_UseTeleport               @ FLDEFF_USE_TELEPORT
	ptrvalue gFieldEffectScript_RayquazaSpotlight         @ FLDEFF_RAYQUAZA_SPOTLIGHT
	ptrvalue gFieldEffectScript_DestroyDeoxysRock         @ FLDEFF_DESTROY_DEOXYS_ROCK
	ptrvalue gFieldEffectScript_MoveDeoxysRock            @ FLDEFF_MOVE_DEOXYS_ROCK
	ptrvalue gFldEffScript_UseVsSeeker                    @ FLDEFF_USE_VS_SEEKER
	ptrvalue gFldEffScript_XIcon                          @ FLDEFF_X_ICON
	ptrvalue gFldEffScript_DoubleExclMarkIcon             @ FLDEFF_DOUBLE_EXCL_MARK_ICON
	ptrvalue gFieldEffectScript_TracksSlither             @ FLDEFF_TRACKS_SLITHER
	ptrvalue gFieldEffectScript_TracksBug                 @ FLDEFF_TRACKS_BUG
	ptrvalue gFieldEffectScript_TracksSpot                @ FLDEFF_TRACKS_SPOT
	ptrvalue gFieldEffectScript_CaveDust                  @ FLDEFF_CAVE_DUST
	ptrvalue gFieldEffectScript_Defog                     @ FLDEFF_DEFOG
	ptrvalue gFieldEffectScript_UseRockClimb              @ FLDEFF_USE_ROCK_CLIMB
	ptrvalue gFieldEffectScript_RockClimbDust             @ FLDEFF_ROCK_CLIMB_DUST
	ptrvalue gFieldEffectScript_ORASDowse                 @ FLDEFF_ORAS_DOWSE
	ptrvalue gFldEffScript_SmileyFaceIcon    		        @ FLDEFF_SMILEY_FACE_ICON
	ptrvalue gFieldEffectScript_HallOfFameRecordFrlg      @ FLDEFF_HALL_OF_FAME_RECORD_FRLG
	ptrvalue gFldEffScript_PhotoFlash      		        @ FLDEFF_PHOTO_FLASH
	ptrvalue gFieldEffectScript_OWE_SpawnAnim				@ FLDEFF_OW_ENCOUNTER_SPAWN_ANIM

gFieldEffectScript_ExclamationMarkIcon1::
	field_eff_callnative FldEff_ExclamationMarkIcon
	field_eff_end

gFieldEffectScript_UseCutOnTallGrass::
	field_eff_callnative FldEff_UseCutOnGrass
	field_eff_end

gFieldEffectScript_UseCutOnTree::
	field_eff_callnative FldEff_UseCutOnTree
	field_eff_end

gFieldEffectScript_Shadow::
	field_eff_callnative FldEff_Shadow
	field_eff_end

gFieldEffectScript_TallGrass::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect1, FldEff_TallGrass
	field_eff_end

gFieldEffectScript_Ripple::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect1, FldEff_Ripple
	field_eff_end

gFieldEffectScript_FieldMoveShowMon::
	field_eff_callnative FldEff_FieldMoveShowMon
	field_eff_end

gFieldEffectScript_Ash::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect1, FldEff_Ash
	field_eff_end

gFieldEffectScript_SurfBlob::
	field_eff_callnative FldEff_SurfBlob
	field_eff_end

gFieldEffectScript_UseSurf::
	field_eff_callnative FldEff_UseSurf
	field_eff_end

gFieldEffectScript_GroundImpactDust::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_Dust
	field_eff_end

gFieldEffectScript_UseSecretPowerCave::
	field_eff_callnative FldEff_UseSecretPowerCave
	field_eff_end

gFieldEffectScript_JumpTallGrass::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect1, FldEff_JumpTallGrass
	field_eff_end

gFieldEffectScript_SandFootprints::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_SandFootprints
	field_eff_end

gFieldEffectScript_JumpBigSplash::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_JumpBigSplash
	field_eff_end

gFieldEffectScript_Splash::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_Splash
	field_eff_end

gFieldEffectScript_JumpSmallSplash::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_JumpSmallSplash
	field_eff_end

gFieldEffectScript_LongGrass::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect1, FldEff_LongGrass
	field_eff_end

gFieldEffectScript_JumpLongGrass::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect1, FldEff_JumpLongGrass
	field_eff_end

gFieldEffectScript_ShakingGrass::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect1, FldEff_ShakingGrass
	field_eff_end

gFieldEffectScript_ShakingGrass2::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect1, FldEff_ShakingGrass2
	field_eff_end

gFieldEffectScript_UnusedSand::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_UnusedSand
	field_eff_end

gFieldEffectScript_WaterSurfacing::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_WaterSurfacing
	field_eff_end

gFieldEffectScript_BerryTreeGrowthSparkle::
	field_eff_callnative FldEff_BerryTreeGrowthSparkle
	field_eff_end

gFieldEffectScript_DeepSandFootprints::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_DeepSandFootprints
	field_eff_end

gFieldEffectScript_PokeCenterHeal::
	field_eff_loadfadedpal gSpritePalette_PokeballGlow
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_PokecenterHeal
	field_eff_end

gFieldEffectScript_UseSecretPowerTree::
	field_eff_callnative FldEff_UseSecretPowerTree
	field_eff_end

gFieldEffectScript_UseSecretPowerShrub::
	field_eff_callnative FldEff_UseSecretPowerShrub
	field_eff_end

gFieldEffectScript_TreeDisguise::
	field_eff_callnative ShowTreeDisguiseFieldEffect
	field_eff_end

gFieldEffectScript_MountainDisguise::
	field_eff_callnative ShowMountainDisguiseFieldEffect
	field_eff_end

gFieldEffectScript_NPCUseFly::
	field_eff_callnative FldEff_NPCFlyOut
	field_eff_end

gFieldEffectScript_UseFly::
	field_eff_callnative FldEff_UseFly
	field_eff_end

gFieldEffectScript_FlyIn::
	field_eff_callnative FldEff_FlyIn
	field_eff_end

gFieldEffectScript_QuestionMarkIcon::
	field_eff_callnative FldEff_QuestionMarkIcon
	field_eff_end

gFieldEffectScript_FeetInFlowingWater::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_FeetInFlowingWater
	field_eff_end

gFieldEffectScript_BikeTireTracks::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_BikeTireTracks
	field_eff_end

gFieldEffectScript_SandDisguisePlaceholder::
	field_eff_callnative ShowSandDisguiseFieldEffect
	field_eff_end

gFieldEffectScript_UseRockSmash::
	field_eff_callnative FldEff_UseRockSmash
	field_eff_end

gFieldEffectScript_UseStrength::
	field_eff_callnative FldEff_UseStrength
	field_eff_end

gFieldEffectScript_UseDig::
	field_eff_callnative FldEff_UseDig
	field_eff_end

gFieldEffectScript_SandPile::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_SandPile
	field_eff_end

gFieldEffectScript_ShortGrass::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect1, FldEff_ShortGrass
	field_eff_end

gFieldEffectScript_HotSpringsWater::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect1, FldEff_HotSpringsWater
	field_eff_end

gFieldEffectScript_UseWaterfall::
	field_eff_callnative FldEff_UseWaterfall
	field_eff_end

gFieldEffectScript_UseDive::
	field_eff_callnative FldEff_UseDive
	field_eff_end

gFieldEffectScript_PokeballTrail::
	field_eff_loadpal gSpritePalette_Pokeball
	field_eff_callnative FldEff_PokeballTrail
	field_eff_end

gFieldEffectScript_HeartIcon::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_HeartIcon
	field_eff_end

gFieldEffectScript_Nop47::
	field_eff_callnative FldEff_Nop47
	field_eff_end

gFieldEffectScript_Nop48::
	field_eff_callnative FldEff_Nop48
	field_eff_end

gFieldEffectScript_AshPuff::
	field_eff_loadfadedpal_callnative gSpritePalette_Ash, FldEff_AshPuff
	field_eff_end

gFieldEffectScript_AshLaunch::
	field_eff_loadfadedpal_callnative gSpritePalette_Ash, FldEff_AshLaunch
	field_eff_end

gFieldEffectScript_SweetScent::
	field_eff_callnative FldEff_SweetScent
	field_eff_end

gFieldEffectScript_SandPillar::
	field_eff_loadfadedpal_callnative gSpritePalette_SandPillar, FldEff_SandPillar
	field_eff_end

gFieldEffectScript_Bubbles::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_Bubbles
	field_eff_end

gFieldEffectScript_Sparkle::
	field_eff_loadfadedpal_callnative gSpritePalette_SmallSparkle, FldEff_Sparkle
	field_eff_end

gFieldEffectScript_ShowSecretPowerCave::
	field_eff_loadfadedpal_callnative gSpritePalette_SecretPower_Cave, FldEff_SecretPowerCave
	field_eff_end

gFieldEffectScript_ShowSecretPowerTree::
	field_eff_loadfadedpal_callnative gSpritePalette_SecretPower_Plant, FldEff_SecretPowerTree
	field_eff_end

gFieldEffectScript_ShowSecretPowerShrub::
	field_eff_loadfadedpal_callnative gSpritePalette_SecretPower_Plant, FldEff_SecretPowerShrub
	field_eff_end

gFieldEffectScript_ShowCutGrass::
	field_eff_loadfadedpal_callnative gSpritePalette_CutGrass, FldEff_CutGrass
	field_eff_end

gFieldEffectScript_FieldMoveShowMonInit::
	field_eff_callnative FldEff_FieldMoveShowMonInit
	field_eff_end

gFieldEffectScript_UsePuzzleEffect::
	field_eff_callnative FldEff_UsePuzzleEffect
	field_eff_end

gFieldEffectScript_SecretBaseBootPC::
	field_eff_callnative FldEff_SecretBasePCTurnOn
	field_eff_end

gFieldEffectScript_HallOfFameRecord::
	field_eff_loadfadedpal gSpritePalette_PokeballGlow
	field_eff_loadfadedpal_callnative gSpritePalette_HofMonitor, FldEff_HallOfFameRecord
	field_eff_end

gFieldEffectScript_UseTeleport::
	field_eff_callnative FldEff_UseTeleport
	field_eff_end

gFieldEffectScript_RayquazaSpotlight::
	field_eff_callnative FldEff_RayquazaSpotlight
	field_eff_end

gFieldEffectScript_DestroyDeoxysRock::
	field_eff_callnative FldEff_DestroyDeoxysRock
	field_eff_end

gFieldEffectScript_MoveDeoxysRock::
	field_eff_callnative FldEff_MoveDeoxysRock
	field_eff_end

gFldEffScript_UseVsSeeker::
	field_eff_callnative FldEff_UseVsSeeker
	field_eff_end

gFldEffScript_XIcon::
	field_eff_callnative FldEff_XIcon
	field_eff_end

gFldEffScript_DoubleExclMarkIcon::
	field_eff_callnative FldEff_DoubleExclMarkIcon
	field_eff_end

gFieldEffectScript_TracksBug::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_TracksBug
	field_eff_end

gFieldEffectScript_TracksSpot::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_TracksSpot
	field_eff_end

gFieldEffectScript_TracksSlither::
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_TracksSlither
	field_eff_end

gFieldEffectScript_CaveDust::
	field_eff_loadfadedpal_callnative gSpritePalette_CaveDust FldEff_CaveDust
	field_eff_end

gFieldEffectScript_Defog::
	field_eff_callnative FldEff_Defog
	field_eff_end

gFieldEffectScript_UseRockClimb:: @ 82DBC3F
	field_eff_callnative FldEff_UseRockClimb
	field_eff_end

gFieldEffectScript_RockClimbDust:: @ 82DBB28
	field_eff_loadfadedpal_callnative gSpritePalette_BigDust, FldEff_RockClimbDust
	field_eff_end

gFieldEffectScript_ORASDowse::
	field_eff_callnative FldEff_ORASDowsing
	field_eff_end

gFldEffScript_SmileyFaceIcon::
	field_eff_callnative FldEff_SmileyFaceIcon
	field_eff_end

gFieldEffectScript_HallOfFameRecordFrlg::
	field_eff_loadfadedpal gSpritePalette_PokeballGlow
	field_eff_loadfadedpal_callnative gSpritePalette_HofMonitor_Frlg, FldEff_HallOfFameRecord
	field_eff_end

gFldEffScript_PhotoFlash::
	field_eff_callnative FldEff_PhotoFlash
	field_eff_end

gFieldEffectScript_OWE_SpawnAnim::
	field_eff_callnative FldEff_OWE_SpawnAnim
	field_eff_end
