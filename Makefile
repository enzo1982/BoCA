########## BoCA directory makefile ##########

BOCA_PATH = .

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

all:
	mkdir -p $(BOCA_PATH)/$(BINDIR) $(BOCA_PATH)/$(LIBDIR)

	+ $(call makein,runtime)
	+ $(call makein,components)

clean:
	+ $(call cleanin,runtime)
	+ $(call cleanin,components)

ifneq ($(SRCDIR),$(CURDIR))
	rmdir $(BOCA_PATH)/$(BINDIR) $(BOCA_PATH)/$(LIBDIR) || true
endif

install: all
ifneq ($(BUILD_WIN32),True)
	$(call makein,include,install)
	$(call makein,runtime,install)
	$(call makein,scripts,install)
	$(call makein,components,install)
endif

uninstall:
ifneq ($(BUILD_WIN32),True)
	$(call makein,include,uninstall)
	$(call makein,runtime,uninstall)
	$(call makein,scripts,uninstall)
	$(call makein,components,uninstall)
endif
