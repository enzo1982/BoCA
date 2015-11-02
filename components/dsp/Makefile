########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

all:
	$(call makein,resample)

clean:
	$(call cleanin,resample)

install:
	$(call makein,resample,install)

uninstall:
	$(call makein,resample,uninstall)
