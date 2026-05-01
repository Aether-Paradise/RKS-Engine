# This allows CI to recompile and change specific configs to confirm that they still build.

OVR_RKSE_PARTY_MENU_DESIGN ?=
OVR_RKSE_FREE_SAVE ?=
ifneq (,$(OVR_RKSE_PARTY_MENU_DESIGN))
  override CPPFLAGS += -DOVR_RKSE_PARTY_MENU_DESIGN=$(OVR_RKSE_PARTY_MENU_DESIGN)
endif
ifneq (,$(OVR_RKSE_FREE_SAVE))
  override CPPFLAGS += -DOVR_RKSE_FREE_SAVE=$(OVR_RKSE_FREE_SAVE)
endif
ifneq (,$(OVR_T_SHOULD_RUN_MOVE_ANIM))
  override CPPFLAGS += -DOVR_T_SHOULD_RUN_MOVE_ANIM=$(OVR_T_SHOULD_RUN_MOVE_ANIM)
endif

touch-config_party_menu:
	@touch $(INCLUDE_DIRS)/config/override/party_menu.h

touch-config_save:
	@touch $(INCLUDE_DIRS)/config/override/save.h
