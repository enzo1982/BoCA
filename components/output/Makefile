########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(BOCA_PATH)/Makefile-options

all:
ifeq ($(BUILD_LINUX),True)
	cd alsa && $(MAKE)
endif
ifeq ($(BUILD_OSX),True)
	cd coreaudio && $(MAKE)
endif
ifeq ($(BUILD_FREEBSD),True)
	cd oss && $(MAKE)
endif
ifeq ($(BUILD_OPENBSD),True)
	cd oss && $(MAKE)
endif
ifeq ($(BUILD_NETBSD),True)
	cd oss && $(MAKE)
endif
ifeq ($(BUILD_SOLARIS),True)
	cd oss && $(MAKE)
endif
ifeq ($(BUILD_QNX),True)
	cd qsa && $(MAKE)
endif
ifeq ($(BUILD_WIN32),True)
	cd waveout && $(MAKE)
	cd winamp && $(MAKE)
endif

clean:
ifeq ($(BUILD_LINUX),True)
	cd alsa && $(MAKE) clean
endif
ifeq ($(BUILD_OSX),True)
	cd coreaudio && $(MAKE) clean
endif
ifeq ($(BUILD_FREEBSD),True)
	cd oss && $(MAKE) clean
endif
ifeq ($(BUILD_OPENBSD),True)
	cd oss && $(MAKE) clean
endif
ifeq ($(BUILD_NETBSD),True)
	cd oss && $(MAKE) clean
endif
ifeq ($(BUILD_SOLARIS),True)
	cd oss && $(MAKE) clean
endif
ifeq ($(BUILD_QNX),True)
	cd qsa && $(MAKE) clean
endif
ifeq ($(BUILD_WIN32),True)
	cd waveout && $(MAKE) clean
	cd winamp && $(MAKE) clean
endif
