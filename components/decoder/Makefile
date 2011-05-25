########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(BOCA_PATH)/Makefile-options

all:
	cd aiff && $(MAKE)
	cd au && $(MAKE)
	cd bonk && $(MAKE)
ifeq ($(BUILD_UNIX),True)
ifneq ($(BUILD_OSX),True)
ifneq ($(BUILD_QNX),True)
ifneq ($(BUILD_SOLARIS),True)
ifneq ($(BUILD_OPENBSD),True)
	cd cdio && $(MAKE)
endif
ifneq ($(BUILD_NETBSD),True)
ifneq ($(BUILD_HAIKU),True)
	cd cdparanoia && $(MAKE)
endif
endif
endif
endif
endif
endif
ifeq ($(BUILD_WIN32),True)
	cd cdrip && $(MAKE)
endif
	cd cuesheet && $(MAKE)
	cd faad2 && $(MAKE)
	cd flac && $(MAKE)
	cd lame && $(MAKE)
ifeq ($(BUILD_WIN32),True)
	cd mac && $(MAKE)
endif
	cd mad && $(MAKE)
	cd sndfile && $(MAKE)
	cd speex && $(MAKE)
ifeq ($(BUILD_WIN32),True)
	cd twinvq && $(MAKE)
endif
	cd voc && $(MAKE)
	cd vorbis && $(MAKE)
	cd wave && $(MAKE)
ifeq ($(BUILD_WIN32),True)
	cd winamp && $(MAKE)
	cd wma && $(MAKE)
endif

clean:
	cd aiff && $(MAKE) clean
	cd au && $(MAKE) clean
	cd bonk && $(MAKE) clean
ifeq ($(BUILD_UNIX),True)
ifneq ($(BUILD_OSX),True)
ifneq ($(BUILD_QNX),True)
ifneq ($(BUILD_SOLARIS),True)
ifneq ($(BUILD_OPENBSD),True)
	cd cdio && $(MAKE) clean
endif
ifneq ($(BUILD_NETBSD),True)
ifneq ($(BUILD_HAIKU),True)
	cd cdparanoia && $(MAKE) clean
endif
endif
endif
endif
endif
endif
ifeq ($(BUILD_WIN32),True)
	cd cdrip && $(MAKE) clean
endif
	cd cuesheet && $(MAKE) clean
	cd faad2 && $(MAKE) clean
	cd flac && $(MAKE) clean
	cd lame && $(MAKE) clean
ifeq ($(BUILD_WIN32),True)
	cd mac && $(MAKE) clean
endif
	cd mad && $(MAKE) clean
	cd sndfile && $(MAKE) clean
	cd speex && $(MAKE) clean
ifeq ($(BUILD_WIN32),True)
	cd twinvq && $(MAKE) clean
endif
	cd voc && $(MAKE) clean
	cd vorbis && $(MAKE) clean
	cd wave && $(MAKE) clean
ifeq ($(BUILD_WIN32),True)
	cd winamp && $(MAKE) clean
	cd wma && $(MAKE) clean
endif
