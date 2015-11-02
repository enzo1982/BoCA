########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

all:
	$(call makein,md5)

clean:
	$(call cleanin,md5)

install:
	$(call makein,md5,install)

uninstall:
	$(call makein,md5,uninstall)
