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
	cd opus && $(MAKE)
	cd speex && $(MAKE)
ifeq ($(BUILD_WIN32),True)
	cd twinvq && $(MAKE)
endif
	cd voaacenc && $(MAKE)
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
	cd opus && $(MAKE) clean
	cd speex && $(MAKE) clean
ifeq ($(BUILD_WIN32),True)
	cd twinvq && $(MAKE) clean
endif
	cd voaacenc && $(MAKE) clean
	cd vorbis && $(MAKE) clean
	cd wave && $(MAKE) clean
ifeq ($(BUILD_WIN32),True)
	cd wma && $(MAKE) clean
endif

install:
	cd faac && $(MAKE) install
	cd flac && $(MAKE) install
	cd lame && $(MAKE) install
	cd opus && $(MAKE) install
	cd speex && $(MAKE) install
	cd voaacenc && $(MAKE) install
	cd vorbis && $(MAKE) install
	cd wave && $(MAKE) install

uninstall:
	cd faac && $(MAKE) uninstall
	cd flac && $(MAKE) uninstall
	cd lame && $(MAKE) uninstall
	cd opus && $(MAKE) uninstall
	cd speex && $(MAKE) uninstall
	cd voaacenc && $(MAKE) uninstall
	cd vorbis && $(MAKE) uninstall
	cd wave && $(MAKE) uninstall
