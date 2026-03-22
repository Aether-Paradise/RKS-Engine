# This allows CI to recompile and change specific configs to confirm that they still build.

CONFIG_OVERRIDE ?=
ifneq (,$(CONFIG_OVERRIDE))
  override CPPFLAGS += -DCONFIG_OVERRIDE=$(CONFIG_OVERRIDE)
endif

touch-config_overrides:
	@touch $(INCLUDE_DIRS)/config/override/party_menu.h
