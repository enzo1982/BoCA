########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

all:
	$(call makein,aiff)
	$(call makein,au)
	$(call makein,bonk)
ifeq ($(BUILD_UNIX),True)
ifneq ($(BUILD_QNX),True)
ifneq ($(BUILD_OPENBSD),True)
	$(call makein,cdio)
endif
ifneq ($(BUILD_OSX),True)
ifneq ($(BUILD_GNU),True)
ifneq ($(BUILD_SOLARIS),True)
ifneq ($(BUILD_NETBSD),True)
ifneq ($(BUILD_HAIKU),True)
	$(call makein,cdparanoia)
endif
endif
endif
endif
endif
endif
endif
ifeq ($(BUILD_WIN32),True)
	$(call makein,cdrip)
endif
	$(call makein,cuesheet)
	$(call makein,faad2)
	$(call makein,fdkaac)
	$(call makein,flac)
	$(call makein,lame)
ifeq ($(BUILD_WIN32),True)
	$(call makein,mac)
endif
	$(call makein,mad)
	$(call makein,mpg123)
	$(call makein,opus)
	$(call makein,sndfile)
	$(call makein,speex)
ifeq ($(BUILD_WIN32),True)
	$(call makein,twinvq)
endif
	$(call makein,voc)
	$(call makein,vorbis)
	$(call makein,wave)
ifeq ($(BUILD_WIN32),True)
	$(call makein,winamp)
	$(call makein,wma)
endif

clean:
	$(call cleanin,aiff)
	$(call cleanin,au)
	$(call cleanin,bonk)
ifeq ($(BUILD_UNIX),True)
ifneq ($(BUILD_QNX),True)
ifneq ($(BUILD_OPENBSD),True)
	$(call cleanin,cdio)
endif
ifneq ($(BUILD_OSX),True)
ifneq ($(BUILD_GNU),True)
ifneq ($(BUILD_SOLARIS),True)
ifneq ($(BUILD_NETBSD),True)
ifneq ($(BUILD_HAIKU),True)
	$(call cleanin,cdparanoia)
endif
endif
endif
endif
endif
endif
endif
ifeq ($(BUILD_WIN32),True)
	$(call cleanin,cdrip)
endif
	$(call cleanin,cuesheet)
	$(call cleanin,faad2)
	$(call cleanin,fdkaac)
	$(call cleanin,flac)
	$(call cleanin,lame)
ifeq ($(BUILD_WIN32),True)
	$(call cleanin,mac)
endif
	$(call cleanin,mad)
	$(call cleanin,mpg123)
	$(call cleanin,opus)
	$(call cleanin,sndfile)
	$(call cleanin,speex)
ifeq ($(BUILD_WIN32),True)
	$(call cleanin,twinvq)
endif
	$(call cleanin,voc)
	$(call cleanin,vorbis)
	$(call cleanin,wave)
ifeq ($(BUILD_WIN32),True)
	$(call cleanin,winamp)
	$(call cleanin,wma)
endif

install:
ifeq ($(BUILD_UNIX),True)
ifneq ($(BUILD_QNX),True)
ifneq ($(BUILD_OPENBSD),True)
	$(call makein,cdio,install)
endif
ifneq ($(BUILD_OSX),True)
ifneq ($(BUILD_GNU),True)
ifneq ($(BUILD_SOLARIS),True)
ifneq ($(BUILD_NETBSD),True)
ifneq ($(BUILD_HAIKU),True)
	$(call makein,cdparanoia,install)
endif
endif
endif
endif
endif
endif
endif
	$(call makein,cuesheet,install)
	$(call makein,faad2,install)
	$(call makein,fdkaac,install)
	$(call makein,flac,install)
ifneq ($(BUILD_OSX),True)
	$(call makein,lame,install)
	$(call makein,mad,install)
endif
	$(call makein,mpg123,install)
	$(call makein,opus,install)
	$(call makein,sndfile,install)
	$(call makein,speex,install)
	$(call makein,vorbis,install)

uninstall:
ifeq ($(BUILD_UNIX),True)
ifneq ($(BUILD_QNX),True)
ifneq ($(BUILD_OPENBSD),True)
	$(call makein,cdio,uninstall)
endif
ifneq ($(BUILD_OSX),True)
ifneq ($(BUILD_GNU),True)
ifneq ($(BUILD_SOLARIS),True)
ifneq ($(BUILD_NETBSD),True)
ifneq ($(BUILD_HAIKU),True)
	$(call makein,cdparanoia,uninstall)
endif
endif
endif
endif
endif
endif
endif
	$(call makein,cuesheet,uninstall)
	$(call makein,faad2,uninstall)
	$(call makein,fdkaac,uninstall)
	$(call makein,flac,uninstall)
ifneq ($(BUILD_OSX),True)
	$(call makein,lame,uninstall)
	$(call makein,mad,uninstall)
endif
	$(call makein,mpg123,uninstall)
	$(call makein,opus,uninstall)
	$(call makein,sndfile,uninstall)
	$(call makein,speex,uninstall)
	$(call makein,vorbis,uninstall)
