########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(BOCA_PATH)/Makefile-options

all:
ifeq ($(BUILD_WIN32),True)
	cd bladeenc && $(MAKE)
endif
	cd bonk && $(MAKE)
ifeq ($(BUILD_OSX),True)
	cd coreaudio && $(MAKE)
endif
ifeq ($(BUILD_WIN32),True)
	cd coreaudio && $(MAKE)
	cd coreaudioconnect && $(MAKE)
endif
	cd faac && $(MAKE)
	cd fdkaac && $(MAKE)
	cd flac && $(MAKE)
	cd lame && $(MAKE)
ifeq ($(BUILD_WIN32),True)
	cd mac && $(MAKE)
endif
	cd meh && $(MAKE)
	cd opus && $(MAKE)
	cd sndfile && $(MAKE)
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
ifeq ($(BUILD_OSX),True)
	cd coreaudio && $(MAKE) clean
endif
ifeq ($(BUILD_WIN32),True)
	cd coreaudio && $(MAKE) clean
	cd coreaudioconnect && $(MAKE) clean
endif
	cd faac && $(MAKE) clean
	cd fdkaac && $(MAKE) clean
	cd flac && $(MAKE) clean
	cd lame && $(MAKE) clean
ifeq ($(BUILD_WIN32),True)
	cd mac && $(MAKE) clean
endif
	cd meh && $(MAKE) clean
	cd opus && $(MAKE) clean
	cd sndfile && $(MAKE) clean
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
ifeq ($(BUILD_OSX),True)
	cd coreaudio && $(MAKE) install
endif
ifneq ($(BUILD_OSX),True)
	cd faac && $(MAKE) install
endif
	cd fdkaac && $(MAKE) install
	cd flac && $(MAKE) install
	cd lame && $(MAKE) install
	cd meh && $(MAKE) install
	cd opus && $(MAKE) install
	cd sndfile && $(MAKE) install
	cd speex && $(MAKE) install
ifneq ($(BUILD_OSX),True)
	cd voaacenc && $(MAKE) install
endif
	cd vorbis && $(MAKE) install

uninstall:
ifeq ($(BUILD_OSX),True)
	cd coreaudio && $(MAKE) uninstall
endif
ifneq ($(BUILD_OSX),True)
	cd faac && $(MAKE) uninstall
endif
	cd fdkaac && $(MAKE) uninstall
	cd flac && $(MAKE) uninstall
	cd lame && $(MAKE) uninstall
	cd meh && $(MAKE) uninstall
	cd opus && $(MAKE) uninstall
	cd sndfile && $(MAKE) uninstall
	cd speex && $(MAKE) uninstall
ifneq ($(BUILD_OSX),True)
	cd voaacenc && $(MAKE) uninstall
endif
	cd vorbis && $(MAKE) uninstall
