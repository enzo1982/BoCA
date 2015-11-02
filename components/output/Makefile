########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

all:
ifeq ($(BUILD_LINUX),True)
	$(call makein,alsa)
endif
ifeq ($(BUILD_OSX),True)
	$(call makein,coreaudio)
endif
ifeq ($(BUILD_FREEBSD),True)
	$(call makein,oss)
endif
ifeq ($(BUILD_OPENBSD),True)
	$(call makein,oss)
endif
ifeq ($(BUILD_NETBSD),True)
	$(call makein,oss)
endif
ifeq ($(BUILD_SOLARIS),True)
	$(call makein,oss)
endif
ifeq ($(BUILD_QNX),True)
	$(call makein,qsa)
endif
ifeq ($(BUILD_WIN32),True)
	$(call makein,directsound)
	$(call makein,waveout)
	$(call makein,winamp)
endif

clean:
ifeq ($(BUILD_LINUX),True)
	$(call cleanin,alsa)
endif
ifeq ($(BUILD_OSX),True)
	$(call cleanin,coreaudio)
endif
ifeq ($(BUILD_FREEBSD),True)
	$(call cleanin,oss)
endif
ifeq ($(BUILD_OPENBSD),True)
	$(call cleanin,oss)
endif
ifeq ($(BUILD_NETBSD),True)
	$(call cleanin,oss)
endif
ifeq ($(BUILD_SOLARIS),True)
	$(call cleanin,oss)
endif
ifeq ($(BUILD_QNX),True)
	$(call cleanin,qsa)
endif
ifeq ($(BUILD_WIN32),True)
	$(call cleanin,directsound)
	$(call cleanin,waveout)
	$(call cleanin,winamp)
endif

install:
ifeq ($(BUILD_LINUX),True)
	$(call makein,alsa,install)
endif
ifeq ($(BUILD_OSX),True)
	$(call makein,coreaudio,install)
endif
ifeq ($(BUILD_FREEBSD),True)
	$(call makein,oss,install)
endif
ifeq ($(BUILD_OPENBSD),True)
	$(call makein,oss,install)
endif
ifeq ($(BUILD_NETBSD),True)
	$(call makein,oss,install)
endif
ifeq ($(BUILD_SOLARIS),True)
	$(call makein,oss,install)
endif
ifeq ($(BUILD_QNX),True)
	$(call makein,qsa,install)
endif

uninstall:
ifeq ($(BUILD_LINUX),True)
	$(call makein,alsa,uninstall)
endif
ifeq ($(BUILD_OSX),True)
	$(call makein,coreaudio,uninstall)
endif
ifeq ($(BUILD_FREEBSD),True)
	$(call makein,oss,uninstall)
endif
ifeq ($(BUILD_OPENBSD),True)
	$(call makein,oss,uninstall)
endif
ifeq ($(BUILD_NETBSD),True)
	$(call makein,oss,uninstall)
endif
ifeq ($(BUILD_SOLARIS),True)
	$(call makein,oss,uninstall)
endif
ifeq ($(BUILD_QNX),True)
	$(call makein,qsa,uninstall)
endif
