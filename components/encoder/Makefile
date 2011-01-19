########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(BOCA_PATH)/Makefile-options

all:
ifeq ($(BUILD_WIN32),True)
	cd bladeenc && $(MAKE)
endif
	cd bonk && $(MAKE)
	cd faac && $(MAKE)
	cd flac && $(MAKE)
	cd lame && $(MAKE)
ifeq ($(BUILD_WIN32),True)
	cd mac && $(MAKE)
endif
	cd speex && $(MAKE)
ifeq ($(BUILD_WIN32),True)
	cd twinvq && $(MAKE)
endif
	cd vorbis && $(MAKE)
	cd wave && $(MAKE)
ifeq ($(BUILD_WIN32),True)
	cd wma && $(MAKE)
endif

clean:
ifeq ($(BUILD_WIN32),True)
	cd bladeenc && $(MAKE) clean
endif
	cd bonk && $(MAKE) clean
	cd faac && $(MAKE) clean
	cd flac && $(MAKE) clean
	cd lame && $(MAKE) clean
ifeq ($(BUILD_WIN32),True)
	cd mac && $(MAKE) clean
endif
	cd speex && $(MAKE) clean
ifeq ($(BUILD_WIN32),True)
	cd twinvq && $(MAKE) clean
endif
	cd vorbis && $(MAKE) clean
	cd wave && $(MAKE) clean
ifeq ($(BUILD_WIN32),True)
	cd wma && $(MAKE) clean
endif
