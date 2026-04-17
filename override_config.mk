# This allows CI to recompile and change specific configs to confirm that they still build.

OVR_RKSE_PARTY_MENU_DESIGN ?=
OVR_RKSE_FREE_SAVE ?=
OVR_FRLG_I_USE_FRLG_BAG ?=
ifneq (,$(OVR_RKSE_PARTY_MENU_DESIGN))
  override CPPFLAGS += -DOVR_RKSE_PARTY_MENU_DESIGN=$(OVR_RKSE_PARTY_MENU_DESIGN)
endif
ifneq (,$(OVR_RKSE_FREE_SAVE))
  override CPPFLAGS += -DOVR_RKSE_FREE_SAVE=$(OVR_RKSE_FREE_SAVE)
endif
ifneq (,$(OVR_FRLG_I_USE_FRLG_BAG))
  override CPPFLAGS += -DOVR_FRLG_I_USE_FRLG_BAG=$(OVR_FRLG_I_USE_FRLG_BAG)
endif

touch-config_party_menu:
	@touch $(INCLUDE_DIRS)/config/override/party_menu.h

touch-config_save:
	@touch $(INCLUDE_DIRS)/config/override/save.h

touch-config_frlg_bag:
	@touch $(INCLUDE_DIRS)/config/override/frlg_bag.h
