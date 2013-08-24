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
ifeq ($(BUILD_OSX),True)
	cd tocplist && $(MAKE)
endif
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
ifeq ($(BUILD_OSX),True)
	cd tocplist && $(MAKE) clean
endif
	cd vorbis && $(MAKE) clean
ifeq ($(BUILD_WIN32),True)
	cd wma && $(MAKE) clean
endif

install:
	cd apev2 && $(MAKE) install
	cd cart && $(MAKE) install
	cd flac && $(MAKE) install
	cd id3v1 && $(MAKE) install
	cd id3v2 && $(MAKE) install
	cd mp4 && $(MAKE) install
	cd riff && $(MAKE) install
ifeq ($(BUILD_OSX),True)
	cd tocplist && $(MAKE) install
endif
	cd vorbis && $(MAKE) install

uninstall:
	cd apev2 && $(MAKE) uninstall
	cd cart && $(MAKE) uninstall
	cd flac && $(MAKE) uninstall
	cd id3v1 && $(MAKE) uninstall
	cd id3v2 && $(MAKE) uninstall
	cd mp4 && $(MAKE) uninstall
	cd riff && $(MAKE) uninstall
ifeq ($(BUILD_OSX),True)
	cd tocplist && $(MAKE) uninstall
endif
	cd vorbis && $(MAKE) uninstall
