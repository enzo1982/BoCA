########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(BOCA_PATH)/Makefile-options

all:
ifeq ($(BUILD_UNIX),True)
ifneq ($(BUILD_OSX),True)
	cd cdparanoia && $(MAKE)
endif
endif
ifeq ($(BUILD_WIN32),True)
	cd cdrip && $(MAKE)
endif

clean:
ifeq ($(BUILD_UNIX),True)
ifneq ($(BUILD_OSX),True)
	cd cdparanoia && $(MAKE) clean
endif
endif
ifeq ($(BUILD_WIN32),True)
	cd cdrip && $(MAKE) clean
endif
