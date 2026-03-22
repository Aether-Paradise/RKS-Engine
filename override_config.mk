# This allows CI to recompile and change specific configs to confirm that they still build.

OVR_RKSE_PARTY_MENU_DESIGN ?=
ifneq (,$(OVR_RKSE_PARTY_MENU_DESIGN))
  override CPPFLAGS += -DOVR_RKSE_PARTY_MENU_DESIGN=$(OVR_RKSE_PARTY_MENU_DESIGN)
endif

touch-config_overrides:
	@touch $(INCLUDE_DIRS)/config/override/party_menu.h
