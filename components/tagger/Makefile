########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

all:
	$(call makein,apev2)
	$(call makein,cart)
	$(call makein,flac)
	$(call makein,id3v1)
	$(call makein,id3v2)
	$(call makein,mp4)
	$(call makein,riff)
ifeq ($(BUILD_OSX),True)
	$(call makein,tocplist)
endif
	$(call makein,vorbis)
ifeq ($(BUILD_WIN32),True)
	$(call makein,wma)
endif

clean:
	$(call cleanin,apev2)
	$(call cleanin,cart)
	$(call cleanin,flac)
	$(call cleanin,id3v1)
	$(call cleanin,id3v2)
	$(call cleanin,mp4)
	$(call cleanin,riff)
ifeq ($(BUILD_OSX),True)
	$(call cleanin,tocplist)
endif
	$(call cleanin,vorbis)
ifeq ($(BUILD_WIN32),True)
	$(call cleanin,wma)
endif

install:
	$(call makein,apev2,install)
	$(call makein,cart,install)
	$(call makein,flac,install)
	$(call makein,id3v1,install)
	$(call makein,id3v2,install)
	$(call makein,mp4,install)
	$(call makein,riff,install)
ifeq ($(BUILD_OSX),True)
	$(call makein,tocplist,install)
endif
	$(call makein,vorbis,install)

uninstall:
	$(call makein,apev2,uninstall)
	$(call makein,cart,uninstall)
	$(call makein,flac,uninstall)
	$(call makein,id3v1,uninstall)
	$(call makein,id3v2,uninstall)
	$(call makein,mp4,uninstall)
	$(call makein,riff,uninstall)
ifeq ($(BUILD_OSX),True)
	$(call makein,tocplist,uninstall)
endif
	$(call makein,vorbis,uninstall)
