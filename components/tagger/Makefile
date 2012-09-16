########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(BOCA_PATH)/Makefile-options

all:
	cd apev2 && $(MAKE)
	cd cart && $(MAKE)
	cd flac && $(MAKE)
	cd id3v1 && $(MAKE)
	cd id3v2 && $(MAKE)
	cd mp4 && $(MAKE)
	cd riff && $(MAKE)
	cd vorbis && $(MAKE)
ifeq ($(BUILD_WIN32),True)
	cd wma && $(MAKE)
endif

clean:
	cd apev2 && $(MAKE) clean
	cd cart && $(MAKE) clean
	cd flac && $(MAKE) clean
	cd id3v1 && $(MAKE) clean
	cd id3v2 && $(MAKE) clean
	cd mp4 && $(MAKE) clean
	cd riff && $(MAKE) clean
	cd vorbis && $(MAKE) clean
ifeq ($(BUILD_WIN32),True)
	cd wma && $(MAKE) clean
endif
