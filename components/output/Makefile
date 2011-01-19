########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(BOCA_PATH)/Makefile-options

all:
ifeq ($(BUILD_LINUX),True)
	cd alsa && $(MAKE)
endif
ifeq ($(BUILD_WIN32),True)
	cd waveout && $(MAKE)
	cd winamp && $(MAKE)
endif

clean:
ifeq ($(BUILD_LINUX),True)
	cd alsa && $(MAKE) clean
endif
ifeq ($(BUILD_WIN32),True)
	cd waveout && $(MAKE) clean
	cd winamp && $(MAKE) clean
endif
