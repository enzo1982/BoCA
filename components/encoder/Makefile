########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

all:
ifeq ($(BUILD_WIN32),True)
	$(call makein,bladeenc)
endif
	$(call makein,bonk)
ifeq ($(BUILD_OSX),True)
	$(call makein,coreaudio)
endif
ifeq ($(BUILD_WIN32),True)
	$(call makein,coreaudio)
	$(call makein,coreaudioconnect)
endif
	$(call makein,faac)
	$(call makein,fdkaac)
	$(call makein,flac)
	$(call makein,lame)
ifeq ($(BUILD_WIN32),True)
	$(call makein,mac)
endif
	$(call makein,meh)
	$(call makein,opus)
	$(call makein,sndfile)
	$(call makein,speex)
ifeq ($(BUILD_WIN32),True)
	$(call makein,twinvq)
endif
	$(call makein,voaacenc)
	$(call makein,vorbis)
	$(call makein,wave)
ifeq ($(BUILD_WIN32),True)
	$(call makein,wma)
endif

clean:
ifeq ($(BUILD_WIN32),True)
	$(call cleanin,bladeenc)
endif
	$(call cleanin,bonk)
ifeq ($(BUILD_OSX),True)
	$(call cleanin,coreaudio)
endif
ifeq ($(BUILD_WIN32),True)
	$(call cleanin,coreaudio)
	$(call cleanin,coreaudioconnect)
endif
	$(call cleanin,faac)
	$(call cleanin,fdkaac)
	$(call cleanin,flac)
	$(call cleanin,lame)
ifeq ($(BUILD_WIN32),True)
	$(call cleanin,mac)
endif
	$(call cleanin,meh)
	$(call cleanin,opus)
	$(call cleanin,sndfile)
	$(call cleanin,speex)
ifeq ($(BUILD_WIN32),True)
	$(call cleanin,twinvq)
endif
	$(call cleanin,voaacenc)
	$(call cleanin,vorbis)
	$(call cleanin,wave)
ifeq ($(BUILD_WIN32),True)
	$(call cleanin,wma)
endif

install:
ifeq ($(BUILD_OSX),True)
	$(call makein,coreaudio,install)
endif
ifneq ($(BUILD_OSX),True)
	$(call makein,faac,install)
endif
	$(call makein,fdkaac,install)
	$(call makein,flac,install)
	$(call makein,lame,install)
	$(call makein,meh,install)
	$(call makein,opus,install)
	$(call makein,sndfile,install)
	$(call makein,speex,install)
ifneq ($(BUILD_OSX),True)
	$(call makein,voaacenc,install)
endif
	$(call makein,vorbis,install)

uninstall:
ifeq ($(BUILD_OSX),True)
	$(call makein,coreaudio,uninstall)
endif
ifneq ($(BUILD_OSX),True)
	$(call makein,faac,uninstall)
endif
	$(call makein,fdkaac,uninstall)
	$(call makein,flac,uninstall)
	$(call makein,lame,uninstall)
	$(call makein,meh,uninstall)
	$(call makein,opus,uninstall)
	$(call makein,sndfile,uninstall)
	$(call makein,speex,uninstall)
ifneq ($(BUILD_OSX),True)
	$(call makein,voaacenc,uninstall)
endif
	$(call makein,vorbis,uninstall)
