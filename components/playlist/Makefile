########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

all:
	$(call makein,cuesheet)
	$(call makein,m3u)
	$(call makein,pls)
	$(call makein,vclt)
	$(call makein,wpl)
	$(call makein,xspf)

clean:
	$(call cleanin,cuesheet)
	$(call cleanin,m3u)
	$(call cleanin,pls)
	$(call cleanin,vclt)
	$(call cleanin,wpl)
	$(call cleanin,xspf)

install:
	$(call makein,cuesheet,install)
	$(call makein,m3u,install)
	$(call makein,pls,install)
	$(call makein,vclt,install)
	$(call makein,wpl,install)
	$(call makein,xspf,install)

uninstall:
	$(call makein,cuesheet,uninstall)
	$(call makein,m3u,uninstall)
	$(call makein,pls,uninstall)
	$(call makein,vclt,uninstall)
	$(call makein,wpl,uninstall)
	$(call makein,xspf,uninstall)
