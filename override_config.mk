# This allows CI to recompile and change specific configs to confirm that they still build.

OVR_RKSE_PARTY_MENU_DESIGN ?=
OVR_RKSE_FREE_SAVE ?=
ifneq (,$(OVR_RKSE_PARTY_MENU_DESIGN))
  override CPPFLAGS += -DOVR_RKSE_PARTY_MENU_DESIGN=$(OVR_RKSE_PARTY_MENU_DESIGN)
endif
ifneq (,$(OVR_RKSE_FREE_SAVE))
  override CPPFLAGS += -DOVR_RKSE_FREE_SAVE=$(OVR_RKSE_FREE_SAVE)
endif

touch-config_party_menu:
	@touch $(INCLUDE_DIRS)/config/override/party_menu.h

touch-config_save:
	@touch $(INCLUDE_DIRS)/config/override/save.h
