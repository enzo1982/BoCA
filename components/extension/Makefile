########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

all:
	$(call makein,donate)
	$(call makein,langnotify)
	$(call makein,protocols)
	$(call makein,statustime)
	$(call makein,tagedit)
	$(call makein,youtube)

clean:
	$(call cleanin,donate)
	$(call cleanin,langnotify)
	$(call cleanin,protocols)
	$(call cleanin,statustime)
	$(call cleanin,tagedit)
	$(call cleanin,youtube)

install:
	$(call makein,donate,install)
	$(call makein,langnotify,install)
	$(call makein,statustime,install)
	$(call makein,tagedit,install)
	$(call makein,youtube,install)

uninstall:
	$(call makein,donate,uninstall)
	$(call makein,langnotify,uninstall)
	$(call makein,statustime,uninstall)
	$(call makein,tagedit,uninstall)
	$(call makein,youtube,uninstall)
