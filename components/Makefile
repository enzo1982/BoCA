########## BoCA directory makefile ##########

BOCA_PATH = ..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

all:
	$(call makein,decoder)
	$(call makein,deviceinfo)
	$(call makein,dsp)
	$(call makein,encoder)
	$(call makein,extension)
	$(call makein,other)
	$(call makein,output)
	$(call makein,playlist)
	$(call makein,tagger)
	$(call makein,verifier)

clean:
	$(call cleanin,decoder)
	$(call cleanin,deviceinfo)
	$(call cleanin,dsp)
	$(call cleanin,encoder)
	$(call cleanin,extension)
	$(call cleanin,other)
	$(call cleanin,output)
	$(call cleanin,playlist)
	$(call cleanin,tagger)
	$(call cleanin,verifier)

install:
	$(call makein,decoder,install)
	$(call makein,deviceinfo,install)
	$(call makein,dsp,install)
	$(call makein,encoder,install)
	$(call makein,extension,install)
	$(call makein,other,install)
	$(call makein,output,install)
	$(call makein,playlist,install)
	$(call makein,tagger,install)
	$(call makein,verifier,install)

uninstall:
	$(call makein,decoder,uninstall)
	$(call makein,deviceinfo,uninstall)
	$(call makein,dsp,uninstall)
	$(call makein,encoder,uninstall)
	$(call makein,extension,uninstall)
	$(call makein,other,uninstall)
	$(call makein,output,uninstall)
	$(call makein,playlist,uninstall)
	$(call makein,tagger,uninstall)
	$(call makein,verifier,uninstall)
